cmake_minimum_required(VERSION 3.16)
project(QMS VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)

find_package(QT NAMES Qt5 Qt6 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Gui)
find_package(Qt${QT_VERSION_MAJOR} OPTIONAL_COMPONENTS Widgets)

qt_standard_project_setup()

qt_add_executable(QMS WIN32 MACOSX_BUNDLE
    Configurator/Configurator.cpp Configurator/Configurator.h Configurator/Configurator.ui
    Dependencies/EnhancedDisplaySwitch/EnhancedDisplaySwitch.cpp Dependencies/EnhancedDisplaySwitch/EnhancedDisplaySwitch.h
    QMS/QMS.cpp QMS/QMS.h
    ShortcutManager/ShortcutManager.cpp ShortcutManager/ShortcutManager.h
    Utils/Utils.cpp Utils/Utils.h
    main.cpp
    Resources/appicon.rc
)

target_include_directories(QMS PRIVATE
    Configurator
    QMS
    ShortcutManager
    Utils
)

target_link_libraries(QMS PRIVATE
    Qt::Core
    Qt::Gui
    advapi32
    user32
    winmm
)


# Resources:
set(resources_resource_files
    "Resources/icons/icon.png"
    "Resources/icons/tray_icon.png"
)

qt_add_resources(QMS "resources"
    PREFIX
        "/"
    BASE
        "Resources"
    FILES
        ${resources_resource_files}
)

if((QT_VERSION_MAJOR GREATER 4))
    target_link_libraries(QMS PRIVATE
        Qt::Widgets
    )
endif()

