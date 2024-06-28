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

cmake_minimum_required(VERSION 3.16)

set(VERSION 2.0)

project(rangoli VERSION ${VERSION} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(LINUX_UDEV_RULE_WRITER_NAME "udev-rule-writer")

set(MACOSX_BUNDLE_COPYRIGHT "Debayan Sutradhar")
set(MACOSX_BUNDLE_GUI_IDENTIFIER io.github.rnayabed.rangoli)

add_compile_definitions(
    VERSION=${VERSION}
    GIT_REPOSITORY="https://github.com/rnayabed/rangoli"
    KEYBOARDS_SUPPORT_STATUS_LIST="https://github.com/rnayabed/rangoli/blob/master/keyboards-list.md"
    WEBSITE="https://rnayabed.github.io/rangoli"
    REPORT_ISSUE="https://github.com/rnayabed/rangoli/issues/new"
    LICENSE="https://github.com/rnayabed/rangoli/blob/master/LICENSE"
    AUTHOR_WEBSITE="https://github.com/rnayabed"
    AUTHOR_KO_FI="https://ko-fi.com/rnayabed"
    AUTHOR_UPI="9903299105@upi"
    AUTHOR_EMAIL="debayansutradhar3@gmail.com"
    GITHUB_RELEASES="https://api.github.com/repos/rnayabed/rangoli/releases"

    LINUX_UDEV_RULE_WRITER_NAME="${LINUX_UDEV_RULE_WRITER_NAME}"
    LINUX_UDEV_RULES_PATH="/etc/udev/rules.d/71-rangoli.rules"
    LINUX_TERMINAL="xterm"

    MACOSX_BUNDLE_COPYRIGHT="${MACOSX_BUNDLE_COPYRIGHT}"
    MACOSX_BUNDLE_GUI_IDENTIFIER="${MACOSX_BUNDLE_GUI_IDENTIFIER}"

    DEFAULT_ACCENT_RED=229
    DEFAULT_ACCENT_GREEN=21
    DEFAULT_ACCENT_BLUE=115
)

find_package(Qt6 COMPONENTS Quick Svg Widgets REQUIRED)

set(HIDAPI_WITH_LIBUSB FALSE)
set(BUILD_SHARED_LIBS FALSE)

add_subdirectory(hidapi)
add_subdirectory(src)

target_link_libraries(rangoli PRIVATE Qt6::Widgets)
