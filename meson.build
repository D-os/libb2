project('libbe', ['cpp', 'c'])
add_global_arguments('-std=c99', '-Wno-multichar', language : 'c')
add_global_arguments('-std=c++98', '-Wno-multichar', language : 'cpp')

supportkit_files = files(
    #'src/kits/support/Archivable.cpp',
    #'src/kits/support/ByteOrder.cpp',
    #'src/kits/support/DataIO.cpp',
    #'src/kits/support/Flattenable.cpp',
    #'src/kits/support/List.cpp',
    )
appkit_files = files(
    #'src/kits/app/AppDefs.cpp',
    #'src/kits/app/Application.cpp',
    #'src/kits/app/Handler.cpp',
    #'src/kits/app/Looper.cpp',
    #'src/kits/app/Message.cpp',
    #'src/kits/app/Messenger.cpp',
    )
interfacekit_files = files(
    #'src/kits/interface/InterfaceDefs.cpp',
    #'src/kits/interface/Point.cpp',
    #'src/kits/interface/Rect.cpp',
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

libroot = shared_library('root', kernelkit_files, 'src/linux/bus.c',
    include_directories : [be_inc, include_directories('private/linux')],
    link_args: '-pthread')

#libbe = shared_library('be', supportkit_files, appkit_files,
#    include_directories : [be_inc, include_directories('private/shared')],
#    link_with : libroot)

#executable('hello', 'hello.cpp',
#    include_directories : be_inc,
#    link_with : libbe)

executable('thread', 'thread.cpp',
    include_directories : be_inc,
    link_with : libroot)

executable('sem', 'sem.cpp',
    include_directories : be_inc,
    link_with : libroot)

#executable('app', 'app.cpp',
#    include_directories : be_inc,
#    link_with : libbe)
