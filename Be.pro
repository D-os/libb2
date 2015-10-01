HEADERS += $$files(libbe/headers/*.h) \
    $$files(libbe/headers/app/*.h) \
    $$files(libbe/headers/interface/*.h) \
    $$files(libbe/headers/kernel/*.h) \
    $$files(libbe/headers/support/*.h)
SOURCES += $$files(*.cpp) \
    $$files(libbe/src/kits/app/*.cpp) \
    $$files(libbe/src/kits/interface/*.cpp) \
    $$files(libbe/src/kits/kernel/*.c)
INCLUDEPATH += libbe/headers \
    libbe/headers/app \
    libbe/headers/interface \
    libbe/headers/kernel \
    libbe/headers/support
OTHER_FILES += meson.build \
    libbe/meson.build \
    libbe/src/kits/kernel/meson.build
