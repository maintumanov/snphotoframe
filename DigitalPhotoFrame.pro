QT       += core gui widgets network concurrent
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG   += c++14
TARGET    = DigitalPhotoFrame
TEMPLATE  = app

# Явно указываем текущую директорию для поиска заголовков
INCLUDEPATH += .

# Оптимизация для Raspberry Pi (если используется)
linux: QMAKE_LFLAGS += -Wl,--no-as-needed

SOURCES += \
    main.cpp \
    config.cpp \
    playlistmanager.cpp \
    imagedisplay.cpp \
    photoframe.cpp

HEADERS += \
    config.h \
    playlistmanager.h \
    imagedisplay.h \
    photoframe.h
