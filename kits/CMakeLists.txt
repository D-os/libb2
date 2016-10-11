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
        kernel/syscalls.c
        kernel/attr.c
        kernel/info.c
        kernel/monitor.c
        private/strlcat.c
        private/strlcpy.c
        private/find_directory.c
)
target_sources(root PRIVATE
        root.map kernel/private.h kernel/utlist.h kernel/rwlock.h kernel/rwlock.c
)
target_include_directories(root PRIVATE private)
target_link_libraries(root PUBLIC pthread dl)
set_target_properties(root PROPERTIES VERSION ${LIBBE_VERSION}
                                      SOVERSION ${LIBBE_VERSION_MAJOR} )
set_target_properties(root PROPERTIES LINK_FLAGS "-Wl,--version-script=${CMAKE_CURRENT_SOURCE_DIR}/root.map")

# Build libbe.
add_library(be SHARED "")
target_sources(be PRIVATE
        ${headerFiles}
        support/Archivable.cpp
        support/ArchivingManagers.cpp
        support/ArchivingManagers.h
        support/BlockCache.cpp
        support/ByteOrder.cpp
        support/DataIO.cpp
        support/Flattenable.cpp
        support/List.cpp
        support/Locker.cpp
        support/PointerList.cpp
        support/String.cpp
        support/StringList.cpp

        storage/AppFileInfo.cpp
        storage/Directory.cpp
        storage/Entry.cpp
        storage/EntryList.cpp
        storage/File.cpp
        storage/FindDirectory.cpp
        storage/Mime.cpp
        storage/MimeType.cpp
        storage/Node.cpp
        storage/NodeInfo.cpp
        storage/NodeMonitor.cpp
        storage/OffsetFile.cpp
        storage/Path.cpp
        storage/ResourceFile.cpp
        storage/ResourceItem.cpp
        storage/Resources.cpp
        storage/ResourcesContainer.cpp
        storage/Statable.cpp
        storage/SymLink.cpp
        storage/Volume.cpp
        storage/VolumeRoster.cpp
        storage/storage_support.cpp
        storage/mime/DatabaseLocation.cpp
        storage/mime/database_support.cpp

        app/AppDefs.cpp
        app/AppMisc.cpp
        app/AppServerLink.cpp
        app/Application.cpp
        app/Cursor.cpp
        app/DesktopLink.cpp
        app/DirectMessageTarget.cpp
        app/Handler.cpp
        app/InitTerminateLibBe.cpp
        app/LinkReceiver.cpp
        app/LinkSender.cpp
        app/Looper.cpp
        app/LooperList.cpp
        app/Message.cpp
        app/MessageAdapter.cpp
        app/MessageFilter.cpp
        app/MessageQueue.cpp
        app/MessageRunner.cpp
        app/MessageUtils.cpp
        app/Messenger.cpp
        app/PortLink.cpp
        app/PropertyInfo.cpp
        app/Roster.cpp
        app/RosterPrivate.cpp
        app/ServerLink.cpp
        app/ServerMemoryAllocator.cpp
        app/TokenSpace.cpp

        interface/InterfaceDefs.cpp
        interface/Bitmap.cpp
        interface/ColorConversion.cpp
        interface/GraphicsDefs.cpp
        interface/Picture.cpp
        interface/Point.cpp
        interface/Rect.cpp
        interface/Size.cpp

        # HVIF: Haiku Vector Icon Format
        private/icon/IconUtils.cpp
        private/icon/Icon.cpp
#        private/icon/IconRenderer.cpp
#        private/icon/flat_icon/FlatIconFormat.cpp
#        private/icon/flat_icon/FlatIconImporter.cpp
#        private/icon/flat_icon/LittleEndianBuffer.cpp
#        private/icon/flat_icon/PathCommandQueue.cpp
#        private/icon/message/Defines.cpp
#        private/icon/message/MessageImporter.cpp
        private/icon/shape/PathContainer.cpp
#        private/icon/shape/Shape.cpp
        private/icon/shape/ShapeContainer.cpp
#        private/icon/shape/VectorPath.cpp
#        private/icon/style/GradientTransformable.cpp
#        private/icon/style/Style.cpp
        private/icon/style/StyleContainer.cpp
#        private/icon/transformable/Transformable.cpp
#        private/icon/transformer/AffineTransformer.cpp
#        private/icon/transformer/ContourTransformer.cpp
#        private/icon/transformer/PathSource.cpp
#        private/icon/transformer/PerspectiveTransformer.cpp
#        private/icon/transformer/StrokeTransformer.cpp
#        private/icon/transformer/Transformer.cpp
#        private/icon/transformer/TransformerFactory.cpp

)

# pull in private headers, so you don't have to mention it above
file(GLOB_RECURSE privateHeaders "private/*.h")
target_sources(be PRIVATE be.map ${privateHeaders})

target_include_directories(be PRIVATE
        private
        private/app
        private/input
        private/interface
        private/shared
        private/storage
        private/support
        private/icon
        private/icon/flat_icon
        private/icon/message
        private/icon/shape
        private/icon/style
        private/icon/transformable
        private/icon/transformer
)

target_link_libraries(be PUBLIC root)
set_target_properties(be PROPERTIES VERSION ${LIBBE_VERSION}
                                    SOVERSION ${LIBBE_VERSION_MAJOR} )
set_target_properties(be PROPERTIES LINK_FLAGS "-Wl,--version-script=${CMAKE_CURRENT_SOURCE_DIR}/be.map")

# Install libraries
#install(
#	TARGETS root be
#	DESTINATION ${SYSTEM_LIB_DIR}
#)
