cmake_minimum_required(VERSION 3.1)
project(libbe CXX C)
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_FLAGS "-Wall -Wno-multichar")

if(NOT CMAKE_GENERATOR STREQUAL "Ninja")
    message(FATAL_ERROR "Only Ninja generator is supported: cmake ... -GNinja")
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fcolor-diagnostics")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcolor-diagnostics")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fdiagnostics-color")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdiagnostics-color")
endif()

add_library(root SHARED
    src/kits/kernel/sem.c
    src/kits/kernel/thread.c
    src/kits/kernel/time.c
    src/kits/kernel/debug.c
)
target_sources(root PRIVATE
    headers/KernelKit.h headers/kernel/OS.h headers/kernel/image.h
    src/kits/kernel/private.h src/linux/bus.c private/linux/kdbus.h private/linux/kdbus-api.h
)
target_include_directories(root PUBLIC headers headers/support headers/kernel PRIVATE private/linux)
target_link_libraries(root PUBLIC pthread)

add_library(be SHARED "")
target_sources(be PRIVATE
    headers/BeBuild.h

    headers/SupportKit.h
    headers/support/Archivable.h
    headers/support/Autolock.h
    headers/support/ByteOrder.h
    headers/support/DataIO.h
    headers/support/Debug.h
    headers/support/Errors.h
    headers/support/Flattenable.h
    headers/support/List.h
    headers/support/Locker.h
    headers/support/StackOrHeapArray.h
    headers/support/String.h
    headers/support/StringList.h
    headers/support/SupportDefs.h
    headers/support/TypeConstants.h
    src/kits/support/Archivable.cpp
    src/kits/support/ArchivingManagers.cpp
    src/kits/support/ArchivingManagers.h
    src/kits/support/ByteOrder.cpp
    src/kits/support/DataIO.cpp
    src/kits/support/Flattenable.cpp
    src/kits/support/List.cpp
    src/kits/support/Locker.cpp
    src/kits/support/String.cpp
    src/kits/support/StringList.cpp

    headers/AppKit.h
    headers/app/AppDefs.h
    headers/app/Handler.h
    headers/app/Looper.h
    headers/app/MessageFilter.h
    headers/app/Message.h
    headers/app/MessageQueue.h
    headers/app/Messenger.h
    src/kits/app/AppDefs.cpp
    src/kits/app/AppMisc.cpp
#    src/kits/app/Application.cpp
    src/kits/app/DirectMessageTarget.cpp
    src/kits/app/Handler.cpp
    src/kits/app/Looper.cpp
    src/kits/app/LooperList.cpp
    src/kits/app/Message.cpp
    src/kits/app/MessageAdapter.cpp
    src/kits/app/MessageFilter.cpp
    src/kits/app/MessageQueue.cpp
    src/kits/app/MessageUtils.cpp
    src/kits/app/Messenger.cpp
    src/kits/app/TokenSpace.cpp

    headers/interface/Point.h
    headers/interface/Rect.h
    headers/interface/Size.h
#    src/kits/interface/InterfaceDefs.cpp
    src/kits/interface/Point.cpp
    src/kits/interface/Rect.cpp
#    src/kits/interface/Size.cpp

    private/app/AppMisc.h
    private/app/DirectMessageTarget.h
    private/app/LooperList.h
    private/app/MessageAdapter.h
    private/app/MessagePrivate.h
    private/app/MessageUtils.h
    private/app/MessengerPrivate.h
    private/app/TokenSpace.h
    private/binary_compatibility/Global.h
    private/binary_compatibility/Interface.h
    private/binary_compatibility/Support.h
    private/interface/utf8_functions.h
    private/shared/AutoLocker.h
    private/support/StringPrivate.h

)
target_include_directories(be
PUBLIC
    headers
    headers/app
    headers/interface
    headers/kernel
    headers/support
PRIVATE
    private
    private/app
    private/interface
    private/shared
    private/support
)
target_link_libraries(be PUBLIC root)

add_executable(thread thread.cpp)
target_link_libraries(thread root)

add_executable(sem sem.cpp)
target_link_libraries(sem root)

add_executable(app app.cpp)
target_link_libraries(app be)

#add_executable(hello hello.cpp)
#target_link_libraries(hello be)
