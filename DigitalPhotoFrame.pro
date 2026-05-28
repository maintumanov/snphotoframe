QT       += core gui widgets network concurrent
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG   += c++14
TARGET    = DigitalPhotoFrame
TEMPLATE  = app

# Оптимизация для Raspberry Pi (если используется)
linux: QMAKE_LFLAGS += -Wl,--no-as-needed

# Пути к заголовочным файлам и исходникам
INCLUDEPATH += $$PWD/include
DEPENDPATH  += $$PWD/src

SOURCES += \
    main.cpp \
    src/config.cpp \
    src/playlistmanager.cpp \
    src/imagedisplay.cpp \
    src/photoframe.cpp

HEADERS += \
    include/config.h \
    include/playlistmanager.h \
    include/imagedisplay.h \
    include/photoframe.h
