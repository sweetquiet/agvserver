QT += core sql network
QT -= gui

CONFIG += c++11

TARGET = AgvServer
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    util/common.cpp \
    util/global.cpp \
    sql/sql.cpp \
    sql/sqlserver.cpp \
    network/agvnetwork.cpp \
    network/IOCPModel.cpp \
    log/log.cpp \
    business/agvstation.cpp \
    business/agvline.cpp \
    business/agv.cpp \
    business/agvtask.cpp \
    business/agvcenter.cpp \
    business/mapcenter.cpp \
    business/taskcenter.cpp \
    business/msgcenter.cpp \
    business/usermsgprocessor.cpp \
    business/agvpositionpublisher.cpp \
    business/agvstatuspublisher.cpp \
    business/logpublisher.cpp \
    business/agvlogpublisher.cpp \
    log/agvlog.cpp \
    log/agvlogprocess.cpp

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

HEADERS += \
    util/common.h \
    util/concurrentqueue.h \
    util/global.h \
    sql/sql.h \
    sql/sqlserver.h \
    network/agvnetwork.h \
    network/IOCPModel.h \
    log/log.h \
    business/agvstation.h \
    business/agvline.h \
    business/agv.h \
    business/agvtask.h \
    business/agvcenter.h \
    business/mapcenter.h \
    business/taskcenter.h \
    business/msgcenter.h \
    business/usermsgprocessor.h \
    business/agvpositionpublisher.h \
    business/agvstatuspublisher.h \
    business/logpublisher.h \
    business/agvlogpublisher.h \
    log/agvlog.h \
    log/agvlogprocess.h

#pluginXml
INCLUDEPATH += D:\thirdparty\pugixml\include
CONFIG(debug, debug|release) {
    LIBS += D:\thirdparty\pugixml\staticlib\Debug\pugixml.lib
} else {
    LIBS += D:\thirdparty\pugixml\staticlib\Release\pugixml.lib
}

