project('libbe', ['cpp', 'c'])
add_global_arguments('-std=c99', '-Wno-multichar', language : 'c')
add_global_arguments('-std=c++98', '-Wno-multichar', language : 'cpp')

supportkit_files = files(
    'src/kits/support/Archivable.cpp',
    'src/kits/support/ArchivingManagers.cpp',
    'src/kits/support/ByteOrder.cpp',
    'src/kits/support/DataIO.cpp',
    'src/kits/support/Flattenable.cpp',
    'src/kits/support/List.cpp',
    'src/kits/support/Locker.cpp',
    'src/kits/support/String.cpp',
    'src/kits/support/StringList.cpp',
    )
appkit_files = files(
    'src/kits/app/AppDefs.cpp',
    'src/kits/app/AppMisc.cpp',
    #'src/kits/app/Application.cpp',
    'src/kits/app/DirectMessageTarget.cpp',
    'src/kits/app/Handler.cpp',
    'src/kits/app/Looper.cpp',
    'src/kits/app/LooperList.cpp',
    'src/kits/app/Message.cpp',
    'src/kits/app/MessageAdapter.cpp',
    'src/kits/app/MessageFilter.cpp',
    'src/kits/app/MessageQueue.cpp',
    'src/kits/app/MessageUtils.cpp',
    'src/kits/app/Messenger.cpp',
    'src/kits/app/TokenSpace.cpp',
    )
interfacekit_files = files(
    #'src/kits/interface/InterfaceDefs.cpp',
    'src/kits/interface/Point.cpp',
    'src/kits/interface/Rect.cpp',
    #'src/kits/interface/Size.cpp',
    )
kernelkit_files = files(
    'src/kits/kernel/sem.c',
    'src/kits/kernel/thread.c',
    'src/kits/kernel/time.c',
    'src/kits/kernel/debug.c',
    )

be_inc = include_directories(
    'headers',
    'headers/app',
    'headers/interface',
    'headers/kernel',
    'headers/support',
    )
priv_inc = include_directories(
    'private',
    'private/app',
    'private/interface',
    'private/shared',
    'private/support',
    )

libroot = shared_library('root', kernelkit_files, 'src/linux/bus.c',
    include_directories : [be_inc, include_directories('private/linux')],
    link_args: '-pthread')

libbe = shared_library('be', supportkit_files, appkit_files, interfacekit_files,
    include_directories : [be_inc, priv_inc],
    link_with : libroot)

#executable('hello', 'hello.cpp',
#    include_directories : be_inc,
#    link_with : libbe)

executable('thread', 'thread.cpp',
    include_directories : be_inc,
    link_with : libroot)

executable('sem', 'sem.cpp',
    include_directories : be_inc,
    link_with : libroot)

executable('app', 'app.cpp',
    include_directories : be_inc,
    link_with : libbe)
