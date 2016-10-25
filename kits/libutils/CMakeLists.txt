add_definitions("-DLIBUTILS_NATIVE=1" "-DOS_PATH_SEPARATOR='/'")

if (CMAKE_SYSTEM_PROCESSOR STREQUAL "mips")
    add_definitions("-DALIGN_DOUBLE")
endif()


add_library(utils SHARED
    BasicHashtable.cpp
    CallStack.cpp
    FileMap.cpp
    JenkinsHash.cpp
    LinearTransform.cpp
    Log.cpp
    NativeHandle.cpp
    Printer.cpp
    ProcessCallStack.cpp
    PropertyMap.cpp
    RefBase.cpp
    SharedBuffer.cpp
    Static.cpp
    StopWatch.cpp
    String8.cpp
    String16.cpp
    SystemClock.cpp
    Threads.cpp
    Timers.cpp
    Tokenizer.cpp
    Unicode.cpp
    VectorImpl.cpp
#    misc.cpp

#    BlobCache.cpp
    Looper.cpp
#    Trace.cpp
)
target_link_libraries(utils cutils dl log)
set_target_properties(utils PROPERTIES LINKER_LANGUAGE C)

add_subdirectory(tests)
