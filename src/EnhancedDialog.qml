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
import QtQuick.Dialogs

import Rangoli

Dialog
{
    id: dialog
    property var controller

    Component.onCompleted: {
        switch (controller.buttonsType) {
        case 0:
            dialog.standardButtons = Dialog.Ok
            break
        case 1:
            dialog.standardButtons = Dialog.Yes | Dialog.No
            break
        }

        open()
    }

    anchors.centerIn: parent
    margins: 10
    modal: true
    closePolicy: Popup.NoAutoClose
    title: controller.title

    width: Math.min(400, implicitWidth)

    Label {
        anchors.fill: parent
        wrapMode: Label.Wrap
        text: controller.text
    }

    onClosed: {
        controller.onClosed()
    }

    onAccepted: {
        controller.onAccepted()
    }

    onRejected: {
        controller.onRejected()
    }
}
