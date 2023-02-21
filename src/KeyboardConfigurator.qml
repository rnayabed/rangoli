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
import QtQuick.Layouts
import QtQuick.Controls.Material
import QtQuick.Controls.Material.impl

import Rangoli

Item {
    id: kbConfigurator

    Connections {
        target: keyboardConfiguratorController

        function onLoadName(name)
        {
            kbName.text = name
        }

        function onLoadImage(source)
        {
            kbCanvas.loadImageSource(source)
        }

        function onCustomLightModeKeysChanged()
        {
            kbCanvas.requestPaint()
        }

        function onIsCustomLightModeSelectedChanged()
        {
            kbCanvas.requestPaint()
        }

        function onSelectedModeChanged()
        {
            kbCanvas.requestPaint()
        }

        function onSelectedColourChanged()
        {
            if (!keyboardConfiguratorController.isCustomLightModeSelected)
            {
                kbCanvas.requestPaint()
            }
        }

        function onRandomColoursChanged()
        {
            kbCanvas.requestPaint()
        }

        function onOpenConfigureKeyMapPopup()
        {
            configureKeyMapPopup.open()
            keyMapClickKeyToEditPrompt.opacity = 0
        }

        function onRequestKbCanvasPaint()
        {
            kbCanvas.requestPaint()
        }
    }

    signal returnToHome()
    signal openSettings()

    readonly property int animationDuration: 200

    Popup {
        id: profileNameEditorPopup

        anchors.centerIn: parent
        margins: 10
        modal: true

        property int index: -1

        function showNew()
        {
            show(-1, null)
        }

        function show(index, name)
        {
            this.index = index

            if (this.index > -1)
            {
                profileNameTextField.text = name
            }

            open()
        }

        Column {
            anchors.fill: parent

            spacing: 10

            Label {
                text: (profileNameEditorPopup.index === -1) ? qsTr("New Profile") : qsTr("Edit Profile")
                font.bold: true
                font.pixelSize: 15
                bottomPadding: 10
            }

            Label {
                text: qsTr("Name");
            }

            TextField {
                id: profileNameTextField
            }

            Row {
                anchors {
                    right: parent.right
                }

                spacing: 10

                Button {
                    text: qsTr("Save")
                    flat: true
                    highlighted: true

                    enabled: profileNameTextField.text.length > 0

                    onClicked: {
                        profileNameEditorPopup.close()

                        if (profileNameEditorPopup.index == -1)
                        {
                            keyboardConfiguratorController.createAndSelectNewProfile(profileNameTextField.text)
                            editProfilesPopup.close()
                        }
                        else
                        {
                            keyboardConfiguratorController.setProfileName(
                                                               profileNameEditorPopup.index,
                                                               profileNameTextField.text)
                        }

                    }
                }

                Button {
                    text: qsTr("Close")
                    flat: true
                    highlighted: true

                    onClicked: profileNameEditorPopup.close()
                }
            }
        }
    }

    Popup {
        id: editProfilesPopup

        anchors.centerIn: parent
        margins: 10
        modal: true

        Column {
            spacing: 10

            Item {
                width: parent.width
                height: Math.max(editProfilesLabel.implicitHeight, newProfileButton.implicitHeight)

                Label {
                    id: editProfilesLabel

                    anchors {
                        left: parent.left
                        verticalCenter: parent.verticalCenter
                    }

                    text: qsTr("Edit Profiles")
                    font.bold: true
                    font.pixelSize: 15
                }

                RoundButton {
                    id: newProfileButton

                    anchors {
                        right: parent.right
                        verticalCenter: parent.verticalCenter
                    }

                    flat: true
                    icon.source: Icons.get(Icons.Add)

                    onClicked: profileNameEditorPopup.showNew()

                    ToolTip.visible: hovered
                    ToolTip.text: qsTr("Add new profile")
                    ToolTip.delay: mainWindowController.toolTipDelay
                    ToolTip.timeout: mainWindowController.toolTipTimeout
                }
            }

            ListView {

                id: editProfilesListView

                clip: true

                height: 200
                width: 300

                model: keyboardConfiguratorController.profiles

                ScrollBar.vertical: ScrollBar {
                    policy: ScrollBar.AsNeeded
                }

                delegate: Row {

                    Label {
                        text: model.name

                        width: editProfilesListView.width - profileEditButton.width - profileDeleteButton.width -
                               editProfilesListView.ScrollBar.vertical.width

                        elide: Text.ElideRight

                        anchors.verticalCenter: parent.verticalCenter
                    }

                    RoundButton {
                        id: profileEditButton

                        flat: true
                        icon.source: Icons.get(Icons.Edit)

                        onClicked: profileNameEditorPopup.show(model.index, model.name)
                    }

                    RoundButton {
                        id: profileDeleteButton

                        flat: true
                        Material.foreground: Material.Red
                        icon.source: Icons.get(Icons.Delete)

                        enabled: ( model.index !== keyboardConfiguratorController.selectedProfileIndex ) && ( model.index !== keyboardConfiguratorController.profiles.defaultProfileIndex)

                        onClicked: keyboardConfiguratorController.deleteProfile(model.index)
                    }
                }
            }

            Button {
                anchors.right: parent.right

                text: qsTr("Close")
                flat: true
                highlighted: true

                onClicked: editProfilesPopup.close()
            }
        }
    }

    Popup {
        id: configureKeyMapPopup

        anchors.centerIn: parent
        margins: 10
        modal: true

        Item {
            anchors.fill: parent

            id: configureKeyMapPopupItem

            implicitHeight: configureKeyMapPopupTitle.implicitHeight +
                            configureKeyMapPopupScrollView.anchors.topMargin +
                            configureKeyMapPopupScrollView.contentHeight +
                            configureKeyMapPopupButtons.anchors.topMargin +
                            configureKeyMapPopupButtons.implicitHeight

            implicitWidth: configureKeyMapPopupScrollView.implicitWidth

            Label {
                id: configureKeyMapPopupTitle

                anchors {
                    top: parent.top
                    left: parent.left
                }

                text: qsTr("Configure Key Map")
                font.bold: true
                font.pixelSize: 15
            }

            ScrollView {
                id: configureKeyMapPopupScrollView

                anchors {
                    top: configureKeyMapPopupTitle.bottom
                    topMargin: 10
                    left: parent.left
                }

                contentHeight: configureKeyMapPopupColumn.height
                contentWidth: configureKeyMapPopupColumn.width +
                              configureKeyMapPopupColumn.anchors.leftMargin +
                              configureKeyMapPopupColumn.anchors.rightMargin +
                              ScrollBar.vertical.width

                height: configureKeyMapPopupItem.height -
                        anchors.topMargin -
                        configureKeyMapPopupTitle.height -
                        configureKeyMapPopupButtons.anchors.topMargin -
                        configureKeyMapPopupButtons.height

                ScrollBar.vertical.policy: ScrollBar.AsNeeded

                clip : true

                Column {
                    id: configureKeyMapPopupColumn
                    spacing: 10

                    anchors {
                        left: parent.left
                        leftMargin: 5
                        rightMargin: 10
                    }

                    width: 300

                    TextField {
                        width: parent.width
                        placeholderText: qsTr("Search for a key ...")

                        onTextEdited: keyboardConfiguratorController.setKeyCodeModelFilter(text)
                    }

                    ComboBox {
                        id: keyCodeModelFilterComboBox

                        width: 140

                        model: [
                            qsTr("All"),
                            qsTr("Keyboard Keys"),
                            qsTr("Combination Keys"),
                            qsTr("Multimedia"),
                            qsTr("Shortcut")
                        ]

                        onCurrentIndexChanged: keyboardConfiguratorController.keyCodesProxy.setCurrentFilter(currentIndex)
                    }

                    Row {
                        spacing: 10

                        Column {
                            spacing: 10

                            Label {
                                text: qsTr("Keys")
                                font.bold: true
                            }

                            ListView {
                                id: configureKeyMapKeyCodesListView

                                clip: true

                                height: 200
                                width: 150

                                model: keyboardConfiguratorController.keyCodesProxy

                                ScrollBar.vertical: ScrollBar {
                                    policy: ScrollBar.AsNeeded
                                }

                                delegate: Row {
                                    Button {
                                        flat: true

                                        width: configureKeyMapKeyCodesListView.width -
                                               configureKeyMapKeyCodesListView.ScrollBar.vertical.width - 5

                                        contentItem: Label {
                                            anchors {
                                                left: parent.left
                                                leftMargin: 10
                                                verticalCenter: parent.verticalCenter
                                            }

                                            text: model.name
                                        }

                                        onClicked: {
                                            keyboardConfiguratorController.unsavedChanges = true
                                            keyboardConfiguratorController.customMappedKeys.add(model.code)
                                        }
                                    }
                                }
                            }
                        }

                        Column {
                            spacing: 10

                            Label {
                                text: qsTr("Custom Keys")
                                font.bold: true
                            }

                            ListView {
                                id: configureKeyMapSelectedKeysListView

                                clip: true

                                height: 200
                                width: 150

                                model: keyboardConfiguratorController.customMappedKeys

                                ScrollBar.vertical: ScrollBar {
                                    policy: ScrollBar.AsNeeded
                                }

                                delegate: Row {

                                    Label {
                                        text: model.name

                                        width: configureKeyMapKeyCodesListView.width -
                                               configureKeyMapKeyCodesListView.ScrollBar.vertical.width - keyDeleteButton.implicitWidth

                                        elide: Text.ElideRight

                                        anchors.verticalCenter: parent.verticalCenter
                                    }

                                    RoundButton {
                                        id: keyDeleteButton

                                        flat: true
                                        Material.foreground: Material.Red
                                        icon.source: Icons.get(Icons.Delete)

                                        onClicked: {
                                            keyboardConfiguratorController.unsavedChanges = true
                                            keyboardConfiguratorController.customMappedKeys.remove(model.index)
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }

            Row {
                id: configureKeyMapPopupButtons

                anchors {
                    top: configureKeyMapPopupScrollView.bottom
                    topMargin: 10
                    right: parent.right
                }

                spacing: 10

                Button {
                    text: qsTr("Reset to default")
                    flat: true
                    highlighted: true

                    onClicked: {
                        keyboardConfiguratorController.unsavedChanges = true
                        keyboardConfiguratorController.customMappedKeys.clear()
                        configureKeyMapPopup.close()
                        kbCanvas.requestPaint()
                    }
                }

                Button {
                    text: qsTr("Close")
                    flat: true
                    highlighted: true

                    onClicked: {
                        configureKeyMapPopup.close()
                        kbCanvas.requestPaint()
                    }
                }
            }
        }
    }

    Popup {
        id: keyMapNotSupportedPopup

        anchors.centerIn: parent
        margins: 10
        modal: true

        width: Math.min(kbConfigurator.width - (margins * 2), 400)

        Column {
            spacing: 10

            width: parent.width

            Label {
                text: qsTr("Unsupported ... for now")
                font.bold: true
                font.pixelSize: 15
            }

            Label {
                width: parent.width - 20
                wrapMode: Text.Wrap
                text: qsTr("Your keyboard has Scroll Lock/Calculator/Left Shift Split key.
Since I do not own an RK Keyboard with any of these keys, I was not able to determine the codes for them.
You can create an issue on GitHub, and I can guide you through the steps to accquire them.");
            }

            Row {
                anchors.right: parent.right

                spacing: 10

                Button {
                    text: qsTr("Report Issue")
                    flat: true
                    highlighted: true

                    onClicked: settingsController.reportIssue()
                }

                Button {
                    text: qsTr("Close")
                    flat: true
                    highlighted: true

                    onClicked: keyMapNotSupportedPopup.close()
                }
            }
        }
    }


    Rectangle {
        id: modeMenu

        anchors {
            top: parent.top
            left: parent.left
            bottom: parent.bottom
        }

        width: 50

        color: (Material.theme == Material.Light) ? "#e6e6e6" : "#444444"


        Button {
            id: lightModeButton
            anchors.top: parent.top
            width: parent.width

            topInset: 0
            bottomInset: 0
            background.implicitHeight: 50

            flat: true

            display: AbstractButton.TextUnderIcon
            text: qsTr("LED")
            font.bold: false
            font.pixelSize: 12
            font.capitalization: Font.MixedCase

            icon.source: Icons.get(Icons.Light)

            onClicked: keyboardConfiguratorController.selectedMode = KeyboardConfiguratorController.Light

            Material.foreground: (keyboardConfiguratorController.selectedMode === KeyboardConfiguratorController.Light)
                   ? Material.accentColor : ((Material.theme == Material.Light) ? "#000000" : "#ffffff")

            ToolTip.visible: hovered
            ToolTip.text: qsTr("Configure Lights")
            ToolTip.delay: mainWindowController.toolTipDelay
            ToolTip.timeout: mainWindowController.toolTipTimeout
        }

        Button {
            id: keyRemapModeButton
            anchors.top: lightModeButton.bottom
            width: parent.width

            topInset: 0
            bottomInset: 0
            background.implicitHeight: 50

            flat: true

            display: AbstractButton.TextUnderIcon
            text: qsTr("Edit")
            font.bold: false
            font.pixelSize: 12
            font.capitalization: Font.MixedCase

            icon.source: Icons.get(Icons.Keyboard)

            onClicked: keyboardConfiguratorController.selectedMode = KeyboardConfiguratorController.KeyMap

            Material.foreground: (keyboardConfiguratorController.selectedMode === KeyboardConfiguratorController.KeyMap)
                   ? Material.accentColor : ((Material.theme == Material.Light) ? "#000000" : "#ffffff")

            ToolTip.visible: hovered
            ToolTip.text: qsTr("Configure Key Map")
            ToolTip.delay: mainWindowController.toolTipDelay
            ToolTip.timeout: mainWindowController.toolTipTimeout
        }

        Button {
            id: homeButton
            anchors.bottom: settingsButton.top
            width: parent.width

            topInset: 0
            bottomInset: 0
            background.implicitHeight: 50

            flat: true

            icon.source: Icons.get(Icons.Home)

            onClicked: {
                kbCanvas.unloadImageSource()
                kbConfigurator.returnToHome()
            }

            ToolTip.visible: hovered
            ToolTip.text: qsTr("Return to Home")
            ToolTip.delay: mainWindowController.toolTipDelay
            ToolTip.timeout: mainWindowController.toolTipTimeout
        }

        Button {
            id: settingsButton
            anchors.bottom: parent.bottom
            width: parent.width

            topInset: 0
            bottomInset: 0
            background.implicitHeight: 50

            flat: true

            icon.source: Icons.get(Icons.Settings)

            onClicked: kbConfigurator.openSettings()

            ToolTip.visible: hovered
            ToolTip.text: qsTr("Settings")
            ToolTip.delay: mainWindowController.toolTipDelay
            ToolTip.timeout: mainWindowController.toolTipTimeout
        }
    }

    Rectangle {
        id: lightModeMenu
        anchors {
            top: parent.top
            left: modeMenu.right
            bottom: parent.bottom
        }

        Behavior on width {
            SmoothedAnimation { duration: animationDuration }
        }

        clip: true

        color: (Material.theme == Material.Light) ? "#f0f0f0" : "#3a3a3a"

        width: (keyboardConfiguratorController.selectedMode === KeyboardConfiguratorController.Light) ?
                   implicitWidth : 0

        implicitWidth: 200

        ListView {
            id: lightModeMenuList

            anchors {
                fill: parent
            }

            model: keyboardConfiguratorController.modes

            ScrollBar.vertical: ScrollBar {
                policy: ScrollBar.AsNeeded
            }

            delegate: Button {
                id: lightModeMenuOption

                topInset: 0
                bottomInset: 0
                background.implicitHeight: 50

                flat: true

                width: lightModeMenu.width

                contentItem: Item {
                    Label {
                        text: model.name
                        color: (keyboardConfiguratorController.selectedLightModeIndex === model.index)
                               ? Material.accentColor : ((Material.theme == Material.Light) ? "#000000" : "#ffffff")
                        font.pixelSize: 14
                        anchors {
                            left: parent.left
                            leftMargin: 10
                            verticalCenter: parent.verticalCenter
                        }
                    }
                }

                onClicked: {
                    if (keyboardConfiguratorController.selectedLightModeIndex !== model.index)
                    {
                        keyboardConfiguratorController.unsavedChanges = true
                        keyboardConfiguratorController.selectedLightModeIndex = model.index
                    }
                }
            }
        }
    }

    Item {
        id: header

        anchors {
            top: parent.top
            left: lightModeMenu.right
            right: parent.right
        }

        height: 60

        Label {
            id: kbName

            anchors {
                left: parent.left
                leftMargin: 20
                verticalCenter: parent.verticalCenter
            }

            width: parent.width - anchors.leftMargin - headerButtons.width - headerButtons.anchors.rightMargin

            font.pixelSize: 20
            elide: Text.ElideRight
        }

        Row {
            id: headerButtons

            spacing: 10

            anchors {
                right: parent.right
                rightMargin: 20
                verticalCenter: parent.verticalCenter
            }

            RoundButton {
                id: resetButton

                flat: true
                icon.source: Icons.get(Icons.Reset)

                onClicked: keyboardConfiguratorController.reset()

                enabled: keyboardConfiguratorController.unsavedChanges

                ToolTip.visible: hovered
                ToolTip.text: qsTr("Discard unsaved settings")
                ToolTip.delay: mainWindowController.toolTipDelay
                ToolTip.timeout: mainWindowController.toolTipTimeout
            }

            RoundButton {
                id: saveButton

                flat: ! keyboardConfiguratorController.unsavedChanges

                highlighted: keyboardConfiguratorController.unsavedChanges
                enabled: ! keyboardConfiguratorController.savingChanges

                icon.source: Icons.get(Icons.Save)

                onClicked: keyboardConfiguratorController.save()

                ToolTip.visible: hovered
                ToolTip.text: qsTr("Save and apply changes to keyboard")
                ToolTip.delay: mainWindowController.toolTipDelay
                ToolTip.timeout: mainWindowController.toolTipTimeout
            }
        }
    }

    ScrollView {
        id: sv
        anchors {
            top: header.bottom
            left: lightModeMenu.right
            right: parent.right
            bottom: parent.bottom
        }


        Item {
            id: svItem

            implicitHeight: selectedProfileComboBoxContainer.implicitHeight +
                            (kbCanvas.height * kbCanvasScale.yScale) +
                            (keyboardConfiguratorController.selectedMode === KeyboardConfiguratorController.Light
                            ? lightModeControls.implicitHeight  : keyMapModeControls.implicitHeight)
                            + (margin * 2)

            readonly property int margin: 20

            implicitWidth: lightModeControls.implicitWidth + (margin * 2)

            width: Math.max(sv.width, implicitWidth)
            height: Math.max(sv.height, implicitHeight)

            Column {
                id: selectedProfileComboBoxContainer

                anchors {
                    top: parent.top
                    left: parent.left
                    leftMargin: 20
                }

                Label {
                    text: qsTr("Current profile")
                }

                Row {
                    ComboBox {
                        anchors.verticalCenter: parent.verticalCenter

                        width: 150

                        model: keyboardConfiguratorController.profiles
                        currentIndex : keyboardConfiguratorController.selectedProfileIndex
                        onCurrentIndexChanged: {
                            keyboardConfiguratorController.selectedProfileIndex = currentIndex
                            ToolTip.text = keyboardConfiguratorController.profiles.profileName(currentIndex)
                        }

                        textRole: "name"

                        ToolTip.visible: hovered
                        ToolTip.delay: mainWindowController.toolTipDelay
                        ToolTip.timeout: mainWindowController.toolTipTimeout
                    }

                    RoundButton {
                        anchors.verticalCenter: parent.verticalCenter

                        flat:true
                        icon.source: Icons.get(Icons.Edit)

                        onClicked: editProfilesPopup.open()

                        ToolTip.visible: hovered
                        ToolTip.text: qsTr("Edit Profiles")
                        ToolTip.delay: mainWindowController.toolTipDelay
                        ToolTip.timeout: mainWindowController.toolTipTimeout
                    }
                }
            }

            Canvas {
                id: kbCanvas

                anchors {
                    top: selectedProfileComboBoxContainer.bottom
                    topMargin: Math.max((svItem.height - kbCanvas.height - lightModeControlsRightColumn.implicitHeight) / 4, 0)
                    leftMargin: Math.abs((svItem.width - (kbCanvas.width * kbCanvasScale.xScale)) / 2)
                    left: parent.left
                }

                MouseArea {
                    anchors.fill: parent

                    enabled: keyboardConfiguratorController.isCustomLightModeSelected ||
                            (keyboardConfiguratorController.selectedMode === KeyboardConfiguratorController.KeyMap)

                    onClicked: (mouse) => {
                        var l = mapToItem(kbCanvas,
                                          mouse.x,
                                          mouse.y)

                        keyboardConfiguratorController.clickKey(l.x, l.y)
                    }
                }

                Timer {
                    id: requestPaintWithDelay
                    repeat: false
                    running: false
                    onTriggered: parent.requestPaint()
                    interval: 5
                }

                readonly property real minScale: 0.6
                property string imageSource: ""

                function loadImageSource(source)
                {
                    imageSource = source
                    loadImage(imageSource)
                }

                function unloadImageSource()
                {
                    unloadImage(imageSource)
                }

                transform : Scale {
                    id: kbCanvasScale
                    xScale: Math.max(Math.min(sv.width - svItem.margin , kbCanvas.width) / kbCanvas.width, kbCanvas.minScale)
                    yScale: xScale
                }

                function loadImgDetails()
                {
                    var imgData = getContext("2d").createImageData(imageSource)

                    width = imgData.width
                    height = imgData.height
                }

                height: 1
                width: 1

                onImageLoaded: requestPaint()


                onPaint: {

                    if (height === 1) {
                        loadImgDetails()
                        requestPaintWithDelay.running = true
                        return
                    }

                    var ctx = getContext("2d");

                    ctx.save()

                    ctx.clearRect(0, 0, width, height)

                    if (keyboardConfiguratorController.isCustomLightModeSelected ||
                            keyboardConfiguratorController.selectedMode === KeyboardConfiguratorController.KeyMap)
                    {
                        ctx.fillStyle = "#949292"
                    }
                    else if (keyboardConfiguratorController.randomColours)
                    {
                        var gradient = ctx.createLinearGradient(keyboardConfiguratorController.topLeftX,
                                                                keyboardConfiguratorController.topLeftY,
                                                                keyboardConfiguratorController.bottomRightX,
                                                                keyboardConfiguratorController.bottomRightY)
                        gradient.addColorStop(0, "red")
                        gradient.addColorStop(0.5, "green")
                        gradient.addColorStop(1, "blue")
                        ctx.fillStyle = gradient
                    }
                    else
                    {
                        ctx.fillStyle = keyboardConfiguratorController.selectedColour
                    }

                    ctx.fillRect(keyboardConfiguratorController.topLeftX,
                                 keyboardConfiguratorController.topLeftY,
                                 (keyboardConfiguratorController.bottomRightX  - keyboardConfiguratorController.topLeftX),
                                 (keyboardConfiguratorController.bottomRightY  - keyboardConfiguratorController.topLeftY))

                    var i;

                    if (keyboardConfiguratorController.isCustomLightModeSelected &&
                            keyboardConfiguratorController.selectedMode === KeyboardConfiguratorController.Light)
                    {
                        var acSize = keyboardConfiguratorController.customLightModeKeys.rowCount()
                        var key;

                        for (i = 0; i < acSize; i++)
                        {
                            key = keyboardConfiguratorController.getKeyByCustomLightModeKeysIndex(i)
                            ctx.fillStyle = keyboardConfiguratorController.customLightModeKeys.getColour(i)
                            ctx.fillRect(key.topX,
                                         key.topY,
                                         (key.bottomX - key.topX),
                                         (key.bottomY - key.topY))
                        }
                    }
                    else if(keyboardConfiguratorController.selectedMode === KeyboardConfiguratorController.KeyMap)
                    {
                        var kmSize = keyboardConfiguratorController.customMappedKeys.keysCount()
                        var kmKey;

                        for (i = 0; i < kmSize; i++)
                        {
                            kmKey = keyboardConfiguratorController.getKeyByCustomMappedKeysIndex(i)
                            ctx.fillStyle = Material.accent
                            ctx.fillRect(kmKey.topX,
                                         kmKey.topY,
                                         (kmKey.bottomX - kmKey.topX),
                                         (kmKey.bottomY - kmKey.topY))
                        }
                    }

                    ctx.drawImage(imageSource, 0, 0, width, height)

                    if (keyboardConfiguratorController.isCustomLightModeSelected &&
                            keyboardConfiguratorController.selectedMode === KeyboardConfiguratorController.Light)
                    {
                        for (i = 0; i < acSize; i++)
                        {
                            key = keyboardConfiguratorController.getKeyByCustomLightModeKeysIndex(i)
                            ctx.strokeStyle = "#fc9d03"
                            ctx.strokeRect(key.topX,
                                           key.topY,
                                           (key.bottomX - key.topX),
                                           (key.bottomY - key.topY))
                        }
                    }
                    else if(keyboardConfiguratorController.selectedMode === KeyboardConfiguratorController.KeyMap)
                    {
                        for (i = 0; i < kmSize; i++)
                        {
                            kmKey = keyboardConfiguratorController.getKeyByCustomMappedKeysIndex(i)
                            ctx.strokeStyle = "#fc9d03"
                            ctx.strokeRect(kmKey.topX,
                                           kmKey.topY,
                                           (kmKey.bottomX - kmKey.topX),
                                           (kmKey.bottomY - kmKey.topY))
                        }
                    }

                    ctx.restore()
                }

            }

            Item {
                id: lightModeControls

                implicitHeight: lightModeControlsRightColumn.implicitHeight
                implicitWidth: lightModeControlsLeftColumn.implicitWidth + lightModeControlsRightColumn.implicitWidth

                visible: keyboardConfiguratorController.selectedMode === KeyboardConfiguratorController.Light

                anchors {
                    top: kbCanvas.bottom
                    topMargin: ((kbCanvas.height * kbCanvasScale.yScale) - kbCanvas.height )
                               + kbCanvas.anchors.topMargin
                    bottom: parent.bottom
                    left: parent.left
                    right: parent.right
                }

                Item {
                    anchors {
                        top: parent.top
                        bottom: parent.bottom
                        left: parent.left
                        right: keyboardConfiguratorController.showColoursColumn
                               ? parent.horizontalCenter : parent.right
                    }

                    Column {
                        id: lightModeControlsLeftColumn

                        anchors.centerIn: parent

                        Row {
                            anchors.horizontalCenter: parent.horizontalCenter

                            spacing: 10

                            visible: keyboardConfiguratorController.isCustomLightModeSelected

                            Button {
                                text: qsTr("Select all")

                                onClicked: keyboardConfiguratorController.selectAllKeysForCustomMode()
                            }

                            Button {
                                text: qsTr("Unselect all")

                                onClicked: keyboardConfiguratorController.unselectAllKeysForCustomMode()
                            }
                        }

                        Grid {

                            rows: 3
                            columns: 3


                            verticalItemAlignment: Grid.AlignVCenter
                            horizontalItemAlignment: Grid.AlignHCenter

                            columnSpacing: 10

                            Label {
                                id: test1
                                text: qsTr("Brightness")
                            }

                            Slider {
                                id: brightnessSlider
                                from : 0
                                to: 5
                                stepSize: 1.0
                                snapMode: Slider.SnapAlways

                                width: 100

                                property var texts : [qsTr("Off"), qsTr("Very Low"), qsTr("Low"),
                                    qsTr("Medium"), qsTr("High"), qsTr("Full")]

                                value: keyboardConfiguratorController.brightness

                                onValueChanged: brightnessSliderText.text = texts[value]

                                onMoved: {
                                    keyboardConfiguratorController.brightness = value
                                    keyboardConfiguratorController.unsavedChanges = true
                                }
                            }

                            Label {
                                id: brightnessSliderText
                                elide: Text.ElideRight
                                width: 80
                            }

                            Label {
                                text: qsTr("Animation")

                                visible: ! keyboardConfiguratorController.isCustomLightModeSelected
                            }

                            Slider {
                                id: animationSlider
                                from : 1
                                to: 5
                                stepSize: 1.0
                                snapMode: Slider.SnapAlways

                                visible: ! keyboardConfiguratorController.isCustomLightModeSelected

                                width: 100

                                property var texts : [qsTr("Very Slow"), qsTr("Slow"), qsTr("Normal"),
                                    qsTr("Fast"), qsTr("Very Fast")]

                                value: keyboardConfiguratorController.animation

                                onValueChanged: animationSliderText.text = texts[value - 1]

                                onMoved: {
                                    keyboardConfiguratorController.animation = value
                                    keyboardConfiguratorController.unsavedChanges = true
                                }
                            }


                            Label {
                                id: animationSliderText
                                elide: Text.ElideRight
                                width: 80

                                visible: ! keyboardConfiguratorController.isCustomLightModeSelected
                            }

                            Label {
                                text: qsTr("Sleep")
                            }

                            Slider {
                                id: sleepSlider
                                from : 1
                                to: 5
                                stepSize: 1.0
                                snapMode: Slider.SnapAlways

                                width: 100

                                property var texts : [qsTr("5 min"), qsTr("10 min"), qsTr("20 min"),
                                    qsTr("30 min"), qsTr("Off")]

                                value: keyboardConfiguratorController.sleep

                                onValueChanged: sleepSliderText.text = texts[value - 1]

                                onMoved: {
                                    keyboardConfiguratorController.sleep = value
                                    keyboardConfiguratorController.unsavedChanges = true
                                }
                            }

                            Label {
                                id: sleepSliderText
                                elide: Text.ElideRight
                                width: 80
                            }
                        }


                    }
                }

                Item {
                    anchors {
                        top: parent.top
                        bottom: parent.bottom
                        left: parent.horizontalCenter
                        right: parent.right
                    }

                    Column {
                        id: lightModeControlsRightColumn

                        anchors.centerIn: parent

                        spacing: 10

                        visible: keyboardConfiguratorController.showColoursColumn

                        CheckBox {
                            id: cm
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: qsTr("Random Colours")

                            visible: ! keyboardConfiguratorController.isCustomLightModeSelected

                            checked: keyboardConfiguratorController.randomColours

                            onToggled: {
                                keyboardConfiguratorController.randomColours = checked
                                keyboardConfiguratorController.unsavedChanges = true
                            }
                        }

                        Label {
                            text: qsTr("Custom Colours:")
                        }

                        Row {
                            spacing: 5

                            Repeater {
                                model: keyboardConfiguratorController.customColours

                                delegate: Rectangle {
                                    width: 30
                                    height: 30

                                    radius: 3

                                    border.width: 1
                                    border.color: "#979797"

                                    color: model.colour

                                    MouseArea {
                                        anchors.fill: parent

                                        acceptedButtons: Qt.LeftButton | Qt.RightButton

                                        onClicked: customColourOptions.openIfNotVisible()

                                        hoverEnabled: true
                                        ToolTip.visible: containsMouse
                                        ToolTip.text: qsTr("Click to open menu, Double click to select colour")
                                        ToolTip.delay: mainWindowController.toolTipDelay
                                        ToolTip.timeout: mainWindowController.toolTipTimeout
                                    }


                                    Menu {
                                        id: customColourOptions

                                        width: 130

                                        function openIfNotVisible()
                                        {
                                            if (!customColourOptions.visible)
                                            {
                                                customColourOptions.open()
                                            }
                                        }

                                        Action {
                                            text: qsTr("Select")
                                            icon.source: Icons.get(Icons.Done)
                                            onTriggered: {
                                                keyboardConfiguratorController.unsavedChanges = true
                                                keyboardConfiguratorController.selectedColour = model.colour // keyboardConfiguratorController.customColours[model.index]
                                                keyboardConfiguratorController.randomColours = false
                                            }
                                        }

                                        Action {
                                            text: qsTr("Edit")
                                            icon.source: Icons.get(Icons.Edit)
                                            onTriggered: colorDialog.show(model)
                                        }
                                    }
                                }
                            }
                        }

                        Row {

                            spacing: 10

                            Label {
                                text: qsTr("Current Colour:")
                                anchors.verticalCenter: parent.verticalCenter
                            }

                            Rectangle {
                                width: 100
                                height: 30

                                radius: 3

                                color: keyboardConfiguratorController.selectedColour

                                border.width: 1
                                border.color: "#979797"
                            }
                        }

                        Button {
                            id: pickColourButton

                            text: qsTr("Pick Colour")

                            onClicked: colorDialog.showAlone()
                        }

                        ColorDialog {
                            id : colorDialog

                            property var model: null

                            function showAlone()
                            {
                                show(null)
                            }

                            function show(m)
                            {
                                model = m

                                if (model !== null)
                                {
                                    selectedColor = model.colour
                                }

                                open()
                            }

                            options: ColorDialog.DontUseNativeDialog

                            onAccepted: {
                                if (model === null)
                                {
                                    keyboardConfiguratorController.selectedColour = selectedColor
                                    keyboardConfiguratorController.randomColours = false
                                }
                                else
                                {
                                    model.colour = selectedColor
                                }

                                keyboardConfiguratorController.unsavedChanges = true

                                model = null
                            }
                        }
                    }
                }
            }


            Item {
                id: keyMapModeControls

                visible: keyboardConfiguratorController.selectedMode === KeyboardConfiguratorController.KeyMap

                implicitWidth: keyboardConfiguratorController.keyMapEnabled
                               ? keyMapModeControlsCol.implicitWidth : keyMapNotSupportedLabel.implicitWidth

                implicitHeight: keyboardConfiguratorController.keyMapEnabled
                                ? keyMapModeControlsCol.implicitHeight : keyMapNotSupportedLabel.implicitHeight

                anchors {
                    top: kbCanvas.bottom
                    topMargin: ((kbCanvas.height * kbCanvasScale.yScale) - kbCanvas.height )
                               + kbCanvas.anchors.topMargin
                    bottom: parent.bottom
                    left: parent.left
                    right: parent.right
                }

                Column {
                    id: keyMapModeControlsCol

                    visible: keyboardConfiguratorController.keyMapEnabled

                    anchors.centerIn: parent
                    spacing: 20

                    Label {
                        id: keyMapClickKeyToEditPrompt

                        anchors.horizontalCenter: parent.horizontalCenter

                        text: qsTr("Click a key to edit")
                    }

                    Button {
                        text: qsTr("Reset to default")

                        onClicked: {
                            keyboardConfiguratorController.unsavedChanges = true
                            keyboardConfiguratorController.customMappedKeys.clearAll()
                            kbCanvas.requestPaint()
                        }
                    }
                }

                Row {
                    id: keyMapNotSupportedLabel

                    anchors.centerIn: parent

                    visible: !keyboardConfiguratorController.keyMapEnabled

                    Label {
                        text: qsTr("Custom Key Maps are not supported on your keyboard yet. ")
                    }

                    Hyperlink {
                        text: "Learn more"
                        mouseArea.onClicked: keyMapNotSupportedPopup.open()
                    }
                }
            }
        }
    }
}
