HEADERS += $$files(headers/*.h) \
    $$files(headers/kernel/*.h) \
    $$files(headers/support/*.h) \
    $$files(headers/app/*.h) \
    $$files(headers/interface/*.h) \
    $$files(private/app/*.h) \
    $$files(private/interface/*.h) \
    $$files(private/shared/*.h) \
    $$files(private/support/*.h) \
    $$files(private/linux/*.h) \
    $$files(src/kits/kernel/*.h)
SOURCES += $$files(*.cpp) \
    $$files(src/kits/kernel/*.c) \
    $$files(src/kits/support/*.cpp) \
    $$files(src/kits/app/*.cpp) \
    $$files(src/kits/interface/*.cpp) \
    $$files(src/linux/*.c)
INCLUDEPATH += headers \
    headers/app \
    headers/interface \
    headers/kernel \
    headers/support \
    private/app \
    private/interface \
    private/shared \
    private/support \
    private/linux
OTHER_FILES += meson.build \
    $$files(docs/*.txt)
