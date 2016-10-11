# Build libroot.
add_library(root SHARED
		kernel/area.c
		kernel/image.c
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
target_link_libraries(root PUBLIC pthread dl)

# Build libbe.
add_library(be SHARED "")
target_sources(be PRIVATE
		${headerFiles}
		support/List.cpp
		support/String.cpp
		support/StringList.cpp

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
