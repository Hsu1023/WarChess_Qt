QT       += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ai.cpp \
    aicontroller.cpp \
    algorithm.cpp \
    animation.cpp \
    character.cpp \
    characterlabel.cpp \
    gamelabel.cpp \
    main.cpp \
    maindialog.cpp \
    map.cpp \
    playingmenu.cpp

HEADERS += \
    ai.h \
    aicontroller.h \
    algorithm.h \
    animation.h \
    character.h \
    characterlabel.h \
    config.h \
    gamelabel.h \
    maindialog.h \
    map.h \
    playingmenu.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    music.qrc \
    pic.qrc

DISTFILES += \
    pic/warrior.gif
