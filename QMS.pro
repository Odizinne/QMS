QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += \
    src/QMS \
    src/Utils

SOURCES += \
    src/main.cpp \
    src/QMS/qms.cpp \
    src/Utils/utils.cpp

HEADERS += \
    src/QMS/qms.h \
    src/Utils/utils.h

RESOURCES += \
    src/Resources/resources.qrc \

LIBS += -luser32

RC_FILE = src/Resources/appicon.rc

DEPENDENCIES_DIR = $$PWD/dependencies
DEST_DIR = $$OUT_PWD/release/dependencies

QMAKE_POST_LINK += powershell -Command "New-Item -ItemType Directory -Path '$$DEST_DIR' -Force; Copy-Item -Path '$$DEPENDENCIES_DIR\*' -Destination '$$DEST_DIR' -Recurse -Force"

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
