TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        main.cpp

SUBDIRS += \
    fpgrow.pro

HEADERS += \
    fptree_headeronly.h
