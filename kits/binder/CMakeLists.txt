add_library(binder SHARED
    AppOpsManager.cpp
    Binder.cpp
    BpBinder.cpp
    BufferedTextOutput.cpp
    Debug.cpp
    IAppOpsCallback.cpp
    IAppOpsService.cpp
    IBatteryStats.cpp
    IInterface.cpp
    IMemory.cpp
    IPCThreadState.cpp
    IPermissionController.cpp
    IProcessInfoService.cpp
    ProcessInfoService.cpp
    IServiceManager.cpp
    MemoryDealer.cpp
    MemoryBase.cpp
    MemoryHeapBase.cpp
    Parcel.cpp
    PermissionCache.cpp
    ProcessState.cpp
    Static.cpp
    TextOutput.cpp
)

if (NOT PLATFORM_BITS EQUAL 64)
    add_definitions(-DBINDER_IPC_32BIT=1)
endif()

#sources := \
#    AppOpsManager.cpp \
#    Binder.cpp \
#    BpBinder.cpp \
#    BufferedTextOutput.cpp \
#    Debug.cpp \
#    IAppOpsCallback.cpp \
#    IAppOpsService.cpp \
#    IBatteryStats.cpp \
#    IInterface.cpp \
#    IMemory.cpp \
#    IPCThreadState.cpp \
#    IPermissionController.cpp \
#    IProcessInfoService.cpp \
#    ProcessInfoService.cpp \
#    IServiceManager.cpp \
#    MemoryDealer.cpp \
#    MemoryBase.cpp \
#    MemoryHeapBase.cpp \
#    Parcel.cpp \
#    PermissionCache.cpp \
#    ProcessState.cpp \
#    Static.cpp \
#    TextOutput.cpp \

#LOCAL_PATH:= $(call my-dir)

#include $(CLEAR_VARS)
#LOCAL_MODULE := libbinder
#LOCAL_SHARED_LIBRARIES := liblog libcutils libutils
#LOCAL_SRC_FILES := $(sources)
#ifneq ($(TARGET_USES_64_BIT_BINDER),true)
#ifneq ($(TARGET_IS_64_BIT),true)
#LOCAL_CFLAGS += -DBINDER_IPC_32BIT=1
#endif
#endif
#LOCAL_CFLAGS += -Werror
#include $(BUILD_SHARED_LIBRARY)

#include $(CLEAR_VARS)
#LOCAL_MODULE := libbinder
#LOCAL_STATIC_LIBRARIES += libutils
#LOCAL_SRC_FILES := $(sources)
#ifneq ($(TARGET_USES_64_BIT_BINDER),true)
#ifneq ($(TARGET_IS_64_BIT),true)
#LOCAL_CFLAGS += -DBINDER_IPC_32BIT=1
#endif
#endif
#LOCAL_CFLAGS += -Werror
#include $(BUILD_STATIC_LIBRARY)
