target_sources(ob_bootstrap PRIVATE
    # support/Autobinder.cpp
    support/Bitfield.cpp
    support/ByteStream.cpp
    #support/CallStack.cpp
    support/Context.cpp
    support/Debug.cpp
    support/DebugLock.cpp
    support/KernelStreams.cpp
    support/Locker.cpp
    support/MemoryStore.cpp
    support/NullStreams.cpp
    support/SignalHandler.cpp
    support/Static.cpp
    support/Storage.cpp
    support/StringIO.cpp
    support/StringUtils.cpp
    support/SupportUtils.cpp
    support/Swap.cpp
    support/TextStream.cpp
    support/Thread.cpp
    support/Threads.cpp
    support/VectorIO.cpp
    support/LinuxCompat.cpp
    support/qsort_r.c
    support/SysSemaphores.cpp
    support/TSD.cpp
    support/strlcat.c
    support/strlcpy.c
    support/timers.cpp
)

target_sources(ob PRIVATE
    # support/Autobinder.cpp
    support/Bitfield.cpp
    support/ByteStream.cpp
    #support/CallStack.cpp
    support/ConditionVariable.cpp
#    support/Context.cpp
#    support/Datum.cpp
#    support/DatumLord.cpp
    support/Debug.cpp
    support/DebugLock.cpp
    support/EventFlag.cpp
#    support/GenericCache.cpp
#    support/Handler.cpp
    support/HashTable.cpp
    support/IntrusiveProfiler.cpp
    support/IOSStream.cpp
#    support/Iterator.cpp
    support/KeyID.cpp
    support/Locker.cpp
#    support/Memory.cpp
    support/MemoryStore.cpp
#    support/Message.cpp
#    support/MessageList.cpp
    support/Node.cpp
    support/NullStreams.cpp
#    support/Observer.cpp
#    support/Package.cpp
#    support/Pipe.cpp
#    support/Process.cpp
    support/ProfileIPC.cpp
    support/RegExp.cpp
#    support/Selector.cpp
    support/SignalHandler.cpp
#    support/Static.cpp
    support/Storage.cpp
    support/StopWatch.cpp
    support/StringIO.cpp
    support/StringTokenizer.cpp
    support/StringUtils.cpp
    support/SupportUtils.cpp
    support/Swap.cpp
#    support/TextStream.cpp
    support/Thread.cpp
    support/Threads.cpp
#    support/TokenSource.cpp
#    support/URL.cpp
    support/VectorIO.cpp
#    support/CatalogMirror.cpp
#    support/Catalog.cpp
    support/LinuxCompat.cpp
    support/KernelStreams.cpp
    support/qsort_r.c
    support/SysSemaphores.cpp
    support/TSD.cpp
    support/strlcat.c
    support/strlcpy.c
    support/timers.cpp
)
