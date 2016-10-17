set(BINDER_SOURCE_DIR "${ANDROID_FRAMEWORKS_NATIVE_DIR}/libs/binder")

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_definitions(-fno-rtti -fno-exceptions)
set(CMAKE_SHARED_LINKER_FLAGS "-nodefaultlibs")
if (CMAKE_CXX_COMPILER_ID MATCHES GNU)
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -lgcc")
endif()


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
    "${BINDER_SOURCE_DIR}/IMemory.cpp"
    "${BINDER_SOURCE_DIR}/IPCThreadState.cpp"
    "${BINDER_SOURCE_DIR}/IPermissionController.cpp"
    "${BINDER_SOURCE_DIR}/IProcessInfoService.cpp"
    "${BINDER_SOURCE_DIR}/ProcessInfoService.cpp"
    "${BINDER_SOURCE_DIR}/IServiceManager.cpp"
    "${BINDER_SOURCE_DIR}/MemoryDealer.cpp"
    "${BINDER_SOURCE_DIR}/MemoryBase.cpp"
    "${BINDER_SOURCE_DIR}/MemoryHeapBase.cpp"
    "${BINDER_SOURCE_DIR}/Parcel.cpp"
    "${BINDER_SOURCE_DIR}/PermissionCache.cpp"
    "${BINDER_SOURCE_DIR}/ProcessState.cpp"
    "${BINDER_SOURCE_DIR}/Static.cpp"
    "${BINDER_SOURCE_DIR}/TextOutput.cpp"
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
