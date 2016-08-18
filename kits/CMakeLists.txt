# Build libroot.
add_library(root SHARED
        kernel/area.c
        kernel/port.c
        kernel/sem.c
        kernel/team.c
        kernel/thread.c
        kernel/time.c
        kernel/debug.c
)
target_sources(root PRIVATE
        kernel/private.h kernel/utlist.h kernel/rwlock.h kernel/rwlock.c
)
target_link_libraries(root PUBLIC pthread)

# Build libbe.
add_library(be SHARED "")
target_sources(be PRIVATE
        ${headerFiles}
        support/Archivable.cpp
        support/ArchivingManagers.cpp
        support/ArchivingManagers.h
        support/BlockCache.cpp
#	support/ByteOrder.cpp
        support/DataIO.cpp
        support/Flattenable.cpp
        support/List.cpp
        support/Locker.cpp
        support/String.cpp
        support/StringList.cpp

        app/AppDefs.cpp
        app/AppMisc.cpp
##	app/Application.cpp
        app/DirectMessageTarget.cpp
        app/Handler.cpp
        app/Looper.cpp
        app/LooperList.cpp
        app/Message.cpp
        app/MessageAdapter.cpp
        app/MessageFilter.cpp
        app/MessageQueue.cpp
        app/MessageUtils.cpp
        app/Messenger.cpp
        app/PropertyInfo.cpp
        app/TokenSpace.cpp

##	interface/InterfaceDefs.cpp
        interface/Point.cpp
        interface/Rect.cpp
#   #	interface/Size.cpp

#	private/app/AppMisc.h
#	private/app/DirectMessageTarget.h
#	private/app/LooperList.h
#	private/app/MessageAdapter.h
#	private/app/MessagePrivate.h
#	private/app/MessageUtils.h
#	private/app/MessengerPrivate.h
#	private/app/TokenSpace.h
#	private/binary_compatibility/Global.h
#	private/binary_compatibility/Interface.h
#	private/binary_compatibility/Support.h
#	private/interface/utf8_functions.h
#	private/shared/AutoLocker.h
#	private/support/StringPrivate.h

)

# pull in private headers, so you don't have to mention it above
file(GLOB_RECURSE privateHeaders "private/*.h")
target_sources(be PRIVATE ${privateHeaders})

target_include_directories(be PRIVATE
        private
        private/app
        private/interface
        private/shared
        private/support
)

target_link_libraries(be PUBLIC root)

# Install libraries
#install(
#	TARGETS root be
#	DESTINATION ${SYSTEM_LIB_DIR}
#)
