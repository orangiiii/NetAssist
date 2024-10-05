HEADERS += \
    DrawWidget.h \
    clientwin.h \
    define.h \
    fileio.h \
    mainwindow.h \
    network.h

SOURCES += \
    DrawWidget.cpp \
    clientwin.cpp \
    fileio.cpp \
    main.cpp \
    mainwindow.cpp \
    network.cpp

QT += core gui widgets network

RESOURCES += \
    resources.qrc \

DISTFILES += \
    .gitattributes \
    .gitignore \
    readme.md

