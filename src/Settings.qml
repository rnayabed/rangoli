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
import QtQuick.Dialogs

import Rangoli

Item {

    id: settings

    signal returnToHome()

    Component.onCompleted: {
        donateButton.configureText()
    }

    onReturnToHome: {
        if (settingsController.unsavedChanges) settingsController.load()
    }

    onWidthChanged: donateButton.configureText()

    Rectangle {

        color: (Material.theme == Material.Light) ? "#e6e6e6" : "#444444"

        id: header

        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }

        height: returnButton.implicitHeight + 5

        layer.enabled: true
        layer.effect: ElevationEffect {
            elevation: 1
        }

        RoundButton {
            id: returnButton

            anchors {
                left: parent.left
                leftMargin: 10
                verticalCenter: parent.verticalCenter
            }

            flat: true
            icon.source: Icons.get(Icons.ArrowBack)
            onClicked: settings.returnToHome()

            ToolTip.visible: hovered
            ToolTip.text: qsTr("Return")
            ToolTip.delay: mainWindowController.toolTipDelay
            ToolTip.timeout: mainWindowController.toolTipTimeout
        }

        Label {
            id: title

            anchors {
                left: returnButton.right
                leftMargin: 10
                verticalCenter: parent.verticalCenter
            }

            text: qsTr("Settings")

            font.pixelSize: 20
        }

        Button {
            id: donateButton

            anchors {
                right: websiteButton.left
                rightMargin: 10
                verticalCenter: parent.verticalCenter
            }

            readonly property int minWidthRequiredForDonateText: 600

            function configureText()
            {
                if (text.length === 0 && settings.width > minWidthRequiredForDonateText)
                {
                    text = qsTr("Donate")
                }
                else if (text.length > 0 && settings.width < minWidthRequiredForDonateText)
                {
                    text = ""
                }
            }

            highlighted: true
            icon.source: Icons.get(Icons.Donate)

            onClicked: {
                if (donateButton.text.length > 0)
                {
                    text = qsTr("Thank you so much!")
                }
                settingsController.openAuthorKoFi()
            }
        }


        RoundButton {
            id: websiteButton

            anchors {
                right: reportIssueButton.left
                rightMargin: 10
                verticalCenter: parent.verticalCenter
            }

            flat: true
            icon.source: Icons.get(Icons.Website)

            onClicked: settingsController.openWebsite()

            ToolTip.visible: hovered
            ToolTip.text: qsTr("Open Website")
            ToolTip.delay: mainWindowController.toolTipDelay
            ToolTip.timeout: mainWindowController.toolTipTimeout
        }

        RoundButton {
            id: reportIssueButton

            anchors {
                right: resetButton.left
                rightMargin: 10
                verticalCenter: parent.verticalCenter
            }

            flat: true
            icon.source: Icons.get(Icons.BugReport)

            onClicked: settingsController.reportIssue()

            ToolTip.visible: hovered
            ToolTip.text: qsTr("Report an Issue")
            ToolTip.delay: mainWindowController.toolTipDelay
            ToolTip.timeout: mainWindowController.toolTipTimeout
        }

        RoundButton {
            id: resetButton

            anchors {
                right: saveButton.left
                rightMargin: 10
                verticalCenter: parent.verticalCenter
            }

            flat: true
            icon.source: Icons.get(Icons.Reset)

            onClicked: settingsController.load()

            ToolTip.visible: hovered
            ToolTip.text: qsTr("Discard unsaved changes")
            ToolTip.delay: mainWindowController.toolTipDelay
            ToolTip.timeout: mainWindowController.toolTipTimeout
        }

        RoundButton {
            id: saveButton

            anchors {
                right: parent.right
                rightMargin: 10
                verticalCenter: parent.verticalCenter
            }

            flat: ! settingsController.unsavedChanges
            highlighted: settingsController.unsavedChanges
            enabled: ! settingsController.savingChanges

            icon.source: Icons.get(Icons.Save)

            onClicked: settingsController.save()

            ToolTip.visible: hovered
            ToolTip.text: qsTr("Apply changes")
            ToolTip.delay: mainWindowController.toolTipDelay
            ToolTip.timeout: mainWindowController.toolTipTimeout
        }
    }

    ScrollView {
        id: sv

        anchors {
            top: header.bottom
            bottom: parent.bottom
            horizontalCenter: parent.horizontalCenter
        }

        width: Math.min(parent.width, 500)

        Item {
            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
                margins: 10
                rightMargin: sv.ScrollBar.vertical.width + anchors.margins
                bottomMargin: 50
            }

            implicitHeight: settingsCol.implicitHeight + (anchors.margins * 2) + anchors.bottomMargin

            Column {
                spacing: 5

                anchors.fill: parent

                id: settingsCol


                ComboBoxSetting {
                    displayText: qsTr("Theme")

                    comboBox {
                        model: settingsController.themes

                        currentIndex: settingsController.selectedThemeIndex
                        onCurrentIndexChanged: {
                            settingsController.selectedThemeIndex = comboBox.currentIndex
                            settingsController.unsavedChanges = true
                        }
                    }
                }

                ColourDialogSetting {
                    displayText: qsTr("Accent Colour")

                    colourBox.color: settingsController.accentColour

                    colourDialog {
                        onAccepted: {
                            settingsController.accentColour = colourDialog.selectedColor
                            settingsController.unsavedChanges = true
                        }
                    }
                }

                ComboBoxSetting {
                    displayText: qsTr("Default Profile")

                    comboBox {
                        model: settingsController.profiles
                        textRole: "name"
                        currentIndex: settingsController.selectedDefaultProfileIndex
                        width: 150
                        onCurrentIndexChanged: {
                            settingsController.selectedDefaultProfileIndex = comboBox.currentIndex
                            settingsController.unsavedChanges = true
                            comboBox.ToolTip.text = keyboardConfiguratorController.profiles.profileName(comboBox.currentIndex)
                        }

                        ToolTip.visible: comboBox.hovered
                        ToolTip.delay: mainWindowController.toolTipDelay
                        ToolTip.timeout: mainWindowController.toolTipTimeout
                    }
                }

                SwitchSetting {
                    displayText: qsTr("Start on system boot")

                    visible: settingsController.startOnBootSupported

                    switchControl {
                        checked: settingsController.startOnBoot
                        onCheckedChanged: {
                            settingsController.startOnBoot = switchControl.checked
                            settingsController.unsavedChanges = true
                        }
                    }
                }

                SwitchSetting {
                    displayText: qsTr("Apply default profile when Rangoli starts")

                    switchControl {
                        checked: settingsController.applyDefaultProfileOnStartup
                        onCheckedChanged: {
                            settingsController.applyDefaultProfileOnStartup = switchControl.checked
                            settingsController.unsavedChanges = true
                        }
                    }
                }

                SwitchSetting {
                    displayText: qsTr("Check for updates when Rangoli starts")

                    switchControl {
                        checked: settingsController.checkForUpdatesOnStartup
                        onCheckedChanged: {
                            settingsController.checkForUpdatesOnStartup = switchControl.checked
                            settingsController.unsavedChanges = true
                        }
                    }
                }

                SwitchSetting {
                    displayText: qsTr("Always show tray icon")

                    switchControl {
                        checked: settingsController.alwaysShowSystemTrayIcon
                        onCheckedChanged: {
                            settingsController.alwaysShowSystemTrayIcon = switchControl.checked
                            settingsController.unsavedChanges = true
                        }
                    }
                }

                SwitchSetting {
                    displayText: qsTr("Close to tray icon")

                    switchControl {
                        checked: settingsController.closeToSystemTrayIcon
                        onCheckedChanged: {
                            settingsController.closeToSystemTrayIcon = switchControl.checked
                            settingsController.unsavedChanges = true
                        }
                    }
                }

                SubHeading {
                    text: qsTr("About")
                }

                Item {
                    implicitWidth: parent.width
                    implicitHeight: checkForUpdatesButton.implicitHeight + 10

                    Label {
                        anchors {
                            left: parent.left
                            verticalCenter: checkForUpdatesButton.verticalCenter
                        }

                        text: qsTr("Rangoli Version %1").arg(settingsController.version)
                    }

                    Button {
                        id: checkForUpdatesButton

                        anchors {
                            right: parent.right
                            top: parent.top
                        }

                        text: qsTr("Check for Updates");
                        highlighted: true

                        enabled: settingsController.checkForUpdatesButtonEnabled

                        onClicked: settingsController.checkForUpdates()
                    }
                }

                Row
                {
                    Label {
                        text: qsTr("Developed by ")
                    }

                    Hyperlink {
                        text: "Debayan Sutradhar"
                        mouseArea.onClicked: settingsController.openAuthorWebsite()
                    }

                    Label {
                        text: qsTr(". Copyright 2023.")
                    }
                }

                Row
                {
                    Label {
                        text: qsTr("Mail: ")
                    }

                    Hyperlink {
                        text: settingsController.authorEmail
                        mouseArea.onClicked: settingsController.emailAuthor()
                    }
                }

                Row {
                    Label {
                        text: qsTr("Rangoli is licensed under the ")
                    }

                    Hyperlink {
                        text: "GNU General Public License v3.0"
                        mouseArea.onClicked: settingsController.openLicense()
                    }
                }

                Row {
                    Hyperlink {
                        text: "Source Code Repository"
                        mouseArea.onClicked: settingsController.openGitRepository()
                    }

                    Label {
                        text: qsTr(" - Consider gifting a star ^^")
                    }
                }

                SubHeading {
                    text: qsTr("Donate")
                }

                Label {
                    width: parent.width
                    wrapMode: Text.Wrap
                    text: qsTr("It takes a lot of time and effort to build quality software. Anything will be highly appreciated <3")
                }

                Label {
                    text: "UPI: " + settingsController.authorUPI
                }

                Hyperlink {
                    text: "Ko-Fi"
                    mouseArea.onClicked: settingsController.openAuthorKoFi()
                }
            }
        }
    }

    component SubHeading: Column {

        property alias text: shLabel.text

        width: parent.width
        height: implicitHeight + 10

        Label {
            id: shLabel
            topPadding: 20
            bottomPadding: 10
            font.pixelSize: 15
        }

        Rectangle {
            color: "grey"
            height: 1
            width: parent.width
        }
    }


    component ComboBoxSetting: Item {
        width: parent.width
        height: Math.max(cbsLabel.implicitHeight, cbsComboBox.implicitHeight)

        property alias displayText: cbsLabel.text
        property alias comboBox: cbsComboBox

        Label {
            id: cbsLabel

            anchors {
                left: parent.left
                verticalCenter: parent.verticalCenter
            }
        }

        ComboBox {
            id: cbsComboBox

            anchors {
                right: parent.right
                verticalCenter: parent.verticalCenter
            }
        }
    }

    component ColourDialogSetting: Item {
        width: parent.width
        height: Math.max(cdsColourBox.implicitHeight, cdsPickButton.height)

        property alias displayText: cdsLabel.text
        property alias colourBox: cdsColourBox
        property alias pickButton: cdsPickButton
        property alias colourDialog: cdsColourDialog

        Label {
            id: cdsLabel

            anchors {
                left: parent.left
                verticalCenter: parent.verticalCenter
            }
        }

        Rectangle {
            id: cdsColourBox

            anchors {
                right: cdsPickButton.left
                rightMargin: 10
                verticalCenter: parent.verticalCenter
            }

            height: 30
            width: 30

            radius: 3
            border.width: 1
            border.color: "#979797"
        }

        Button {
            id: cdsPickButton

            anchors {
                right: parent.right
                verticalCenter: parent.verticalCenter
            }

            text: qsTr("Pick")

            onClicked: cdsColourDialog.open()
        }

        ColorDialog {
            id: cdsColourDialog

            options: ColorDialog.DontUseNativeDialog
        }
    }

    component SwitchSetting: Item {
        width: parent.width
        height: Math.max(ssLabel.implicitHeight, ssSwitch.implicitHeight)

        property alias displayText: ssLabel.text
        property alias switchControl: ssSwitch

        Label {
            id: ssLabel

            anchors {
                left: parent.left
                verticalCenter: parent.verticalCenter
            }
        }

        Switch {
            id: ssSwitch

            anchors {
                right: parent.right
                verticalCenter: parent.verticalCenter
            }
        }
    }

}
