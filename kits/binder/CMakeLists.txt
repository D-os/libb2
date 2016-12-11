set(BINDER_SOURCE_DIR "${ANDROID_FRAMEWORKS_NATIVE_DIR}/libs/binder")

add_library(binder SHARED
    "${BINDER_SOURCE_DIR}/AppOpsManager.cpp"
    "${BINDER_SOURCE_DIR}/Binder.cpp"
    "${BINDER_SOURCE_DIR}/BpBinder.cpp"
    "${BINDER_SOURCE_DIR}/BufferedTextOutput.cpp"
    "${BINDER_SOURCE_DIR}/Debug.cpp"
    "${BINDER_SOURCE_DIR}/IAppOpsCallback.cpp"
    "${BINDER_SOURCE_DIR}/IAppOpsService.cpp"
    "${BINDER_SOURCE_DIR}/IBatteryStats.cpp"
    "${BINDER_SOURCE_DIR}/IInterface.cpp"
    "${BINDER_SOURCE_DIR}/IMediaResourceMonitor.cpp"
    "${BINDER_SOURCE_DIR}/IMemory.cpp"
    "${BINDER_SOURCE_DIR}/IPCThreadState.cpp"
    "${BINDER_SOURCE_DIR}/IPermissionController.cpp"
    "${BINDER_SOURCE_DIR}/IProcessInfoService.cpp"
    "${BINDER_SOURCE_DIR}/IResultReceiver.cpp"
    "${BINDER_SOURCE_DIR}/IServiceManager.cpp"
    "${BINDER_SOURCE_DIR}/MemoryBase.cpp"
    "${BINDER_SOURCE_DIR}/MemoryDealer.cpp"
    "${BINDER_SOURCE_DIR}/MemoryHeapBase.cpp"
    "${BINDER_SOURCE_DIR}/Parcel.cpp"
    "${BINDER_SOURCE_DIR}/PermissionCache.cpp"
    "${BINDER_SOURCE_DIR}/PersistableBundle.cpp"
    "${BINDER_SOURCE_DIR}/ProcessInfoService.cpp"
    "${BINDER_SOURCE_DIR}/ProcessState.cpp"
    "${BINDER_SOURCE_DIR}/Static.cpp"
    "${BINDER_SOURCE_DIR}/Status.cpp"
    "${BINDER_SOURCE_DIR}/TextOutput.cpp"
)
target_link_libraries(binder utils)

add_subdirectory(tests)
