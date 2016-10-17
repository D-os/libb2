include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_definitions(-fno-rtti -fno-exceptions)
set(CMAKE_SHARED_LINKER_FLAGS "-nodefaultlibs")
if (CMAKE_CXX_COMPILER_ID MATCHES GNU)
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -lgcc")
endif()

add_library(binder SHARED
    binder/AppOpsManager.cpp
    binder/Binder.cpp
    binder/BpBinder.cpp
    binder/BufferedTextOutput.cpp
    binder/Debug.cpp
    binder/IAppOpsCallback.cpp
    binder/IAppOpsService.cpp
    binder/IBatteryStats.cpp
    binder/IInterface.cpp
    binder/IMemory.cpp
    binder/IPCThreadState.cpp
    binder/IPermissionController.cpp
    binder/IProcessInfoService.cpp
    binder/ProcessInfoService.cpp
    binder/IServiceManager.cpp
    binder/MemoryDealer.cpp
    binder/MemoryBase.cpp
    binder/MemoryHeapBase.cpp
    binder/Parcel.cpp
    binder/PermissionCache.cpp
    binder/ProcessState.cpp
    binder/Static.cpp
    binder/TextOutput.cpp
)
if (NOT PLATFORM_BITS EQUAL 64)
    set_target_properties(binder PROPERTIES COMPILE_FLAGS "-DBINDER_IPC_32BIT=1")
endif()

add_subdirectory(binder/tests)

add_library(be SHARED "")
target_sources(be PRIVATE
        ${headerFiles}
)
set_target_properties(be PROPERTIES LINKER_LANGUAGE CXX)
