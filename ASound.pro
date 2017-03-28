#-------------------------------------------------
#
# Project created by QtCreator 2017-03-24T08:28:36
#
#-------------------------------------------------

QT       -= gui

CONFIG(debug, debug|release){
    TARGET = ASoundd
    DESTDIR = ../../bin
} else {
    TARGET = ASound
    DESTDIR = ../../bin
}

TEMPLATE = lib

DEFINES += ASOUND_LIBRARY

INCLUDEPATH += include/
VPATH += src/

SOURCES += $$files(src/*.cpp)
HEADERS += $$files(include/*.h)
win32{
    LIBS += -L'C:/Program Files (x86)/OpenAL 1.1 SDK/libs/Win64/' -lOpenAL32
    INCLUDEPATH += 'C:/Program Files (x86)/OpenAL 1.1 SDK/include'
}

unix{
    #LIBS += -lOpenAL32;
}

unix {
    target.path = /usr/lib
    INSTALLS += target
}
