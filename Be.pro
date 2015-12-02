HEADERS += $$files(headers/*.h) \
    $$files(headers/kernel/*.h) \
    $$files(headers/support/*.h) \
    $$files(headers/app/*.h) \
    $$files(headers/interface/*.h) \
    $$files(private/shared/*.h) \
    $$files(private/linux/*.h)
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
    private/shared \
    private/linux
OTHER_FILES += meson.build \
    $$files(docs/*.txt)
