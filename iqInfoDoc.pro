#-------------------------------------------------
#
# Project created by QtCreator 2013-12-25T08:18:04
#
#-------------------------------------------------

QT       += core gui network webkit

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets webkitwidgets

TARGET = iqInfoDoc
TEMPLATE = app

TRANSLATIONS += iqInfoDoc_ru.ts


SOURCES += main.cpp\
        iqhelpviewermainwindow.cpp \
        iqhelpviewwidget.cpp \
        iqwebhelpview.cpp \
        iqcontentwidget.cpp \
        iqscrollarea.cpp \
        iqwebview.cpp \
        iqcontentdockwidget.cpp

HEADERS  += iqhelpviewermainwindow.h \
        iqhelpviewwidget.h \
        iqwebhelpview.h \
        iqcontentwidget.h \
        iqscrollarea.h \
        iqwebview.h \
        iqcontentdockwidget.h

FORMS    += iqhelpviewermainwindow.ui \
        iqcontentdockwidget.ui

RESOURCES += \
        icons.qrc \
        fonts.qrc
