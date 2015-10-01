appkit_files = files(
    #'src/kits/app/AppDefs.cpp',
    #'src/kits/app/Application.cpp',
    #'src/kits/app/Looper.cpp',
    #'src/kits/app/Messenger.cpp',
    )
interfacekit_files = files(
    #'src/kits/interface/InterfaceDefs.cpp',
    )
kernelkit_files = files(
    'src/kits/kernel/thread.c',
    )

libbe_inc = include_directories(
    'headers',
    #'headers/app',
    #'headers/interface',
    'headers/kernel',
    'headers/support',
    )

libbe = shared_library('be', kernelkit_files,
    include_directories : libbe_inc)
