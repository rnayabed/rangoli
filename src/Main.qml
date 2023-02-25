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
import QtQuick.Controls.Material
import QtQuick.Controls.Material.impl
import Qt.labs.platform
import QtQml.Models
import QtQml.WorkerScript

import Rangoli

ApplicationWindow {
    visible: true

    width: 900
    height: 700

    minimumWidth: 500
    minimumHeight: 300

    id: window

    Connections {
        target: mainWindowController

        function onAdjustOptionsSize(length)
        {
            options.adjustSize(length)
        }

        function onLoadEnhancedDialog(dialogController)
        {
            Qt.createComponent("EnhancedDialog.qml")
              .createObject(window, {controller: dialogController})
        }

        function onShow()
        {
            show()
        }

        function onHide()
        {
            hide()
            if (mainWindowController.startToSystemTrayIcon) systemTrayIcon.visible = true
        }

        function onClose()
        {
            Qt.quit()
        }

        function onSetSystemTheme()
        {
            window.Material.theme = Material.System
        }

        function onSetLightTheme()
        {
            window.Material.theme = Material.Light
        }

        function onSetDarkTheme()
        {
            window.Material.theme = Material.Dark
        }

        function onSetAccentColour(accentColour)
        {
            window.Material.accent = accentColour
        }

        function onAlwaysShowSystemTrayIconChanged()
        {
            systemTrayIcon.visible = mainWindowController.alwaysShowSystemTrayIcon
        }

        function onOpenLinuxUdevPopup()
        {
            linuxUdevPopup.open()
        }

        function onCloseLinuxUdevPopup()
        {
            linuxUdevPopup.close();
        }
    }

    Component.onCompleted: mainWindowController.init()

    onClosing: (close) => {
        if (mainWindowController.closeToSystemTrayIcon)
        {
            close.accepted = false
            systemTrayIcon.visible = true
            window.hide()
            sv.pop(null)
            return
        }

        close.accepted = !mainWindowController.interruptClose
        if (mainWindowController.interruptClose)
        {
            window.show()
            window.raise()
            window.requestActivate()
            mainWindowController.closeInterrupted()
        }
    }

    SystemTrayIcon {
        id: systemTrayIcon

        tooltip: qsTr("Rangoli")

        icon.source: Icons.get(Icons.Rangoli)

        onActivated: (reason) => {
            if (reason === SystemTrayIcon.Context)
            {
                systemTrayMenu.open();
            }
            else if (reason === SystemTrayIcon.Trigger)
            {
                if (!mainWindowController.alwaysShowSystemTrayIcon) systemTrayIcon.visible = false

                window.show()
                window.raise()
                window.requestActivate()
            }
        }

        menu: Menu {
            id: systemTrayMenu

            Instantiator {
                model: mainWindowController.connectedKeyboards

                delegate: Menu {
                    title: model.name

                    id: kbMenuDel

                    property int index: model.index

                    Instantiator {
                        model: keyboardConfiguratorController.profiles

                        delegate: MenuItem {
                            text: model.name
                            onTriggered: {
                                mainWindowController.loadKeyboard(kbMenuDel.index)
                                keyboardConfiguratorController.selectedProfileIndex = model.index
                            }
                        }

                        onObjectAdded: (index, object) => kbMenuDel.insertItem(index, object)
                        onObjectRemoved: (index, object) => kbMenuDel.removeItem(object)
                    }
                }

                onObjectAdded: (index, object) => systemTrayMenu.insertMenu(index, object)
                onObjectRemoved: (index, object) => systemTrayMenu.removeMenu(object)
            }

            MenuSeparator {}

            MenuItem {
                text: qsTr("Quit")

                onTriggered: {
                    mainWindowController.closeToSystemTrayIcon = false
                    mainWindowController.interruptClose = false
                    Qt.quit()
                }
            }
        }
    }

    Timer {
        id: delayedGC
        repeat: false
        interval: 10000
        onTriggered: gc()
    }

    Item {
        id: mainScreen

        Image {
            id: appIcon

            anchors {
                bottom: parent.bottom
                bottomMargin: - (height / 2)
                horizontalCenter: parent.horizontalCenter
            }

            source: Icons.get(Icons.Rangoli)

            sourceSize.width: 750
            sourceSize.height: 750

            opacity: 0.4
        }

        Label {
            id: appTitle

            anchors {
                top: parent.top
                topMargin: 50
                horizontalCenter: parent.horizontalCenter
            }

            text: qsTr("Rangoli")
            font.pixelSize: 50
        }

        RoundButton {
            id: settingsBtn

            anchors {
                top: parent.top
                topMargin: 10
                right: parent.right
                rightMargin: 10
            }

            flat: true
            icon.source: Icons.get(Icons.Settings)

            onClicked: {
                delayedGC.stop()
                sv.push(settings)
            }

            ToolTip.visible: hovered
            ToolTip.text: qsTr("Settings")
            ToolTip.delay: mainWindowController.toolTipDelay
            ToolTip.timeout: mainWindowController.toolTipTimeout
        }

        Row
        {
            id: connectAKeyboardPrompt

            visible: options.count === 0 && ! mainWindowController.keyboardsScanning

            anchors {
                top: appTitle.bottom
                topMargin: 50
                horizontalCenter: parent.horizontalCenter
            }

            spacing: 5

            Label {
                anchors.verticalCenter: parent.verticalCenter

                text: qsTr("Connect a Royal Kludge Keyboard to get started!");

                font.pixelSize: 15
            }

            RoundButton {
                flat: true

                icon.source: Icons.get(Icons.Help)

                onClicked: getStartedPopup.open()

                ToolTip.visible: hovered
                ToolTip.text: qsTr("Know More")
                ToolTip.delay: mainWindowController.toolTipDelay
                ToolTip.timeout: mainWindowController.toolTipTimeout
            }
        }



        GridView {
            id: options

            clip: true

            anchors {
                top: appTitle.bottom
                topMargin: 30
                horizontalCenter: parent.horizontalCenter
                bottom: refreshBtn.top
                bottomMargin: 30
            }

            model: mainWindowController.connectedKeyboards

            width: (parent.width > implicitWidth) ? implicitWidth : parent.width - 20

            function adjustSize(length)
            {
                options.implicitWidth = length * 300
            }

            cellWidth: 300
            cellHeight: 150

            ScrollBar.vertical: ScrollBar {
                policy: ScrollBar.AsNeeded
            }

            delegate: KeyboardMenuButton {
                opacity: 1
                kbName: model.name
                kbImage: model.imgPath

                button.onClicked: {
                    delayedGC.stop()
                    sv.push(keyboardConfigurator)
                    mainWindowController.loadKeyboard(model.index)
                }
            }
        }

        RoundButton {
            id: refreshBtn

            anchors {
                bottom: parent.bottom
                bottomMargin: 10
                horizontalCenter: parent.horizontalCenter
            }

            highlighted: true
            icon.source: Icons.get(Icons.Refresh)

            onClicked: mainWindowController.refreshKeyboards()

            ToolTip.visible: hovered
            ToolTip.text: qsTr("Refresh")
            ToolTip.delay: mainWindowController.toolTipDelay
            ToolTip.timeout: mainWindowController.toolTipTimeout
        }
    }

    Component {
        id: keyboardConfigurator

        KeyboardConfigurator {
            onReturnToHome: {
                delayedGC.restart()
                sv.pop()
            }

            onOpenSettings: {
                delayedGC.stop()
                sv.push(settings)
            }
        }
    }

    Component {
        id: settings

        Settings {
            onReturnToHome: {
                delayedGC.restart()
                sv.pop()
            }
        }
    }

    StackView {
        id: sv

        anchors {
            fill: parent
        }

        initialItem: mainScreen
    }

    Popup {
        id: getStartedPopup

        anchors.centerIn: parent
        margins: 10
        modal: true

        Column {
            spacing: 10

            Label {
                text: qsTr("Connect a supported keyboard")
                font.bold: true
                font.pixelSize: 15
                bottomPadding: 10
            }

            Label {
                text: qsTr("If Rangoli is unable to detect your keyboard, check if it is supported yet.
Please report an issue if your keyboard is unsupported, or is supported but undetected.")
                width: parent.width
                wrapMode: Text.Wrap
            }

            Row {
                anchors.right: parent.right

                spacing: 10

                Button {
                    text: qsTr("Open Supported Keyboards List")

                    flat: true
                    highlighted: true

                    onClicked: mainWindowController.openSupportedKeyboardsList()
                }

                Button {
                    text: qsTr("Report Issue")

                    flat: true
                    highlighted: true

                    onClicked: settingsController.reportIssue()
                }

                Button {
                    text: qsTr("OK")

                    flat: true
                    highlighted: true

                    onClicked: getStartedPopup.close()
                }
            }
        }
    }



    Popup {
        id: linuxUdevPopup

        anchors.centerIn: parent
        margins: 10
        modal: true

        closePolicy: Popup.NoAutoClose

        Item {
            anchors.fill: parent

            id: linuxUdevPopupItem

            implicitHeight: linuxUdevPopupTitle.implicitHeight +
                            linuxUdevPopupScrollView.anchors.topMargin +
                            linuxUdevPopupScrollView.contentHeight +
                            linuxUdevPopupProceedButton.anchors.topMargin +
                            linuxUdevPopupProceedButton.implicitHeight

            implicitWidth: 400

            Label {
                id: linuxUdevPopupTitle

                anchors {
                    top: parent.top
                    left: parent.left
                }

                text: qsTr("Note")
                font.bold: true
                font.pixelSize: 15
            }

            ScrollView {
                id: linuxUdevPopupScrollView

                anchors {
                    top: linuxUdevPopupTitle.bottom
                    topMargin: 20
                    left: parent.left
                }

                contentHeight: linuxUdevPopupLabel.implicitHeight
                contentWidth: parent.width

                height: linuxUdevPopupItem.height -
                        anchors.topMargin -
                        linuxUdevPopupTitle.height -
                        linuxUdevPopupProceedButton.anchors.topMargin -
                        linuxUdevPopupProceedButton.height

                width: parent.width

                ScrollBar.vertical.policy: ScrollBar.AsNeeded

                clip : true

                Label {
                    id: linuxUdevPopupLabel

                    text: qsTr("Rangoli uses the HIDRAW driver of the Linux kernel to send information to your keyboard.
However, for safety purposes it requires permissions to do so. In order to accquire these permissions, additional rules has to be written in '%1'.

You will be asked for sudo password.
This action will only be performed on first run, after an update or when Rangoli detects that it does not have sufficient permissions.").arg(mainWindowController.linuxUdevRulesPath)

                    rightPadding: linuxUdevPopupScrollView.ScrollBar.vertical.width
                    width: parent.width - rightPadding
                    wrapMode: Text.Wrap
                }
            }

            Button {
                id: linuxUdevPopupProceedButton

                anchors {
                    top: linuxUdevPopupScrollView.bottom
                    topMargin: 10
                    right: parent.right
                }

                text: qsTr("Proceed")

                enabled: mainWindowController.linuxUdevPopupProceedButtonEnabled

                flat: true
                highlighted: true

                onClicked: mainWindowController.launchLinuxUdevWriter()
            }
        }
    }
}
