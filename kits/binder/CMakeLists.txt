set(BINDER_SOURCE "${ANDROID_FRAMEWORKS_NATIVE_DIR}/libs/binder")

add_library(binder SHARED
    "${BINDER_SOURCE}/AppOpsManager.cpp"
    "${BINDER_SOURCE}/Binder.cpp"
    "${BINDER_SOURCE}/BpBinder.cpp"
    "${BINDER_SOURCE}/BufferedTextOutput.cpp"
    "${BINDER_SOURCE}/Debug.cpp"
    "${BINDER_SOURCE}/IAppOpsCallback.cpp"
    "${BINDER_SOURCE}/IAppOpsService.cpp"
    "${BINDER_SOURCE}/IBatteryStats.cpp"
    "${BINDER_SOURCE}/IInterface.cpp"
    "${BINDER_SOURCE}/IMemory.cpp"
    "${BINDER_SOURCE}/IPCThreadState.cpp"
    "${BINDER_SOURCE}/IPermissionController.cpp"
    "${BINDER_SOURCE}/IProcessInfoService.cpp"
    "${BINDER_SOURCE}/ProcessInfoService.cpp"
    "${BINDER_SOURCE}/IServiceManager.cpp"
    "${BINDER_SOURCE}/MemoryDealer.cpp"
    "${BINDER_SOURCE}/MemoryBase.cpp"
    "${BINDER_SOURCE}/MemoryHeapBase.cpp"
    "${BINDER_SOURCE}/Parcel.cpp"
    "${BINDER_SOURCE}/PermissionCache.cpp"
    "${BINDER_SOURCE}/ProcessState.cpp"
    "${BINDER_SOURCE}/Static.cpp"
    "${BINDER_SOURCE}/TextOutput.cpp"
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

add_subdirectory(tests)
