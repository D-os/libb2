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
LOCAL_CFLAGS := -std=c99 -fvisibility=hidden -fstrict-aliasing
LOCAL_CFLAGS += -DLOG_TAG=\"vkinfo\"
LOCAL_CFLAGS += -Weverything -Werror -Wno-padded -Wno-undef -Wno-switch-enum
LOCAL_CPPFLAGS := -std=c++1y \
	-Wno-c++98-compat-pedantic \
	-Wno-c99-extensions \
	-Wno-old-style-cast

LOCAL_C_INCLUDES := \
	frameworks/native/vulkan/include

LOCAL_SRC_FILES := vkinfo.cpp
LOCAL_ADDITIONAL_DEPENDENCIES := $(LOCAL_PATH)/Android.mk

LOCAL_SHARED_LIBRARIES := libvulkan liblog

LOCAL_MODULE := vkinfo
LOCAL_MODULE_TAGS := optional

include $(BUILD_EXECUTABLE)
