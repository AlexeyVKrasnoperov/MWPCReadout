#-------------------------------------------------
#
# Project created by QtCreator 2017-02-19T15:07:26
#
#-------------------------------------------------

QT += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MWPCReadout
TEMPLATE = app

#DEFINES += PCAPINTERFACE

SOURCES += main.cpp\
        mainwindow.cpp \
    sensor.cpp \
    sensormap.cpp \
    qcompressor.cpp \
    settingsdialog.cpp \
    programsettings.cpp \
    rateplot.cpp \
    dataanalizer.cpp \
    mwpcdatainterface.cpp \
    udpsocketinterface.cpp \
    messagesender.cpp

HEADERS  += mainwindow.h \
    sensor.h \
    sensormap.h \
    qcompressor.h \
    settingsdialog.h \
    programsettings.h \
    rateplot.h \
    dataanalizer.h \
    mwpcdatainterface.h \
    udpsocketinterface.h \
    messagesender.h

FORMS    += mainwindow.ui \
    settingsdialog.ui


INLCUDEPATH += /usr/include
INCLUDEPATH += /usr/include/qwt/

#
contains( DEFINES, PCAPINTERFACE ) {
    LIBS += `pcap-config --libs`
    SOURCES += pcapinterface.cpp
    HEADERS += pcapinterface.h
    QMAKE_POST_LINK +=  "sudo /sbin/setcap  cap_net_raw,cap_net_admin=eip $${OUT_PWD}/$${TARGET}"
}
#
LIBS += -lz
LIBS += -lqwt-qt5

#system (echo $${OUT_PWD}/$${TARGET})


RESOURCES += \
    mwpc.qrc
