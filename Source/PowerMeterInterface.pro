QT       += core gui sql network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
CONFIG += qt plugin thread

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
    DatabaseLayer/AsyncQuery.cpp \
    DatabaseLayer/AsyncQueryResult.cpp \
    DatabaseLayer/AsynqQueryModel.cpp \
    DatabaseLayer/ConnectionManager.cpp \
    DeviceLayer/DeviceInterface.cpp \
    DeviceLayer/DevicesCommunicationService.cpp \
    NetworkLayer/TcpClient.cpp \
    Tools.cpp \
    main.cpp \
    MainWindow.cpp

HEADERS += \
    DatabaseLayer/AsyncQuery.h \
    DatabaseLayer/AsyncQueryResult.h \
    DatabaseLayer/AsynqQueryModel.h \
    DatabaseLayer/ConnectionManager.h \
    DeviceLayer/DeviceInterface.h \
    DeviceLayer/DevicesCommunicationService.h \
    MainWindow.h \
    NetworkLayer/TcpClient.h \
    Tools.h

FORMS += \
    MainWindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

#unix|win32: LIBS += -L$$PWD/BSPLib/ -lBSPLib1

#INCLUDEPATH += $$PWD/BSPLib/Headers
#DEPENDPATH += $$PWD/BSPLib/Headers


#unix|win32: LIBS += -L$$PWD/BSPLib/ -lBSPLib1

#INCLUDEPATH += $$PWD/BSPLib/Headers
#DEPENDPATH += $$PWD/BSPLib/Headers

#win32:!win32-g++: PRE_TARGETDEPS += $$PWD/BSPLib/BSPLib1.lib
#else:unix|win32-g++: PRE_TARGETDEPS += $$PWD/BSPLib/libBSPLib1.a
