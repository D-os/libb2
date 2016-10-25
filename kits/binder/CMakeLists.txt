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
target_link_libraries(binder utils)

add_subdirectory(tests)
