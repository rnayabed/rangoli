#[[
Copyright (C) 2023 Debayan Sutradhar (rnayabed) (debayansutradhar3@gmail.com)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
]]

set(SOURCE_LIST
    customcolours.h customcolours.cpp
    customlightmodekeys.h customlightmodekeys.cpp
    custommappedkeys.h custommappedkeys.cpp
    enhanceddialog.h enhanceddialog.cpp
    hidconnection.h hidconnection.cpp
    hidconnectionworker.h hidconnectionworker.cpp
    icons.h icons.cpp
    key.h key.cpp
    keyboard.h keyboard.cpp
    keyboardconfiguratorcontroller.h keyboardconfiguratorcontroller.cpp
    keyboardmodel.h keyboardmodel.cpp
    keyboardusbid.h keyboardusbid.cpp
    keycode.h
    keycodemodel.h keycodemodel.cpp
    keycodemodelproxy.h keycodemodelproxy.cpp
    main.cpp
    mainwindowcontroller.h mainwindowcontroller.cpp
    messagehandler.h messagehandler.cpp
    mode.h mode.cpp
    modemodel.h modemodel.cpp
    profile.h profile.cpp
    profilemodel.h profilemodel.cpp
    settingscontroller.h settingscontroller.cpp
    startatboot.h startatboot.cpp

    resources.qrc
)

set(MACOS_APP_ICON "${CMAKE_SOURCE_DIR}/icons/rangoli.icns")

if (WIN32)
    list(APPEND SOURCE_LIST icon_windows.rc)
elseif(APPLE)
    list(APPEND SOURCE_LIST ${MACOS_APP_ICON})
endif()

# Ressourcen definieren
qt_add_resources(RANGOLI_RESOURCES
    resources.qrc
)

qt_add_executable(rangoli ${SOURCE_LIST} ${RANGOLI_RESOURCES})

# Linux: udev writer kopieren
if (CMAKE_SYSTEM_NAME MATCHES "Linux")
    add_custom_command(
        TARGET rangoli POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            "${CMAKE_CURRENT_SOURCE_DIR}/${LINUX_UDEV_RULE_WRITER_NAME}"
            "${CMAKE_CURRENT_BINARY_DIR}/${LINUX_UDEV_RULE_WRITER_NAME}"
    )

    # Installationsziel
    install(PROGRAMS "${CMAKE_CURRENT_SOURCE_DIR}/${LINUX_UDEV_RULE_WRITER_NAME}"
            DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()

# Keyboards-Ordner kopieren
add_custom_command(
    TARGET rangoli POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/keyboards"
        "${CMAKE_CURRENT_BINARY_DIR}/keyboards"
)

if (APPLE)
    install(DIRECTORY "${CMAKE_SOURCE_DIR}/keyboards"
            DESTINATION ${CMAKE_INSTALL_PREFIX})
else()
    install(DIRECTORY "${CMAKE_SOURCE_DIR}/keyboards"
            DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()


set_source_files_properties(${MACOS_APP_ICON} PROPERTIES
    MACOSX_PACKAGE_LOCATION "Resources"
)

set_source_files_properties(GlobalProps.qml PROPERTIES
    QT_QML_SINGLETON_TYPE TRUE
)

# QML Modul
qt_add_qml_module(rangoli
    URI Rangoli
    VERSION ${VERSION}
    QML_FILES
        Main.qml
        EnhancedButton.qml
        EnhancedDialog.qml
        KeyboardMenuButton.qml
        KeyboardConfigurator.qml
        Settings.qml
        Hyperlink.qml
        GlobalProps.qml
)

set_target_properties(rangoli PROPERTIES
    MACOSX_BUNDLE_BUNDLE_NAME Rangoli
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_COPYRIGHT ${MACOSX_BUNDLE_COPYRIGHT}
    MACOSX_BUNDLE_GUI_IDENTIFIER ${MACOSX_BUNDLE_GUI_IDENTIFIER}
    MACOSX_BUNDLE_ICON_FILE rangoli.icns
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)


# Add extra compiler flags supported by GCC
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(rangoli PRIVATE -Wpessimizing-move -Wpedantic)
endif()

target_compile_options(rangoli PRIVATE -Wall -Wextra -Werror)

# Libraries
target_link_libraries(rangoli
    PRIVATE Qt6::Quick Qt6::Svg
    PRIVATE hidapi::hidapi
)

install(TARGETS rangoli
    BUNDLE DESTINATION ${CMAKE_INSTALL_PREFIX}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})


# Move keyboards folder to build/install directory
add_custom_command(
    TARGET rangoli POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/keyboards"
    "${CMAKE_CURRENT_BINARY_DIR}/keyboards")

if (APPLE)
    install(DIRECTORY "${CMAKE_SOURCE_DIR}/keyboards"
        DESTINATION ${CMAKE_INSTALL_PREFIX})
else()
    install(DIRECTORY "${CMAKE_SOURCE_DIR}/keyboards"
        DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()


# Linux requires additional udev writer script
if (CMAKE_SYSTEM_NAME MATCHES "Linux")
    add_custom_command(
        TARGET rangoli POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_CURRENT_SOURCE_DIR}/${LINUX_UDEV_RULE_WRITER_NAME}"
        "${CMAKE_CURRENT_BINARY_DIR}")

    install(PROGRAMS "${CMAKE_CURRENT_SOURCE_DIR}/${LINUX_UDEV_RULE_WRITER_NAME}"
        DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()


# Use Qt Deployment API. Not yet supported on Linux
if (WIN32 OR APPLE)
    qt_generate_deploy_qml_app_script(
        TARGET rangoli
        OUTPUT_SCRIPT deploy_script
        NO_UNSUPPORTED_PLATFORM_ERROR
        NO_TRANSLATIONS
    )

    install(SCRIPT ${deploy_script})
endif()
