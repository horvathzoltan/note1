QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    filenamehelper.cpp \
    main.cpp \
    mainwindow.cpp \
    newfiledialog.cpp \
    settings.cpp

HEADERS += \
    filenamehelper.h \
    mainwindow.h \
    newfiledialog.h \
    settings.h

FORMS += \
    mainwindow.ui \
    newfiledialog.ui

TRANSLATIONS += \
    note1_hu_HU.ts

unix:HOME = $$system(echo $HOME)
win32:HOME = $$system(echo %userprofile%)

CONFIG(debug, debug|release){
BUILD = debug
} else {
BUILD = release
}

COMMON = common
COMMON_LIBS = $$COMMON"lib"

equals(BUILD,debug) {
    #message( "build is _ debug" )
    COMMON_LIBS = $$COMMON_LIBS"_debug"
}

!contains(QMAKE_TARGET.arch, x86_64) {
    !contains(QT_ARCH, x86_64) {
        COMMON_LIBS = $$COMMON_LIBS"_32"
    }
}

COMMON_LIBS_FULLPATH = $$shell_path($$HOME/$$COMMON_LIBS)
COMMON_INCLUDE_FULLPATH = $$shell_path($$HOME/$$COMMON)

message("COMMON_LIBS_FULLPATH: " $$COMMON_LIBS_FULLPATH);
message("COMMON_INCLUDE_FULLPATH: " $$COMMON_INCLUDE_FULLPATH);

unix:!macx:
{
LIBS += -L$$COMMON_LIBS_FULLPATH/ -lstringhelper
LIBS += -L$$COMMON_LIBS_FULLPATH/ -lfilehelper
LIBS += -L$$COMMON_LIBS_FULLPATH/ -lshortguid
LIBS += -L$$COMMON_LIBS_FULLPATH/ -llogger
LIBS += -L$$COMMON_LIBS_FULLPATH/ -lmacrofactory
LIBS += -L$$COMMON_LIBS_FULLPATH/ -linihelper
LIBS += -L$$COMMON_LIBS_FULLPATH/ -lsettingshelper
}

INCLUDEPATH += $$COMMON_INCLUDE_FULLPATH
DEPENDPATH += $$COMMON_INCLUDE_FULLPATH

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    todo.txt

RESOURCES += \
    note1.qrc
