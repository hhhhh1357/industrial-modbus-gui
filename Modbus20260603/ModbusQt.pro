QT       += core gui serialport sql

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

# 交叉编译场景：按目标架构判断（QT_ARCH=arm）
equals(QT_ARCH, arm) {
    # i.MX6 ARM目标架构
    DEFINES += IMX6_PLATFORM
    QMAKE_LFLAGS += --sysroot=/home/wp20/T113/SDK-yuanma/ATK-DLT113IS/out/t113_i/atk_dlt113is/buildroot/buildroot/host/arm-buildroot-linux-gnueabi/sysroot

    INCLUDEPATH += /home/wp20/T113/SDK-yuanma/ATK-DLT113IS/out/t113_i/atk_dlt113is/buildroot/buildroot/host/arm-buildroot-linux-gnueabi/sysroot/usr/include
    LIBS += -L/home/wp20/T113/SDK-yuanma/ATK-DLT113IS/out/t113_i/atk_dlt113is/buildroot/buildroot/host/arm-buildroot-linux-gnueabi/sysroot/usr/lib -lmodbus

#    INCLUDEPATH += /home/wp20/linux-imx6/tool/libmodbus-3.1.7/install/include
#    LIBS += -L/home/wp20/linux-imx6/tool/libmodbus-3.1.7/install/lib -lmodbus
} else {
    # Ubuntu x86_64目标架构
    DEFINES += UBUNTU_PLATFORM
    INCLUDEPATH += /home/wp20/tool/libmodbus-ubuntu/include
    LIBS += -L/home/wp20/tool/libmodbus-ubuntu/lib -lmodbus
}
SOURCES += \
    caxun.cpp \
    dengru.cpp \
    digitalkeyboardcpp.cpp \
    logindlg.cpp \
    main.cpp \
    mainwindow.cpp \
    modbusrtu.cpp \
    screen_3.cpp \
    sezhi.cpp \
    sqlitemanager.cpp \
    tiaoshi.cpp \
    yunxing.cpp \
    zhuye.cpp

HEADERS += \
    caxun.h \
    dengru.h \
    digitalkeyboardcpp.h \
    logindlg.h \
    mainwindow.h \
    modbusrtu.h \
    screen_3.h \
    sezhi.h \
    sqlitemanager.h \
    tiaoshi.h \
    yunxing.h \
    zhuye.h

FORMS += \
    caxun.ui \
    dengru.ui \
    logindlg.ui \
    mainwindow.ui \
    screen_3.ui \
    sezhi.ui \
    tiaoshi.ui \
    yunxing.ui \
    zhuye.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Resources.qrc

DISTFILES +=
