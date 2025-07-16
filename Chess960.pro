QT       += core gui network widgets

# greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
# Выше я объединил widgets с основной строкой для большей ясности.
# Это стандартная практика.

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

HEADERS += \
    NetworkDialog.h \
    clickablelabel.h \
    gamewindow.h \
    guidewindow.h \
    mainwindow.h \
    networkhandler.h \
    promotiondialog.h \
    piece_logic.h
SOURCES += \
    NetworkDialog.cpp \
    clickablelabel.cpp \
    gamewindow.cpp \
    guidewindow.cpp \
    main.cpp \
    mainwindow.cpp \
    networkhandler.cpp \
    promotiondialog.cpp \
    piece_logic.cpp

FORMS += \
    NetworkDialog.ui \
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
