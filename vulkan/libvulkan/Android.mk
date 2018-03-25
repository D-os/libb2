# Copyright 2015 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_CLANG := true
LOCAL_SANITIZE := integer

LOCAL_CFLAGS := -DLOG_TAG=\"vulkan\" \
	-DVK_USE_PLATFORM_ANDROID_KHR \
	-DVK_NO_PROTOTYPES \
	-std=c99 -fvisibility=hidden -fstrict-aliasing \
	-Weverything -Werror \
	-Wno-padded \
	-Wno-switch-enum \
	-Wno-undef
#LOCAL_CFLAGS += -DLOG_NDEBUG=0
LOCAL_CPPFLAGS := -std=c++14 \
	-Wno-c99-extensions \
	-Wno-c++98-compat-pedantic \
	-Wno-exit-time-destructors \
	-Wno-global-constructors \
	-Wno-zero-length-array

LOCAL_C_INCLUDES := \
	frameworks/native/vulkan/include \
	system/core/libsync/include

LOCAL_SRC_FILES := \
	api.cpp \
	api_gen.cpp \
	debug_report.cpp \
	driver.cpp \
	driver_gen.cpp \
	layers_extensions.cpp \
	stubhal.cpp \
	swapchain.cpp \
	vulkan_loader_data.cpp
LOCAL_ADDITIONAL_DEPENDENCIES := $(LOCAL_PATH)/Android.mk

LOCAL_STATIC_LIBRARIES := libziparchive
LOCAL_SHARED_LIBRARIES := libhardware libsync libbase liblog libutils libcutils libz

LOCAL_MODULE := libvulkan
include $(BUILD_SHARED_LIBRARY)
