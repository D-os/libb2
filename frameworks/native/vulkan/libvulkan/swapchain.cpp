/*
 * Copyright 2015 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <algorithm>

#include <gui/BufferQueue.h>
#include <log/log.h>
#include <sync/sync.h>
#include <utils/StrongPointer.h>

#include "driver.h"

// TODO(jessehall): Currently we don't have a good error code for when a native
// window operation fails. Just returning INITIALIZATION_FAILED for now. Later
// versions (post SDK 0.9) of the API/extension have a better error code.
// When updating to that version, audit all error returns.
namespace vulkan {
namespace driver {

namespace {

const VkSurfaceTransformFlagsKHR kSupportedTransforms =
    VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR |
    VK_SURFACE_TRANSFORM_ROTATE_90_BIT_KHR |
    VK_SURFACE_TRANSFORM_ROTATE_180_BIT_KHR |
    VK_SURFACE_TRANSFORM_ROTATE_270_BIT_KHR |
    // TODO(jessehall): See TODO in TranslateNativeToVulkanTransform.
    // VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_BIT_KHR |
    // VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_90_BIT_KHR |
    // VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_180_BIT_KHR |
    // VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_270_BIT_KHR |
    VK_SURFACE_TRANSFORM_INHERIT_BIT_KHR;

VkSurfaceTransformFlagBitsKHR TranslateNativeToVulkanTransform(int native) {
    // Native and Vulkan transforms are isomorphic, but are represented
    // differently. Vulkan transforms are built up of an optional horizontal
    // mirror, followed by a clockwise 0/90/180/270-degree rotation. Native
    // transforms are built up from a horizontal flip, vertical flip, and
    // 90-degree rotation, all optional but always in that order.

    // TODO(jessehall): For now, only support pure rotations, not
    // flip or flip-and-rotate, until I have more time to test them and build
    // sample code. As far as I know we never actually use anything besides
    // pure rotations anyway.

    switch (native) {
        case 0:  // 0x0
            return VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        // case NATIVE_WINDOW_TRANSFORM_FLIP_H:  // 0x1
        //     return VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_BIT_KHR;
        // case NATIVE_WINDOW_TRANSFORM_FLIP_V:  // 0x2
        //     return VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_180_BIT_KHR;
        case NATIVE_WINDOW_TRANSFORM_ROT_180:  // FLIP_H | FLIP_V
            return VK_SURFACE_TRANSFORM_ROTATE_180_BIT_KHR;
        case NATIVE_WINDOW_TRANSFORM_ROT_90:  // 0x4
            return VK_SURFACE_TRANSFORM_ROTATE_90_BIT_KHR;
        // case NATIVE_WINDOW_TRANSFORM_FLIP_H | NATIVE_WINDOW_TRANSFORM_ROT_90:
        //     return VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_90_BIT_KHR;
        // case NATIVE_WINDOW_TRANSFORM_FLIP_V | NATIVE_WINDOW_TRANSFORM_ROT_90:
        //     return VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_270_BIT_KHR;
        case NATIVE_WINDOW_TRANSFORM_ROT_270:  // FLIP_H | FLIP_V | ROT_90
            return VK_SURFACE_TRANSFORM_ROTATE_270_BIT_KHR;
        case NATIVE_WINDOW_TRANSFORM_INVERSE_DISPLAY:
        default:
            return VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    }
}

int InvertTransformToNative(VkSurfaceTransformFlagBitsKHR transform) {
    switch (transform) {
        case VK_SURFACE_TRANSFORM_ROTATE_90_BIT_KHR:
            return NATIVE_WINDOW_TRANSFORM_ROT_270;
        case VK_SURFACE_TRANSFORM_ROTATE_180_BIT_KHR:
            return NATIVE_WINDOW_TRANSFORM_ROT_180;
        case VK_SURFACE_TRANSFORM_ROTATE_270_BIT_KHR:
            return NATIVE_WINDOW_TRANSFORM_ROT_90;
        // TODO(jessehall): See TODO in TranslateNativeToVulkanTransform.
        // case VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_BIT_KHR:
        //     return NATIVE_WINDOW_TRANSFORM_FLIP_H;
        // case VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_90_BIT_KHR:
        //     return NATIVE_WINDOW_TRANSFORM_FLIP_H |
        //            NATIVE_WINDOW_TRANSFORM_ROT_90;
        // case VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_180_BIT_KHR:
        //     return NATIVE_WINDOW_TRANSFORM_FLIP_V;
        // case VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_270_BIT_KHR:
        //     return NATIVE_WINDOW_TRANSFORM_FLIP_V |
        //            NATIVE_WINDOW_TRANSFORM_ROT_90;
        case VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR:
        case VK_SURFACE_TRANSFORM_INHERIT_BIT_KHR:
        default:
            return 0;
    }
}

// ----------------------------------------------------------------------------

struct Surface {
    android::sp<ANativeWindow> window;
    VkSwapchainKHR swapchain_handle;
};

VkSurfaceKHR HandleFromSurface(Surface* surface) {
    return VkSurfaceKHR(reinterpret_cast<uint64_t>(surface));
}

Surface* SurfaceFromHandle(VkSurfaceKHR handle) {
    return reinterpret_cast<Surface*>(handle);
}

struct Swapchain {
    Swapchain(Surface& surface_, uint32_t num_images_)
        : surface(surface_), num_images(num_images_) {}

    Surface& surface;
    uint32_t num_images;

    struct Image {
        Image() : image(VK_NULL_HANDLE), dequeue_fence(-1), dequeued(false) {}
        VkImage image;
        android::sp<ANativeWindowBuffer> buffer;
        // The fence is only valid when the buffer is dequeued, and should be
        // -1 any other time. When valid, we own the fd, and must ensure it is
        // closed: either by closing it explicitly when queueing the buffer,
        // or by passing ownership e.g. to ANativeWindow::cancelBuffer().
        int dequeue_fence;
        bool dequeued;
    } images[android::BufferQueue::NUM_BUFFER_SLOTS];
};

VkSwapchainKHR HandleFromSwapchain(Swapchain* swapchain) {
    return VkSwapchainKHR(reinterpret_cast<uint64_t>(swapchain));
}

Swapchain* SwapchainFromHandle(VkSwapchainKHR handle) {
    return reinterpret_cast<Swapchain*>(handle);
}

void ReleaseSwapchainImage(VkDevice device,
                           ANativeWindow* window,
                           int release_fence,
                           Swapchain::Image& image) {
    ALOG_ASSERT(release_fence == -1 || image.dequeued,
                "ReleaseSwapchainImage: can't provide a release fence for "
                "non-dequeued images");

    if (image.dequeued) {
        if (release_fence >= 0) {
            // We get here from vkQueuePresentKHR. The application is
            // responsible for creating an execution dependency chain from
            // vkAcquireNextImage (dequeue_fence) to vkQueuePresentKHR
            // (release_fence), so we can drop the dequeue_fence here.
            if (image.dequeue_fence >= 0)
                close(image.dequeue_fence);
        } else {
            // We get here during swapchain destruction, or various serious
            // error cases e.g. when we can't create the release_fence during
            // vkQueuePresentKHR. In non-error cases, the dequeue_fence should
            // have already signalled, since the swapchain images are supposed
            // to be idle before the swapchain is destroyed. In error cases,
            // there may be rendering in flight to the image, but since we
            // weren't able to create a release_fence, waiting for the
            // dequeue_fence is about the best we can do.
            release_fence = image.dequeue_fence;
        }
        image.dequeue_fence = -1;

        if (window) {
            window->cancelBuffer(window, image.buffer.get(), release_fence);
        } else {
            if (release_fence >= 0) {
                sync_wait(release_fence, -1 /* forever */);
                close(release_fence);
            }
        }

        image.dequeued = false;
    }

    if (image.image) {
        GetData(device).driver.DestroyImage(device, image.image, nullptr);
        image.image = VK_NULL_HANDLE;
    }

    image.buffer.clear();
}

