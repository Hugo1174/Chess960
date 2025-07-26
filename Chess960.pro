QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

HEADERS += \
    clickablelabel.h \
    gamewindow.h \
    guidewindow.h \
    mainwindow.h \
    networkmanager.h \
    networksetupdialog.h \
    promotiondialog.h \
    piece_logic.h
SOURCES += \
    clickablelabel.cpp \
    gamewindow.cpp \
    guidewindow.cpp \
    main.cpp \
    mainwindow.cpp \
    networkmanager.cpp \
    networksetupdialog.cpp \
    piece_logic.cpp \
    promotiondialog.cpp

FORMS += \
    gamewindow.ui \
    guidewindow.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    guide_img.qrc \
    pieces.qrc
