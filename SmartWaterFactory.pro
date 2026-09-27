QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

TARGET = SmartWaterFactory
TEMPLATE = app

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    stockpage.cpp \
    placeholderpage.cpp

HEADERS += \
    mainwindow.h \
    stockpage.h \
    placeholderpage.h

RESOURCES += \
    resources.qrc