void OrphanSwapchain(VkDevice device, Swapchain* swapchain) {
    if (swapchain->surface.swapchain_handle != HandleFromSwapchain(swapchain))
        return;
    for (uint32_t i = 0; i < swapchain->num_images; i++) {
        if (!swapchain->images[i].dequeued)
            ReleaseSwapchainImage(device, nullptr, -1, swapchain->images[i]);
    }
    swapchain->surface.swapchain_handle = VK_NULL_HANDLE;
}

}  // anonymous namespace

VKAPI_ATTR
VkResult CreateAndroidSurfaceKHR(
    VkInstance instance,
    const VkAndroidSurfaceCreateInfoKHR* pCreateInfo,
    const VkAllocationCallbacks* allocator,
    VkSurfaceKHR* out_surface) {
    if (!allocator)
        allocator = &GetData(instance).allocator;
    void* mem = allocator->pfnAllocation(allocator->pUserData, sizeof(Surface),
                                         alignof(Surface),
                                         VK_SYSTEM_ALLOCATION_SCOPE_OBJECT);
    if (!mem)
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    Surface* surface = new (mem) Surface;

    surface->window = pCreateInfo->window;
    surface->swapchain_handle = VK_NULL_HANDLE;

    // TODO(jessehall): Create and use NATIVE_WINDOW_API_VULKAN.
    int err =
        native_window_api_connect(surface->window.get(), NATIVE_WINDOW_API_EGL);
    if (err != 0) {
        // TODO(jessehall): Improve error reporting. Can we enumerate possible
        // errors and translate them to valid Vulkan result codes?
        ALOGE("native_window_api_connect() failed: %s (%d)", strerror(-err),
              err);
        surface->~Surface();
        allocator->pfnFree(allocator->pUserData, surface);
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    *out_surface = HandleFromSurface(surface);
    return VK_SUCCESS;
}

VKAPI_ATTR
void DestroySurfaceKHR(VkInstance instance,
                       VkSurfaceKHR surface_handle,
                       const VkAllocationCallbacks* allocator) {
    Surface* surface = SurfaceFromHandle(surface_handle);
    if (!surface)
        return;
    native_window_api_disconnect(surface->window.get(), NATIVE_WINDOW_API_EGL);
    ALOGV_IF(surface->swapchain_handle != VK_NULL_HANDLE,
             "destroyed VkSurfaceKHR 0x%" PRIx64
             " has active VkSwapchainKHR 0x%" PRIx64,
             reinterpret_cast<uint64_t>(surface_handle),
             reinterpret_cast<uint64_t>(surface->swapchain_handle));
    surface->~Surface();
    if (!allocator)
        allocator = &GetData(instance).allocator;
    allocator->pfnFree(allocator->pUserData, surface);
}

VKAPI_ATTR
VkResult GetPhysicalDeviceSurfaceSupportKHR(VkPhysicalDevice /*pdev*/,
                                            uint32_t /*queue_family*/,
                                            VkSurfaceKHR /*surface*/,
                                            VkBool32* supported) {
    *supported = VK_TRUE;
    return VK_SUCCESS;
}

VKAPI_ATTR
VkResult GetPhysicalDeviceSurfaceCapabilitiesKHR(
    VkPhysicalDevice /*pdev*/,
    VkSurfaceKHR surface,
    VkSurfaceCapabilitiesKHR* capabilities) {
    int err;
    ANativeWindow* window = SurfaceFromHandle(surface)->window.get();

    int width, height;
    err = window->query(window, NATIVE_WINDOW_DEFAULT_WIDTH, &width);
    if (err != 0) {
        ALOGE("NATIVE_WINDOW_DEFAULT_WIDTH query failed: %s (%d)",
              strerror(-err), err);
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    err = window->query(window, NATIVE_WINDOW_DEFAULT_HEIGHT, &height);
    if (err != 0) {
        ALOGE("NATIVE_WINDOW_DEFAULT_WIDTH query failed: %s (%d)",
              strerror(-err), err);
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    int transform_hint;
    err = window->query(window, NATIVE_WINDOW_TRANSFORM_HINT, &transform_hint);
    if (err != 0) {
        ALOGE("NATIVE_WINDOW_TRANSFORM_HINT query failed: %s (%d)",
              strerror(-err), err);
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    // TODO(jessehall): Figure out what the min/max values should be.
    capabilities->minImageCount = 2;
    capabilities->maxImageCount = 3;

    capabilities->currentExtent =
        VkExtent2D{static_cast<uint32_t>(width), static_cast<uint32_t>(height)};

    // TODO(jessehall): Figure out what the max extent should be. Maximum
    // texture dimension maybe?
    capabilities->minImageExtent = VkExtent2D{1, 1};
    capabilities->maxImageExtent = VkExtent2D{4096, 4096};

    capabilities->maxImageArrayLayers = 1;

    capabilities->supportedTransforms = kSupportedTransforms;
    capabilities->currentTransform =
        TranslateNativeToVulkanTransform(transform_hint);

    // On Android, window composition is a WindowManager property, not something
    // associated with the bufferqueue. It can't be changed from here.
    capabilities->supportedCompositeAlpha = VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;

    // TODO(jessehall): I think these are right, but haven't thought hard about
    // it. Do we need to query the driver for support of any of these?
    // Currently not included:
    // - VK_IMAGE_USAGE_GENERAL: maybe? does this imply cpu mappable?
    // - VK_IMAGE_USAGE_DEPTH_STENCIL_BIT: definitely not
    // - VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT: definitely not
    capabilities->supportedUsageFlags =
        VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
        VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT |
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
        VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;

    return VK_SUCCESS;
}

VKAPI_ATTR
VkResult GetPhysicalDeviceSurfaceFormatsKHR(VkPhysicalDevice /*pdev*/,
                                            VkSurfaceKHR /*surface*/,
                                            uint32_t* count,
                                            VkSurfaceFormatKHR* formats) {
    // TODO(jessehall): Fill out the set of supported formats. Longer term, add
    // a new gralloc method to query whether a (format, usage) pair is
    // supported, and check that for each gralloc format that corresponds to a
    // Vulkan format. Shorter term, just add a few more formats to the ones
    // hardcoded below.

    const VkSurfaceFormatKHR kFormats[] = {
        {VK_FORMAT_R8G8B8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR},
        {VK_FORMAT_R8G8B8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR},
        {VK_FORMAT_R5G6B5_UNORM_PACK16, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR},
    };
    const uint32_t kNumFormats = sizeof(kFormats) / sizeof(kFormats[0]);

    VkResult result = VK_SUCCESS;
    if (formats) {
        if (*count < kNumFormats)
            result = VK_INCOMPLETE;
        std::copy(kFormats, kFormats + std::min(*count, kNumFormats), formats);
    }
    *count = kNumFormats;
    return result;
}

VKAPI_ATTR
VkResult GetPhysicalDeviceSurfacePresentModesKHR(VkPhysicalDevice /*pdev*/,
                                                 VkSurfaceKHR /*surface*/,
                                                 uint32_t* count,
                                                 VkPresentModeKHR* modes) {
    const VkPresentModeKHR kModes[] = {
        VK_PRESENT_MODE_MAILBOX_KHR, VK_PRESENT_MODE_FIFO_KHR,
    };
    const uint32_t kNumModes = sizeof(kModes) / sizeof(kModes[0]);

    VkResult result = VK_SUCCESS;
    if (modes) {
        if (*count < kNumModes)
            result = VK_INCOMPLETE;
        std::copy(kModes, kModes + std::min(*count, kNumModes), modes);
    }
    *count = kNumModes;
    return result;
}

VKAPI_ATTR
VkResult CreateSwapchainKHR(VkDevice device,
                            const VkSwapchainCreateInfoKHR* create_info,
                            const VkAllocationCallbacks* allocator,
                            VkSwapchainKHR* swapchain_handle) {
    int err;
    VkResult result = VK_SUCCESS;

    ALOGV("vkCreateSwapchainKHR: surface=0x%" PRIx64
          " minImageCount=%u imageFormat=%u imageColorSpace=%u"
          " imageExtent=%ux%u imageUsage=%#x preTransform=%u presentMode=%u"
          " oldSwapchain=0x%" PRIx64,
          reinterpret_cast<uint64_t>(create_info->surface),
          create_info->minImageCount, create_info->imageFormat,
          create_info->imageColorSpace, create_info->imageExtent.width,
          create_info->imageExtent.height, create_info->imageUsage,
          create_info->preTransform, create_info->presentMode,
          reinterpret_cast<uint64_t>(create_info->oldSwapchain));

    if (!allocator)
        allocator = &GetData(device).allocator;

    ALOGV_IF(create_info->imageArrayLayers != 1,
             "swapchain imageArrayLayers=%u not supported",
             create_info->imageArrayLayers);
    ALOGV_IF(create_info->imageColorSpace != VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
             "swapchain imageColorSpace=%u not supported",
             create_info->imageColorSpace);
    ALOGV_IF((create_info->preTransform & ~kSupportedTransforms) != 0,
             "swapchain preTransform=%#x not supported",
             create_info->preTransform);
    ALOGV_IF(!(create_info->presentMode == VK_PRESENT_MODE_FIFO_KHR ||
               create_info->presentMode == VK_PRESENT_MODE_MAILBOX_KHR),
             "swapchain presentMode=%u not supported",
             create_info->presentMode);

    Surface& surface = *SurfaceFromHandle(create_info->surface);

    if (surface.swapchain_handle != create_info->oldSwapchain) {
        ALOGV("Can't create a swapchain for VkSurfaceKHR 0x%" PRIx64
              " because it already has active swapchain 0x%" PRIx64
              " but VkSwapchainCreateInfo::oldSwapchain=0x%" PRIx64,
              reinterpret_cast<uint64_t>(create_info->surface),
              reinterpret_cast<uint64_t>(surface.swapchain_handle),
              reinterpret_cast<uint64_t>(create_info->oldSwapchain));
        return VK_ERROR_NATIVE_WINDOW_IN_USE_KHR;
    }
    if (create_info->oldSwapchain != VK_NULL_HANDLE)
        OrphanSwapchain(device, SwapchainFromHandle(create_info->oldSwapchain));

    // -- Reset the native window --
    // The native window might have been used previously, and had its properties
    // changed from defaults. That will affect the answer we get for queries
    // like MIN_UNDEQUED_BUFFERS. Reset to a known/default state before we
    // attempt such queries.

    // The native window only allows dequeueing all buffers before any have
    // been queued, since after that point at least one is assumed to be in
    // non-FREE state at any given time. Disconnecting and re-connecting
    // orphans the previous buffers, getting us back to the state where we can
    // dequeue all buffers.
    err = native_window_api_disconnect(surface.window.get(),
                                       NATIVE_WINDOW_API_EGL);
    ALOGW_IF(err != 0, "native_window_api_disconnect failed: %s (%d)",
             strerror(-err), err);
    err =
        native_window_api_connect(surface.window.get(), NATIVE_WINDOW_API_EGL);
    ALOGW_IF(err != 0, "native_window_api_connect failed: %s (%d)",
             strerror(-err), err);

    err = native_window_set_buffer_count(surface.window.get(), 0);
    if (err != 0) {
        ALOGE("native_window_set_buffer_count(0) failed: %s (%d)",
              strerror(-err), err);
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    err = surface.window->setSwapInterval(surface.window.get(), 1);
    if (err != 0) {
        // TODO(jessehall): Improve error reporting. Can we enumerate possible
        // errors and translate them to valid Vulkan result codes?
        ALOGE("native_window->setSwapInterval(1) failed: %s (%d)",
              strerror(-err), err);
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    // -- Configure the native window --

    const auto& dispatch = GetData(device).driver;

    int native_format = HAL_PIXEL_FORMAT_RGBA_8888;
    switch (create_info->imageFormat) {
        case VK_FORMAT_R8G8B8A8_UNORM:
        case VK_FORMAT_R8G8B8A8_SRGB:
            native_format = HAL_PIXEL_FORMAT_RGBA_8888;
            break;
        case VK_FORMAT_R5G6B5_UNORM_PACK16:
            native_format = HAL_PIXEL_FORMAT_RGB_565;
            break;
        default:
            ALOGV("unsupported swapchain format %d", create_info->imageFormat);
            break;
    }
    err = native_window_set_buffers_format(surface.window.get(), native_format);
    if (err != 0) {
        // TODO(jessehall): Improve error reporting. Can we enumerate possible
        // errors and translate them to valid Vulkan result codes?
        ALOGE("native_window_set_buffers_format(%d) failed: %s (%d)",
              native_format, strerror(-err), err);
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    err = native_window_set_buffers_data_space(surface.window.get(),
                                               HAL_DATASPACE_SRGB_LINEAR);
    if (err != 0) {
        // TODO(jessehall): Improve error reporting. Can we enumerate possible
        // errors and translate them to valid Vulkan result codes?
        ALOGE("native_window_set_buffers_data_space(%d) failed: %s (%d)",
              HAL_DATASPACE_SRGB_LINEAR, strerror(-err), err);
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    err = native_window_set_buffers_dimensions(
        surface.window.get(), static_cast<int>(create_info->imageExtent.width),
        static_cast<int>(create_info->imageExtent.height));
    if (err != 0) {
        // TODO(jessehall): Improve error reporting. Can we enumerate possible
        // errors and translate them to valid Vulkan result codes?
        ALOGE("native_window_set_buffers_dimensions(%d,%d) failed: %s (%d)",
              create_info->imageExtent.width, create_info->imageExtent.height,
              strerror(-err), err);
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    // VkSwapchainCreateInfo::preTransform indicates the transformation the app
    // applied during rendering. native_window_set_transform() expects the
    // inverse: the transform the app is requesting that the compositor perform
    // during composition. With native windows, pre-transform works by rendering
    // with the same transform the compositor is applying (as in Vulkan), but
    // then requesting the inverse transform, so that when the compositor does
    // it's job the two transforms cancel each other out and the compositor ends
    // up applying an identity transform to the app's buffer.
    err = native_window_set_buffers_transform(
        surface.window.get(),
        InvertTransformToNative(create_info->preTransform));
    if (err != 0) {
        // TODO(jessehall): Improve error reporting. Can we enumerate possible
        // errors and translate them to valid Vulkan result codes?
        ALOGE("native_window_set_buffers_transform(%d) failed: %s (%d)",
              InvertTransformToNative(create_info->preTransform),
              strerror(-err), err);
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    err = native_window_set_scaling_mode(
        surface.window.get(), NATIVE_WINDOW_SCALING_MODE_SCALE_TO_WINDOW);
    if (err != 0) {
        // TODO(jessehall): Improve error reporting. Can we enumerate possible
        // errors and translate them to valid Vulkan result codes?
        ALOGE("native_window_set_scaling_mode(SCALE_TO_WINDOW) failed: %s (%d)",
              strerror(-err), err);
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    int query_value;
    err = surface.window->query(surface.window.get(),
                                NATIVE_WINDOW_MIN_UNDEQUEUED_BUFFERS,
                                &query_value);
    if (err != 0 || query_value < 0) {
        // TODO(jessehall): Improve error reporting. Can we enumerate possible
        // errors and translate them to valid Vulkan result codes?
        ALOGE("window->query failed: %s (%d) value=%d", strerror(-err), err,
              query_value);
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    uint32_t min_undequeued_buffers = static_cast<uint32_t>(query_value);
    // The MIN_UNDEQUEUED_BUFFERS query doesn't know whether we'll be using
    // async mode or not, and assumes not. But in async mode, the BufferQueue
    // requires an extra undequeued buffer.
    // See BufferQueueCore::getMinUndequeuedBufferCountLocked().
    if (create_info->presentMode == VK_PRESENT_MODE_MAILBOX_KHR)
        min_undequeued_buffers += 1;

    uint32_t num_images =
        (create_info->minImageCount - 1) + min_undequeued_buffers;
    err = native_window_set_buffer_count(surface.window.get(), num_images);
    if (err != 0) {
        // TODO(jessehall): Improve error reporting. Can we enumerate possible
        // errors and translate them to valid Vulkan result codes?
        ALOGE("native_window_set_buffer_count(%d) failed: %s (%d)", num_images,
              strerror(-err), err);
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    int gralloc_usage = 0;
    // TODO(jessehall): Remove conditional once all drivers have been updated
    if (dispatch.GetSwapchainGrallocUsageANDROID) {
        result = dispatch.GetSwapchainGrallocUsageANDROID(
            device, create_info->imageFormat, create_info->imageUsage,
            &gralloc_usage);
        if (result != VK_SUCCESS) {
            ALOGE("vkGetSwapchainGrallocUsageANDROID failed: %d", result);
            return VK_ERROR_INITIALIZATION_FAILED;
        }
    } else {
        gralloc_usage = GRALLOC_USAGE_HW_RENDER | GRALLOC_USAGE_HW_TEXTURE;
    }
    err = native_window_set_usage(surface.window.get(), gralloc_usage);
    if (err != 0) {
        // TODO(jessehall): Improve error reporting. Can we enumerate possible
        // errors and translate them to valid Vulkan result codes?
        ALOGE("native_window_set_usage failed: %s (%d)", strerror(-err), err);
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    int swap_interval =
        create_info->presentMode == VK_PRESENT_MODE_MAILBOX_KHR ? 0 : 1;
    err = surface.window->setSwapInterval(surface.window.get(), swap_interval);
    if (err != 0) {
        // TODO(jessehall): Improve error reporting. Can we enumerate possible
        // errors and translate them to valid Vulkan result codes?
        ALOGE("native_window->setSwapInterval(%d) failed: %s (%d)",
              swap_interval, strerror(-err), err);
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    // -- Allocate our Swapchain object --
    // After this point, we must deallocate the swapchain on error.

    void* mem = allocator->pfnAllocation(allocator->pUserData,
                                         sizeof(Swapchain), alignof(Swapchain),
                                         VK_SYSTEM_ALLOCATION_SCOPE_OBJECT);
    if (!mem)
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    Swapchain* swapchain = new (mem) Swapchain(surface, num_images);

    // -- Dequeue all buffers and create a VkImage for each --
    // Any failures during or after this must cancel the dequeued buffers.

    VkNativeBufferANDROID image_native_buffer = {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wold-style-cast"
        .sType = VK_STRUCTURE_TYPE_NATIVE_BUFFER_ANDROID,
#pragma clang diagnostic pop
        .pNext = nullptr,
    };
    VkImageCreateInfo image_create = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .pNext = &image_native_buffer,
        .imageType = VK_IMAGE_TYPE_2D,
        .format = create_info->imageFormat,
        .extent = {0, 0, 1},
        .mipLevels = 1,
        .arrayLayers = 1,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .usage = create_info->imageUsage,
        .flags = 0,
        .sharingMode = create_info->imageSharingMode,
        .queueFamilyIndexCount = create_info->queueFamilyIndexCount,
        .pQueueFamilyIndices = create_info->pQueueFamilyIndices,
    };

    for (uint32_t i = 0; i < num_images; i++) {
        Swapchain::Image& img = swapchain->images[i];

        ANativeWindowBuffer* buffer;
        err = surface.window->dequeueBuffer(surface.window.get(), &buffer,
                                            &img.dequeue_fence);
        if (err != 0) {
            // TODO(jessehall): Improve error reporting. Can we enumerate
            // possible errors and translate them to valid Vulkan result codes?
            ALOGE("dequeueBuffer[%u] failed: %s (%d)", i, strerror(-err), err);
            result = VK_ERROR_INITIALIZATION_FAILED;
            break;
        }
        img.buffer = buffer;
        img.dequeued = true;

        image_create.extent =
            VkExtent3D{static_cast<uint32_t>(img.buffer->width),
                       static_cast<uint32_t>(img.buffer->height),
                       1};
        image_native_buffer.handle = img.buffer->handle;
        image_native_buffer.stride = img.buffer->stride;
        image_native_buffer.format = img.buffer->format;
        image_native_buffer.usage = img.buffer->usage;

        result =
            dispatch.CreateImage(device, &image_create, nullptr, &img.image);
        if (result != VK_SUCCESS) {
            ALOGD("vkCreateImage w/ native buffer failed: %u", result);
            break;
        }
    }

    // -- Cancel all buffers, returning them to the queue --
    // If an error occurred before, also destroy the VkImage and release the
    // buffer reference. Otherwise, we retain a strong reference to the buffer.
    //
    // TODO(jessehall): The error path here is the same as DestroySwapchain,
    // but not the non-error path. Should refactor/unify.
    for (uint32_t i = 0; i < num_images; i++) {
        Swapchain::Image& img = swapchain->images[i];
        if (img.dequeued) {
            surface.window->cancelBuffer(surface.window.get(), img.buffer.get(),
                                         img.dequeue_fence);
            img.dequeue_fence = -1;
            img.dequeued = false;
        }
        if (result != VK_SUCCESS) {
            if (img.image)
                dispatch.DestroyImage(device, img.image, nullptr);
        }
    }

    if (result != VK_SUCCESS) {
        swapchain->~Swapchain();
        allocator->pfnFree(allocator->pUserData, swapchain);
        return result;
    }

    surface.swapchain_handle = HandleFromSwapchain(swapchain);
    *swapchain_handle = surface.swapchain_handle;
    return VK_SUCCESS;
}

VKAPI_ATTR
void DestroySwapchainKHR(VkDevice device,
                         VkSwapchainKHR swapchain_handle,
                         const VkAllocationCallbacks* allocator) {
    const auto& dispatch = GetData(device).driver;
    Swapchain* swapchain = SwapchainFromHandle(swapchain_handle);
    bool active = swapchain->surface.swapchain_handle == swapchain_handle;
    ANativeWindow* window = active ? swapchain->surface.window.get() : nullptr;

    for (uint32_t i = 0; i < swapchain->num_images; i++)
        ReleaseSwapchainImage(device, window, -1, swapchain->images[i]);
    if (active)
        swapchain->surface.swapchain_handle = VK_NULL_HANDLE;
    if (!allocator)
        allocator = &GetData(device).allocator;
    swapchain->~Swapchain();
    allocator->pfnFree(allocator->pUserData, swapchain);
}

VKAPI_ATTR
VkResult GetSwapchainImagesKHR(VkDevice,
                               VkSwapchainKHR swapchain_handle,
                               uint32_t* count,
                               VkImage* images) {
    Swapchain& swapchain = *SwapchainFromHandle(swapchain_handle);
    ALOGW_IF(swapchain.surface.swapchain_handle != swapchain_handle,
             "getting images for non-active swapchain 0x%" PRIx64
             "; only dequeued image handles are valid",
             reinterpret_cast<uint64_t>(swapchain_handle));
    VkResult result = VK_SUCCESS;
    if (images) {
        uint32_t n = swapchain.num_images;
        if (*count < swapchain.num_images) {
            n = *count;
            result = VK_INCOMPLETE;
        }
        for (uint32_t i = 0; i < n; i++)
            images[i] = swapchain.images[i].image;
    }
    *count = swapchain.num_images;
    return result;
}

VKAPI_ATTR
VkResult AcquireNextImageKHR(VkDevice device,
                             VkSwapchainKHR swapchain_handle,
                             uint64_t timeout,
                             VkSemaphore semaphore,
                             VkFence vk_fence,
                             uint32_t* image_index) {
    Swapchain& swapchain = *SwapchainFromHandle(swapchain_handle);
    ANativeWindow* window = swapchain.surface.window.get();
    VkResult result;
    int err;

    if (swapchain.surface.swapchain_handle != swapchain_handle)
        return VK_ERROR_OUT_OF_DATE_KHR;

    ALOGW_IF(
        timeout != UINT64_MAX,
        "vkAcquireNextImageKHR: non-infinite timeouts not yet implemented");

    ANativeWindowBuffer* buffer;
    int fence_fd;
    err = window->dequeueBuffer(window, &buffer, &fence_fd);
    if (err != 0) {
        // TODO(jessehall): Improve error reporting. Can we enumerate possible
        // errors and translate them to valid Vulkan result codes?
        ALOGE("dequeueBuffer failed: %s (%d)", strerror(-err), err);
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    uint32_t idx;
    for (idx = 0; idx < swapchain.num_images; idx++) {
        if (swapchain.images[idx].buffer.get() == buffer) {
            swapchain.images[idx].dequeued = true;
            swapchain.images[idx].dequeue_fence = fence_fd;
            break;
        }
    }
    if (idx == swapchain.num_images) {
        ALOGE("dequeueBuffer returned unrecognized buffer");
        window->cancelBuffer(window, buffer, fence_fd);
        return VK_ERROR_OUT_OF_DATE_KHR;
    }

    int fence_clone = -1;
    if (fence_fd != -1) {
        fence_clone = dup(fence_fd);
        if (fence_clone == -1) {
            ALOGE("dup(fence) failed, stalling until signalled: %s (%d)",
                  strerror(errno), errno);
            sync_wait(fence_fd, -1 /* forever */);
        }
    }

    result = GetData(device).driver.AcquireImageANDROID(
        device, swapchain.images[idx].image, fence_clone, semaphore, vk_fence);
    if (result != VK_SUCCESS) {
        // NOTE: we're relying on AcquireImageANDROID to close fence_clone,
        // even if the call fails. We could close it ourselves on failure, but
        // that would create a race condition if the driver closes it on a
        // failure path: some other thread might create an fd with the same
        // number between the time the driver closes it and the time we close
        // it. We must assume one of: the driver *always* closes it even on
        // failure, or *never* closes it on failure.
        window->cancelBuffer(window, buffer, fence_fd);
        swapchain.images[idx].dequeued = false;
        swapchain.images[idx].dequeue_fence = -1;
        return result;
    }

    *image_index = idx;
    return VK_SUCCESS;
}

static VkResult WorstPresentResult(VkResult a, VkResult b) {
    // See the error ranking for vkQueuePresentKHR at the end of section 29.6
    // (in spec version 1.0.14).
    static const VkResult kWorstToBest[] = {
        VK_ERROR_DEVICE_LOST,
        VK_ERROR_SURFACE_LOST_KHR,
        VK_ERROR_OUT_OF_DATE_KHR,
        VK_ERROR_OUT_OF_DEVICE_MEMORY,
        VK_ERROR_OUT_OF_HOST_MEMORY,
        VK_SUBOPTIMAL_KHR,
    };
    for (auto result : kWorstToBest) {
        if (a == result || b == result)
            return result;
    }
    ALOG_ASSERT(a == VK_SUCCESS, "invalid vkQueuePresentKHR result %d", a);
    ALOG_ASSERT(b == VK_SUCCESS, "invalid vkQueuePresentKHR result %d", b);
    return a != VK_SUCCESS ? a : b;
}

VKAPI_ATTR
VkResult QueuePresentKHR(VkQueue queue, const VkPresentInfoKHR* present_info) {
    ALOGV_IF(present_info->sType != VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
             "vkQueuePresentKHR: invalid VkPresentInfoKHR structure type %d",
             present_info->sType);
    ALOGV_IF(present_info->pNext, "VkPresentInfo::pNext != NULL");

    VkDevice device = GetData(queue).driver_device;
    const auto& dispatch = GetData(queue).driver;
    VkResult final_result = VK_SUCCESS;

    for (uint32_t sc = 0; sc < present_info->swapchainCount; sc++) {
        Swapchain& swapchain =
            *SwapchainFromHandle(present_info->pSwapchains[sc]);
        uint32_t image_idx = present_info->pImageIndices[sc];
        Swapchain::Image& img = swapchain.images[image_idx];
        VkResult swapchain_result = VK_SUCCESS;
        VkResult result;
        int err;

        int fence = -1;
        result = dispatch.QueueSignalReleaseImageANDROID(
            queue, present_info->waitSemaphoreCount,
            present_info->pWaitSemaphores, img.image, &fence);
        if (result != VK_SUCCESS) {
            ALOGE("QueueSignalReleaseImageANDROID failed: %d", result);
            swapchain_result = result;
        }

        if (swapchain.surface.swapchain_handle ==
            present_info->pSwapchains[sc]) {
            ANativeWindow* window = swapchain.surface.window.get();
            if (swapchain_result == VK_SUCCESS) {
                err = window->queueBuffer(window, img.buffer.get(), fence);
                // queueBuffer always closes fence, even on error
                if (err != 0) {
                    // TODO(jessehall): What now? We should probably cancel the
                    // buffer, I guess?
                    ALOGE("queueBuffer failed: %s (%d)", strerror(-err), err);
                    swapchain_result = WorstPresentResult(
                        swapchain_result, VK_ERROR_OUT_OF_DATE_KHR);
                }
                if (img.dequeue_fence >= 0) {
                    close(img.dequeue_fence);
                    img.dequeue_fence = -1;
                }
                img.dequeued = false;
            }
            if (swapchain_result != VK_SUCCESS) {
                ReleaseSwapchainImage(device, window, fence, img);
                OrphanSwapchain(device, &swapchain);
            }
        } else {
            ReleaseSwapchainImage(device, nullptr, fence, img);
            swapchain_result = VK_ERROR_OUT_OF_DATE_KHR;
        }

        if (present_info->pResults)
            present_info->pResults[sc] = swapchain_result;

        if (swapchain_result != final_result)
            final_result = WorstPresentResult(final_result, swapchain_result);
    }

    return final_result;
}

}  // namespace driver
}  // namespace vulkan
