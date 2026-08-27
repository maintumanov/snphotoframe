QT       += core gui qml quick network concurrent multimedia

CONFIG   += c++14
TARGET    = DigitalPhotoFrame
TEMPLATE  = app

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD
MOC_DIR = .

RESOURCES += qml.qrc

linux: QMAKE_LFLAGS += -Wl,--no-as-needed

SOURCES += \
    main.cpp \
    config.cpp \
    playlistmanager.cpp \
    photoframe.cpp \
    signalnet.cpp \
    qsnshapes.cpp \
    qsntcpclient.cpp \
    qsnudpclient.cpp \
    qsninterface.cpp

HEADERS += \
    config.h \
    playlistmanager.h \
    photoframe.h \
    signalnet.h \
    qsnshapes.h \
    qsntcpclient.h \
    qsnudpclient.h \
    qsninterface.h

DISTFILES += \
    main.qml
