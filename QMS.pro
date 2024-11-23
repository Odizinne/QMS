QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

INCLUDEPATH += \
    QMS \
    Utils \
    ShortcutManager \
    Configurator \
    Dependencies/EnhancedDisplaySwitch

SOURCES += \
    Configurator/Configurator.cpp \
    ShortcutManager/ShortcutManager.cpp \
    main.cpp \
    QMS/QMS.cpp \
    Utils/Utils.cpp \
    Dependencies/EnhancedDisplaySwitch/EnhancedDisplaySwitch.cpp

HEADERS += \
    Configurator/Configurator.h \
    QMS/QMS.h \
    ShortcutManager/ShortcutManager.h \
    Utils/Utils.h \
    Dependencies/EnhancedDisplaySwitch/EnhancedDisplaySwitch.h

FORMS += \
    Configurator/Configurator.ui \

RESOURCES += \
    Resources/resources.qrc \

RC_FILE = Resources/appicon.rc

LIBS += -luser32 -ladvapi32 -lwinmm

DEPENDENCIES_DIR = $$PWD/dependencies
DEST_DIR = $$OUT_PWD/release/dependencies

