/*
 * Copyright (C) 2023 Debayan Sutradhar (rnayabed) (debayansutradhar3@gmail.com)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material 2.15
import QtQuick.Controls.Material.impl 2.15

Item {
    property alias kbName: kbName.text
    property alias kbImage: kbImage.source

    property alias button: button

    implicitWidth: 300
    implicitHeight: 150

    Button
    {
        id: button

        topInset: 0
        bottomInset: 0

        anchors {
            fill: parent
            margins: 5
        }

        Material.background: (Material.theme == Material.Light) ? "#ffffff" : "#444444"

        contentItem: Item {
            anchors {
                fill: parent
            }

            Image {
                id: kbImage

                anchors {
                    top: parent.top
                    topMargin: 10
                    horizontalCenter: parent.horizontalCenter
                }

                sourceSize.width: 230

                fillMode: Image.PreserveAspectFit
            }

            Label {
                id: kbName

                anchors {
                    bottom: parent.bottom
                    bottomMargin: 10
                    horizontalCenter: parent.horizontalCenter
                }
            }
        }

        ToolTip.visible: hovered
        ToolTip.text: qsTr("Click to configure the %1 connected to your PC.").arg(kbName.text)
        ToolTip.delay: mainWindowController.toolTipDelay
        ToolTip.timeout: mainWindowController.toolTipTimeout
    }
}

