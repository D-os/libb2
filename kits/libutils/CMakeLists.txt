set(UTILS_SOURCE_DIR "${ANDROID_SYSTEM_CORE_DIR}/libutils")
include_directories(
    "${ANDROID_LIBNATIVEHELPER_DIR}/include/nativehelper"
    "${EXTERNAL_SAFE_IOP_DIR}/include"
)

add_definitions("-DLIBUTILS_NATIVE=1" "-DOS_PATH_SEPARATOR='/'")

if (CMAKE_SYSTEM_PROCESSOR STREQUAL "mips")
    add_definitions("-DALIGN_DOUBLE")
endif()


add_library(utils SHARED
    "${UTILS_SOURCE_DIR}/CallStack.cpp"
    "${UTILS_SOURCE_DIR}/FileMap.cpp"
    "${UTILS_SOURCE_DIR}/JenkinsHash.cpp"
    "${UTILS_SOURCE_DIR}/LinearTransform.cpp"
    "${UTILS_SOURCE_DIR}/Log.cpp"
    "${UTILS_SOURCE_DIR}/NativeHandle.cpp"
    "${UTILS_SOURCE_DIR}/Printer.cpp"
    "${UTILS_SOURCE_DIR}/ProcessCallStack.cpp"
    "${UTILS_SOURCE_DIR}/PropertyMap.cpp"
    "${UTILS_SOURCE_DIR}/RefBase.cpp"
    "${UTILS_SOURCE_DIR}/SharedBuffer.cpp"
    "${UTILS_SOURCE_DIR}/Static.cpp"
    "${UTILS_SOURCE_DIR}/StopWatch.cpp"
    "${UTILS_SOURCE_DIR}/String.cpp"
    "${UTILS_SOURCE_DIR}/SystemClock.cpp"
    "${UTILS_SOURCE_DIR}/Threads.cpp"
    "${UTILS_SOURCE_DIR}/Timers.cpp"
    "${UTILS_SOURCE_DIR}/Tokenizer.cpp"
    "${UTILS_SOURCE_DIR}/Unicode.cpp"
    "${UTILS_SOURCE_DIR}/VectorImpl.cpp"
#    "${UTILS_SOURCE_DIR}/misc.cpp"

#    "${UTILS_SOURCE_DIR}/BlobCache.cpp"
    "${UTILS_SOURCE_DIR}/Looper.cpp"
#    "${UTILS_SOURCE_DIR}/Trace.cpp"
)
target_link_libraries(utils cutils dl log)
set_target_properties(utils PROPERTIES LINKER_LANGUAGE C)

add_subdirectory(tests)
