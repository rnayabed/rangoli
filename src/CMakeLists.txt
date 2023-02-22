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
    main.cpp
    resources.qrc
    key.h key.cpp
    keyboard.h keyboard.cpp
    keyboardusbid.h keyboardusbid.cpp
    profile.h profile.cpp
    mode.h mode.cpp
    hidconnection.h hidconnection.cpp
    hidconnectionworker.h hidconnectionworker.cpp
    keycode.h
    enhanceddialog.h enhanceddialog.cpp
    icons.h icons.cpp
    mainwindowcontroller.h mainwindowcontroller.cpp
    keyboardmodel.h keyboardmodel.cpp
    keyboardconfiguratorcontroller.h keyboardconfiguratorcontroller.cpp
    customlightmodekeys.h customlightmodekeys.cpp
    modemodel.h modemodel.cpp
    profilemodel.h profilemodel.cpp
    settingscontroller.h settingscontroller.cpp
    customcolours.h customcolours.cpp
    custommappedkeys.h custommappedkeys.cpp
    keycodemodel.h keycodemodel.cpp
    keycodemodelproxy.h keycodemodelproxy.cpp
    startatboot.h startatboot.cpp
)

if (WIN32)
    list(APPEND SOURCE_LIST icon_windows.rc)
endif()

qt_add_executable(rangoli ${SOURCE_LIST})

qt_add_qml_module(rangoli
    URI Rangoli
    VERSION 1.0
    QML_FILES
        Main.qml
        EnhancedDialog.qml
        KeyboardMenuButton.qml
        KeyboardConfigurator.qml
        Settings.qml
        Hyperlink.qml
)

set_target_properties(rangoli PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER io.github.rnayabed.rangoli
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(rangoli PRIVATE -Wpessimizing-move -Wpedantic)
endif()

target_compile_options(rangoli PRIVATE -Wall -Wextra -Werror)

target_link_libraries(rangoli
    PRIVATE Qt6::Quick
    PRIVATE hidapi::hidapi)

install(TARGETS rangoli
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

add_custom_command(
    TARGET rangoli POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/keyboards"
    "${CMAKE_CURRENT_BINARY_DIR}/keyboards")

install(DIRECTORY "${CMAKE_SOURCE_DIR}/keyboards"
    DESTINATION "${CMAKE_INSTALL_PREFIX}/bin")

if (UNIX AND NOT APPLE)
    add_custom_command(
        TARGET rangoli POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_CURRENT_SOURCE_DIR}/${LINUX_UDEV_RULE_WRITER_NAME}"
        "${CMAKE_CURRENT_BINARY_DIR}")

    install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/${LINUX_UDEV_RULE_WRITER_NAME}"
        DESTINATION "${CMAKE_INSTALL_PREFIX}/bin")
endif()

if (WIN32 OR APPLE)
    qt_generate_deploy_qml_app_script(
        TARGET rangoli
        FILENAME_VARIABLE deploy_script
        NO_UNSUPPORTED_PLATFORM_ERROR
    )

    install(SCRIPT ${deploy_script})
endif()
