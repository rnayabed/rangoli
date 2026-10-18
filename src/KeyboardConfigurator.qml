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
import QtQuick.Layouts // QTBUG-111187
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

        function onMacroEditorChanged()
        {
            keyMapMacroComboBox.refresh()
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
            profileNameTextField.text = name
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

                EnhancedButton {
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

                EnhancedButton {
                    text: qsTr("Close")
                    flat: true
                    highlighted: true

                    onClicked: profileNameEditorPopup.close()
                }
            }
        }
    }

    Popup {
        id: customEffectNamePopup

        anchors.centerIn: parent
        margins: 10
        modal: true

        // -1 saves the current keys as a new effect, otherwise renames that row
        property int index: -1

        function nextDefaultName()
        {
            var names = []
            for (var i = 0; i < keyboardConfiguratorController.customEffects.rowCount(); i++)
            {
                names.push(keyboardConfiguratorController.customEffects.effectName(i))
            }

            var n = keyboardConfiguratorController.customEffects.rowCount() + 1
            while (names.indexOf(qsTr("Custom Effect #%1").arg(n)) !== -1) n++

            return qsTr("Custom Effect #%1").arg(n)
        }

        function showNew()
        {
            index = -1
            customEffectNameTextField.text = nextDefaultName()
            open()
            customEffectNameTextField.selectAll()
            customEffectNameTextField.forceActiveFocus()
        }

        function showRename(row, name)
        {
            index = row
            customEffectNameTextField.text = name
            open()
            customEffectNameTextField.selectAll()
            customEffectNameTextField.forceActiveFocus()
        }

        function accept()
        {
            if (customEffectNameTextField.text.length === 0) return

            close()

            if (index === -1)
            {
                keyboardConfiguratorController.saveAsNewCustomEffect(customEffectNameTextField.text)
            }
            else
            {
                keyboardConfiguratorController.setCustomEffectName(index, customEffectNameTextField.text)
            }
        }

        Column {
            anchors.fill: parent

            spacing: 10

            Label {
                text: (customEffectNamePopup.index === -1) ? qsTr("Save as New Effect") : qsTr("Rename Effect")
                font.bold: true
                font.pixelSize: 15
                bottomPadding: 10
            }

            Label {
                text: qsTr("Name");
            }

            TextField {
                id: customEffectNameTextField

                width: 250

                onAccepted: customEffectNamePopup.accept()
            }

            Row {
                anchors {
                    right: parent.right
                }

                spacing: 10

                EnhancedButton {
                    text: qsTr("Save")
                    flat: true
                    highlighted: true

                    enabled: customEffectNameTextField.text.length > 0

                    onClicked: customEffectNamePopup.accept()
                }

                EnhancedButton {
                    text: qsTr("Cancel")
                    flat: true
                    highlighted: true

                    onClicked: customEffectNamePopup.close()
                }
            }
        }
    }

    Popup {
        id: discardCustomEffectChangesPopup

        anchors.centerIn: parent
        margins: 10
        modal: true

        property var pendingAction: null

        // Runs action straight away, or asks first if the selected effect has unsaved edits
        function confirm(action)
        {
            if (!keyboardConfiguratorController.customEffectModified)
            {
                action()
                return
            }

            pendingAction = action
            open()
        }

        function finish()
        {
            close()
            var action = pendingAction
            pendingAction = null
            action()
        }

        Column {
            anchors.fill: parent

            spacing: 10

            Label {
                text: qsTr("Unsaved Effect Changes")
                font.bold: true
                font.pixelSize: 15
                bottomPadding: 10
            }

            Label {
                text: qsTr("\"%1\" has changes that haven't been saved.")
                        .arg(keyboardConfiguratorController.customEffects.effectName(
                                 keyboardConfiguratorController.selectedCustomEffectIndex))
            }

            Row {
                anchors {
                    right: parent.right
                }

                spacing: 10

                EnhancedButton {
                    text: qsTr("Save effect")
                    flat: true
                    highlighted: true

                    onClicked: {
                        keyboardConfiguratorController.saveSelectedCustomEffect()
                        discardCustomEffectChangesPopup.finish()
                    }
                }

                EnhancedButton {
                    text: qsTr("Discard")
                    flat: true
                    highlighted: true
                    Material.accent: Material.Red

                    onClicked: discardCustomEffectChangesPopup.finish()
                }

                EnhancedButton {
                    text: qsTr("Cancel")
                    flat: true
                    highlighted: true

                    onClicked: {
                        discardCustomEffectChangesPopup.pendingAction = null
                        discardCustomEffectChangesPopup.close()
                    }
                }
            }
        }
    }

    Popup {
        id: deleteCustomEffectPopup

        anchors.centerIn: parent
        margins: 10
        modal: true

        property int index: -1
        property string name: ""

        function show(row, effectName)
        {
            index = row
            name = effectName
            open()
        }

        Column {
            anchors.fill: parent

            spacing: 10

            Label {
                text: qsTr("Delete Effect")
                font.bold: true
                font.pixelSize: 15
                bottomPadding: 10
            }

            Label {
                text: qsTr("Delete \"%1\"? This cannot be undone.").arg(deleteCustomEffectPopup.name)
            }

            Row {
                anchors {
                    right: parent.right
                }

                spacing: 10

                EnhancedButton {
                    text: qsTr("Delete")
                    flat: true
                    highlighted: true
                    Material.accent: Material.Red

                    onClicked: {
                        deleteCustomEffectPopup.close()
                        keyboardConfiguratorController.deleteCustomEffect(deleteCustomEffectPopup.index)
                    }
                }

                EnhancedButton {
                    text: qsTr("Cancel")
                    flat: true
                    highlighted: true

                    onClicked: deleteCustomEffectPopup.close()
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

            EnhancedButton {
                anchors.right: parent.right

                text: qsTr("Close")
                flat: true
                highlighted: true

                onClicked: editProfilesPopup.close()
            }
        }
    }

    Popup {
        id: macroEditorPopup

        anchors.centerIn: parent
        margins: 10
        modal: true

        width: Math.min(820, kbConfigurator.width - 40)
        height: Math.min(640, kbConfigurator.height - 40)

        // Escape and clicks outside are keys to record while recording
        closePolicy: macroEditor.recording ? Popup.NoAutoClose : (Popup.CloseOnEscape | Popup.CloseOnPressOutside)

        onOpened: {
            if (keyboardConfiguratorController.editingMacroSlot === -1)
            {
                keyboardConfiguratorController.editMacro(0)
            }
        }

        onClosed: macroEditor.recording = false

        Item {
            id: macroEditor

            anchors.fill: parent

            property bool recording: false
            property real lastEventTime: 0

            // Slot to switch to once unsaved changes are dealt with
            property int pendingSlot: -1

            onRecordingChanged: lastEventTime = 0

            function switchTo(slot)
            {
                if (slot === keyboardConfiguratorController.editingMacroSlot) return

                if (keyboardConfiguratorController.editingMacroModified)
                {
                    pendingSlot = slot
                    discardMacroChangesPopup.open()
                    return
                }

                recording = false
                keyboardConfiguratorController.editMacro(slot)
            }

            function record(event, release)
            {
                event.accepted = true
                if (event.isAutoRepeat) return

                var now = Date.now()
                var gap = (macroKeepTiming.checked && lastEventTime > 0) ? now - lastEventTime : 0

                var ok = keyboardConfiguratorController.recordMacroKey(event.nativeScanCode, release,
                                                                       gap, macroDelaySpinBox.value)
                if (ok)
                {
                    lastEventTime = now
                    macroWarning.text = ""
                    macroEventsList.positionViewAtEnd()
                }
                else
                {
                    macroWarning.text = keyboardConfiguratorController.editingMacroEvents.length >= 64
                            ? qsTr("The macro is full (64 key presses and releases)")
                            : qsTr("That key can't be recorded (Fn and some special keys can't)")
                }
            }

            // Takes the focus while recording so presses on the real keyboard become events
            Item {
                id: macroRecorder

                width: 0
                height: 0

                focus: macroEditor.recording

                Keys.onPressed: (event) => macroEditor.record(event, false)
                Keys.onReleased: (event) => macroEditor.record(event, true)

                onActiveFocusChanged: if (!activeFocus) macroEditor.recording = false
            }

            Label {
                id: macroEditorTitle

                anchors {
                    top: parent.top
                    left: parent.left
                }

                text: qsTr("Macros")
                font.bold: true
                font.pixelSize: 15
            }

            ListView {
                id: macroSlotsList

                anchors {
                    top: macroEditorTitle.bottom
                    topMargin: 10
                    left: parent.left
                    bottom: macroEditorHint.top
                    bottomMargin: 10
                }

                width: 200
                clip: true

                model: keyboardConfiguratorController.macros

                delegate: ItemDelegate {
                    width: macroSlotsList.width

                    highlighted: index === keyboardConfiguratorController.editingMacroSlot

                    text: (model.empty ? qsTr("%1. %2 (empty)") : qsTr("%1. %2"))
                              .arg(index + 1).arg(model.name)
                          + ((highlighted && keyboardConfiguratorController.editingMacroModified) ? " *" : "")

                    opacity: model.empty && !highlighted ? 0.6 : 1

                    focusPolicy: Qt.NoFocus

                    onClicked: macroEditor.switchTo(index)
                }
            }

            Column {
                id: macroEditColumn

                anchors {
                    top: macroEditorTitle.bottom
                    topMargin: 10
                    left: macroSlotsList.right
                    leftMargin: 20
                    right: parent.right
                }

                spacing: 8

                Row {
                    spacing: 10

                    TextField {
                        width: 220

                        placeholderText: qsTr("Macro %1").arg(keyboardConfiguratorController.editingMacroSlot + 1)
                        text: keyboardConfiguratorController.editingMacroName

                        enabled: !macroEditor.recording

                        onTextEdited: keyboardConfiguratorController.editingMacroName = text
                    }

                    Label {
                        anchors.verticalCenter: parent.verticalCenter

                        text: qsTr("%1 / 64 events").arg(keyboardConfiguratorController.editingMacroEvents.length)
                        opacity: 0.7
                    }
                }

                Row {
                    spacing: 10

                    EnhancedButton {
                        text: macroEditor.recording ? qsTr("Done") : qsTr("Record")
                        highlighted: macroEditor.recording

                        // Taking the focus would stop the recording before the click restarts it
                        focusPolicy: Qt.NoFocus

                        onClicked: {
                            macroEditor.recording = !macroEditor.recording
                            macroWarning.text = ""

                            if (macroEditor.recording) macroRecorder.forceActiveFocus()
                        }

                        ToolTip.visible: hovered
                        ToolTip.text: qsTr("Press keys on your keyboard to add them, including Shift, Ctrl, Alt and Tab. Every press and release is recorded.")
                        ToolTip.delay: mainWindowController.toolTipDelay
                        ToolTip.timeout: mainWindowController.toolTipTimeout
                    }

                    CheckBox {
                        id: macroKeepTiming

                        anchors.verticalCenter: parent.verticalCenter

                        text: qsTr("Keep timing")
                        focusPolicy: Qt.NoFocus

                        ToolTip.visible: hovered
                        ToolTip.text: qsTr("Record how long you actually waited between keys. Otherwise every key waits the delay set here.")
                        ToolTip.delay: mainWindowController.toolTipDelay
                        ToolTip.timeout: mainWindowController.toolTipTimeout
                    }

                    Label {
                        anchors.verticalCenter: parent.verticalCenter
                        text: qsTr("Delay")
                    }

                    SpinBox {
                        id: macroDelaySpinBox

                        from: 0
                        to: 14000
                        stepSize: 10
                        value: 20
                        editable: true

                        focusPolicy: Qt.ClickFocus

                        textFromValue: (value) => qsTr("%1 ms").arg(value)
                        valueFromText: (text) => parseInt(text) || 0
                    }
                }

                Row {
                    spacing: 10

                    TextField {
                        id: macroTextField

                        width: 260
                        placeholderText: qsTr("Type text, e.g. Hello!{Tab}{Enter}")

                        enabled: !macroEditor.recording

                        function add()
                        {
                            if (text.length === 0) return

                            var unknown = keyboardConfiguratorController.addMacroText(text, macroDelaySpinBox.value)

                            macroWarning.text = unknown.length === 0 ? ""
                                    : qsTr("Can't type: %1").arg(unknown)
                            text = ""
                            macroEventsList.positionViewAtEnd()
                        }

                        onAccepted: add()

                        ToolTip.visible: hovered
                        ToolTip.text: qsTr("Capitals and symbols add Shift by themselves. Put other keys in braces, like {Tab}, {Enter}, {Esc}, {Backspace}, {F5} or {Up}.")
                        ToolTip.delay: mainWindowController.toolTipDelay
                        ToolTip.timeout: mainWindowController.toolTipTimeout
                    }

                    EnhancedButton {
                        anchors.verticalCenter: parent.verticalCenter

                        text: qsTr("Add")
                        enabled: macroTextField.text.length > 0 && !macroEditor.recording

                        onClicked: macroTextField.add()
                    }
                }

                Label {
                    visible: macroEditor.recording

                    text: qsTr("Recording, press keys on your keyboard. Click Done when finished.")
                    color: Material.accentColor
                }

                Label {
                    id: macroWarning

                    visible: text.length > 0
                    color: Material.color(Material.Red)
                    font.pixelSize: 12
                }
            }

            ListView {
                id: macroEventsList

                anchors {
                    top: macroEditColumn.bottom
                    topMargin: 8
                    left: macroEditColumn.left
                    right: parent.right
                    bottom: macroButtons.top
                    bottomMargin: 8
                }

                clip: true

                // A count rather than the list, so editing a delay keeps the scroll position
                model: keyboardConfiguratorController.editingMacroEvents.length

                ScrollBar.vertical: ScrollBar {
                    policy: ScrollBar.AsNeeded
                }

                delegate: Row {
                    id: macroEventRow

                    spacing: 10

                    readonly property var event: keyboardConfiguratorController.editingMacroEvents[index]
                    readonly property bool isLast: index === macroEventsList.count - 1

                    Label {
                        anchors.verticalCenter: parent.verticalCenter

                        width: 30
                        horizontalAlignment: Text.AlignRight
                        text: index + 1
                        opacity: 0.6
                    }

                    Label {
                        anchors.verticalCenter: parent.verticalCenter

                        width: 200
                        elide: Text.ElideRight
                        text: macroEventRow.event ? macroEventRow.event.text : ""
                    }

                    SpinBox {
                        visible: !macroEventRow.isLast

                        from: 0
                        to: 14000
                        stepSize: 10
                        editable: true

                        value: macroEventRow.event ? macroEventRow.event.delay : 0

                        textFromValue: (value) => qsTr("then wait %1 ms").arg(value)
                        valueFromText: (text) => parseInt(text.replace(/[^0-9]/g, "")) || 0

                        onValueModified: keyboardConfiguratorController.setMacroEventDelay(index, value)
                    }

                    Label {
                        anchors.verticalCenter: parent.verticalCenter

                        visible: macroEventRow.isLast
                        width: 140
                        text: qsTr("end")
                        opacity: 0.6
                    }

                    RoundButton {
                        flat: true
                        Material.foreground: Material.Red
                        icon.source: Icons.get(Icons.Delete)

                        enabled: !macroEditor.recording
                        focusPolicy: Qt.NoFocus

                        onClicked: keyboardConfiguratorController.removeMacroEvent(index)
                    }
                }
            }

            Row {
                id: macroButtons

                anchors {
                    bottom: macroEditorHint.top
                    bottomMargin: 10
                    left: macroEditColumn.left
                }

                spacing: 10

                EnhancedButton {
                    text: qsTr("Set all delays")
                    enabled: keyboardConfiguratorController.editingMacroEvents.length > 1 && !macroEditor.recording

                    onClicked: keyboardConfiguratorController.setAllMacroDelays(macroDelaySpinBox.value)

                    ToolTip.visible: hovered
                    ToolTip.text: qsTr("Give every key the delay set above")
                    ToolTip.delay: mainWindowController.toolTipDelay
                    ToolTip.timeout: mainWindowController.toolTipTimeout
                }

                EnhancedButton {
                    text: qsTr("Clear")
                    enabled: keyboardConfiguratorController.editingMacroEvents.length > 0 && !macroEditor.recording

                    onClicked: keyboardConfiguratorController.clearMacroEvents()
                }

                EnhancedButton {
                    text: qsTr("Save macro")
                    highlighted: keyboardConfiguratorController.editingMacroModified
                    enabled: keyboardConfiguratorController.editingMacroModified && !macroEditor.recording

                    onClicked: keyboardConfiguratorController.saveEditingMacro()
                }

                EnhancedButton {
                    text: qsTr("Close")
                    flat: true
                    highlighted: true

                    onClicked: macroEditorPopup.close()
                }
            }

            Label {
                id: macroEditorHint

                anchors {
                    bottom: parent.bottom
                    left: parent.left
                    right: parent.right
                }

                wrapMode: Text.WordWrap
                font.pixelSize: 12
                opacity: 0.7

                text: qsTr("Macros are stored on the keyboard and work on any computer. Saved macros are sent with Apply to keyboard. To put one on a key, click the key in Edit mode and pick it under Play macro.")
            }
        }
    }

    Popup {
        id: discardMacroChangesPopup

        anchors.centerIn: parent
        margins: 10
        modal: true

        Column {
            spacing: 10

            Label {
                text: qsTr("Discard changes to this macro?")
                font.bold: true
                font.pixelSize: 15
            }

            Row {
                anchors.right: parent.right
                spacing: 10

                EnhancedButton {
                    text: qsTr("Cancel")
                    flat: true
                    highlighted: true

                    onClicked: discardMacroChangesPopup.close()
                }

                EnhancedButton {
                    text: qsTr("Discard")
                    flat: true
                    highlighted: true

                    onClicked: {
                        discardMacroChangesPopup.close()
                        macroEditor.recording = false
                        keyboardConfiguratorController.editMacro(macroEditor.pendingSlot)
                    }
                }

                EnhancedButton {
                    text: qsTr("Save")
                    flat: true
                    highlighted: true

                    onClicked: {
                        discardMacroChangesPopup.close()
                        macroEditor.recording = false
                        keyboardConfiguratorController.saveEditingMacro()
                        keyboardConfiguratorController.editMacro(macroEditor.pendingSlot)
                    }
                }
            }
        }
    }

    Popup {
        id: configureKeyMapPopup

        anchors.centerIn: parent
        margins: 10
        modal: true

        onOpened: keyMapMacroComboBox.refresh()


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

                        model: ListModel {
                            ListElement { text: qsTr("All") }
                            ListElement { text: qsTr("Keyboard Keys") }
                            ListElement { text: qsTr("Combination Keys") }
                            ListElement { text: qsTr("Multimedia") }

                            Component.onCompleted: {
                                if (Qt.platform.os !== "osx")
                                {
                                    append({ text: qsTr("Shortcut") })
                                }
                            }
                        }


                        onCurrentIndexChanged: keyboardConfiguratorController.keyCodesProxy.setCurrentFilter(currentIndex)
                    }

                    // A key can play one of the macros stored on the keyboard instead
                    Row {
                        spacing: 10
                        visible: keyboardConfiguratorController.macrosSupported

                        Label {
                            anchors.verticalCenter: parent.verticalCenter
                            text: qsTr("Play macro")
                        }

                        ComboBox {
                            id: keyMapMacroComboBox

                            width: 200

                            function refresh()
                            {
                                var choices = [qsTr("None")]
                                var macros = keyboardConfiguratorController.macros

                                for (var slot = 0; slot < macros.rowCount(); slot++)
                                {
                                    choices.push(macros.isEmpty(slot)
                                                 ? qsTr("%1. %2 (empty)").arg(slot + 1).arg(macros.macroName(slot))
                                                 : qsTr("%1. %2").arg(slot + 1).arg(macros.macroName(slot)))
                                }

                                model = choices
                                currentIndex = keyboardConfiguratorController.customMappedKeys.selectedMacroSlot() + 1
                            }

                            onActivated: (index) => {
                                keyboardConfiguratorController.setSelectedKeyMacro(index - 1)
                                kbCanvas.requestPaint()
                            }
                        }
                    }

                    EnhancedButton {
                        text: qsTr("Edit macros...")
                        flat: true
                        highlighted: true
                        visible: keyboardConfiguratorController.macrosSupported

                        onClicked: {
                            configureKeyMapPopup.close()
                            macroEditorPopup.open()
                        }
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
                                    EnhancedButton {
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
                                            keyMapMacroComboBox.refresh()
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
                                            keyMapMacroComboBox.refresh()
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

                EnhancedButton {
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

                EnhancedButton {
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

                EnhancedButton {
                    text: qsTr("Report Issue")
                    flat: true
                    highlighted: true

                    onClicked: settingsController.reportIssue()
                }

                EnhancedButton {
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


        EnhancedButton {
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

        EnhancedButton {
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

        EnhancedButton {
            id: macrosButton
            anchors.top: keyRemapModeButton.bottom
            width: parent.width

            topInset: 0
            bottomInset: 0
            background.implicitHeight: 50

            visible: keyboardConfiguratorController.macrosSupported

            flat: true

            display: AbstractButton.TextUnderIcon
            text: qsTr("Macros")
            font.bold: false
            font.pixelSize: 12
            font.capitalization: Font.MixedCase

            icon.source: Icons.get(Icons.Edit)

            onClicked: macroEditorPopup.open()

            Material.foreground: macroEditorPopup.opened
                   ? Material.accentColor : ((Material.theme == Material.Light) ? "#000000" : "#ffffff")

            ToolTip.visible: hovered
            ToolTip.text: qsTr("Record macros to store on the keyboard")
            ToolTip.delay: mainWindowController.toolTipDelay
            ToolTip.timeout: mainWindowController.toolTipTimeout
        }

        EnhancedButton {
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

        EnhancedButton {
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

            delegate: EnhancedButton {
                id: lightModeMenuOption

                topInset: 0
                bottomInset: 0
                background.implicitHeight: 50

                flat: true

                width: lightModeMenu.width

                contentItem: Item {
                    Label {
                        text: model.name
                        color: (keyboardConfiguratorController.selectedLightModeIndex === model.index &&
                                keyboardConfiguratorController.selectedCustomEffectIndex === -1)
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
                    if (keyboardConfiguratorController.selectedLightModeIndex !== model.index ||
                            keyboardConfiguratorController.selectedCustomEffectIndex !== -1)
                    {
                        var modeIndex = model.index

                        discardCustomEffectChangesPopup.confirm(function() {
                            keyboardConfiguratorController.unsavedChanges = true
                            keyboardConfiguratorController.selectedCustomEffectIndex = -1
                            keyboardConfiguratorController.selectedLightModeIndex = modeIndex
                        })
                    }
                }
            }

            footer: Column {
                width: lightModeMenu.width

                Label {
                    text: qsTr("My Effects")
                    font.bold: true
                    font.pixelSize: 13
                    opacity: 0.7
                    leftPadding: 10
                    topPadding: 15
                    bottomPadding: 5
                }

                Label {
                    visible: customEffectsRepeater.count === 0
                    width: parent.width
                    text: qsTr("Choose Custom, light up some keys, then click \"Save as new effect\".")
                    wrapMode: Text.WordWrap
                    font.pixelSize: 12
                    opacity: 0.6
                    leftPadding: 10
                    rightPadding: 10
                    bottomPadding: 10
                }

                Repeater {
                    id: customEffectsRepeater

                    model: keyboardConfiguratorController.customEffects

                    delegate: EnhancedButton {
                        id: customEffectOption

                        topInset: 0
                        bottomInset: 0
                        background.implicitHeight: 50

                        flat: true

                        width: lightModeMenu.width

                        readonly property bool selected:
                            keyboardConfiguratorController.selectedCustomEffectIndex === model.index

                        contentItem: Item {
                            Label {
                                text: (model.animated ? "\u25B6 " : "") + model.name +
                                      ((customEffectOption.selected && keyboardConfiguratorController.customEffectModified)
                                       ? " *" : "")
                                color: customEffectOption.selected
                                       ? Material.accentColor : ((Material.theme == Material.Light) ? "#000000" : "#ffffff")
                                font.pixelSize: 14
                                elide: Text.ElideRight
                                anchors {
                                    left: parent.left
                                    leftMargin: 10
                                    right: customEffectMenuButton.left
                                    verticalCenter: parent.verticalCenter
                                }
                            }

                            RoundButton {
                                id: customEffectMenuButton

                                anchors {
                                    right: parent.right
                                    verticalCenter: parent.verticalCenter
                                }

                                flat: true
                                icon.source: Icons.get(Icons.Edit)

                                onClicked: customEffectMenu.open()

                                Menu {
                                    id: customEffectMenu

                                    Action {
                                        text: qsTr("Rename")
                                        onTriggered: customEffectNamePopup.showRename(model.index, model.name)
                                    }

                                    Action {
                                        text: qsTr("Duplicate")
                                        onTriggered: keyboardConfiguratorController.duplicateCustomEffect(
                                                         model.index, qsTr("%1 (copy)").arg(model.name))
                                    }

                                    Action {
                                        text: qsTr("Delete")
                                        icon.source: Icons.get(Icons.Delete)
                                        onTriggered: deleteCustomEffectPopup.show(model.index, model.name)
                                    }
                                }
                            }
                        }

                        onClicked: {
                            if (!selected)
                            {
                                var effectIndex = model.index

                                discardCustomEffectChangesPopup.confirm(function() {
                                    keyboardConfiguratorController.unsavedChanges = true
                                    keyboardConfiguratorController.selectedCustomEffectIndex = effectIndex
                                })
                            }
                        }
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

            Label {
                anchors.verticalCenter: parent.verticalCenter

                visible: keyboardConfiguratorController.playing

                text: qsTr("\u25B6 Animation playing on keyboard")
                color: Material.accentColor
                font.pixelSize: 14
            }

            EnhancedButton {
                anchors.verticalCenter: parent.verticalCenter

                visible: keyboardConfiguratorController.playing

                text: qsTr("Stop")
                flat: true
                highlighted: true

                onClicked: keyboardConfiguratorController.stopPlayback()

                ToolTip.visible: hovered
                ToolTip.text: qsTr("Stop the animation and go back to the keys saved on the keyboard")
                ToolTip.delay: mainWindowController.toolTipDelay
                ToolTip.timeout: mainWindowController.toolTipTimeout
            }

            EnhancedButton {
                anchors.verticalCenter: parent.verticalCenter

                visible: keyboardConfiguratorController.canResumePlayback

                text: qsTr("Play animation")
                flat: true
                highlighted: true

                onClicked: keyboardConfiguratorController.resumePlayback()

                ToolTip.visible: hovered
                ToolTip.text: qsTr("Play the last applied animation again")
                ToolTip.delay: mainWindowController.toolTipDelay
                ToolTip.timeout: mainWindowController.toolTipTimeout
            }

            // Applying only saves the profile, so point out edits that would stay out of My Effects
            Label {
                anchors.verticalCenter: parent.verticalCenter

                visible: keyboardConfiguratorController.customEffectModified &&
                         keyboardConfiguratorController.selectedCustomEffectIndex !== -1

                text: qsTr("Effect not saved")
                color: Material.color(Material.Orange)
                font.pixelSize: 13

                MouseArea {
                    id: effectNotSavedHover

                    anchors.fill: parent
                    hoverEnabled: true
                }

                ToolTip.visible: effectNotSavedHover.containsMouse
                ToolTip.text: qsTr("Your changes to this effect aren't in My Effects yet. Use Save effect or Save as new effect to keep them; Apply to keyboard only updates this profile.")
                ToolTip.delay: mainWindowController.toolTipDelay
                ToolTip.timeout: mainWindowController.toolTipTimeout
            }

            EnhancedButton {
                id: resetButton

                anchors.verticalCenter: parent.verticalCenter

                text: qsTr("Undo changes")
                flat: true

                onClicked: keyboardConfiguratorController.reset()

                enabled: keyboardConfiguratorController.unsavedChanges

                ToolTip.visible: hovered
                ToolTip.text: qsTr("Throw away changes you haven't applied and go back to the profile as last applied")
                ToolTip.delay: mainWindowController.toolTipDelay
                ToolTip.timeout: mainWindowController.toolTipTimeout
            }

            EnhancedButton {
                id: saveButton

                anchors.verticalCenter: parent.verticalCenter

                text: qsTr("Apply to keyboard")

                flat: ! keyboardConfiguratorController.unsavedChanges

                highlighted: keyboardConfiguratorController.unsavedChanges
                enabled: ! keyboardConfiguratorController.savingChanges

                onClicked: keyboardConfiguratorController.save()

                ToolTip.visible: hovered
                ToolTip.text: qsTr("Save these settings to the current profile and send them to the keyboard")
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
                    topMargin: 20
                    leftMargin: Math.abs((svItem.width - (kbCanvas.width * kbCanvasScale.xScale)) / 2)
                    left: parent.left
                }

                MouseArea {
                    anchors.fill: parent

                    enabled: keyboardConfiguratorController.isCustomLightModeSelected ||
                            (keyboardConfiguratorController.selectedMode === KeyboardConfiguratorController.KeyMap)

                    acceptedButtons: Qt.LeftButton | Qt.RightButton

                    onClicked: (mouse) => {
                        var l = mapToItem(kbCanvas,
                                          mouse.x,
                                          mouse.y)

                        if (mouse.button === Qt.RightButton)
                        {
                            keyboardConfiguratorController.clickKeySecondary(l.x, l.y)
                        }
                        else if (mouse.modifiers & (Qt.ShiftModifier | Qt.ControlModifier))
                        {
                            keyboardConfiguratorController.clickKeyGrouped(l.x, l.y)
                        }
                        else
                        {
                            keyboardConfiguratorController.clickKey(l.x, l.y)
                        }
                    }
                }

                Timer {
                    id: animationPreviewTimer

                    interval: 40
                    repeat: true
                    running: kbConfigurator.visible &&
                             Qt.application.state === Qt.ApplicationActive &&
                             keyboardConfiguratorController.animEnabled &&
                             keyboardConfiguratorController.isCustomLightModeSelected &&
                             keyboardConfiguratorController.selectedMode === KeyboardConfiguratorController.Light

                    onTriggered: {
                        keyboardConfiguratorController.advancePreview()
                        parent.requestPaint()
                    }

                    onRunningChanged: parent.requestPaint()
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

                        if (keyboardConfiguratorController.animEnabled)
                        {
                            var keyCount = keyboardConfiguratorController.keyCount()
                            var level;

                            for (i = 0; i < keyCount; i++)
                            {
                                level = keyboardConfiguratorController.previewLevel(i)
                                if (level <= 0) continue

                                key = keyboardConfiguratorController.getKey(i)
                                ctx.fillStyle = Qt.rgba(0.24, 0.55, 1.0, level / 250)
                                ctx.fillRect(key.topX,
                                             key.topY,
                                             (key.bottomX - key.topX),
                                             (key.bottomY - key.topY))
                            }
                        }
                        else
                        {
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
                        var showOrder = keyboardConfiguratorController.animEnabled &&
                                keyboardConfiguratorController.animPattern === 0
                        var showOrigin = keyboardConfiguratorController.animEnabled &&
                                keyboardConfiguratorController.animPattern === 3

                        ctx.font = "bold 11px sans-serif"

                        for (i = 0; i < acSize; i++)
                        {
                            key = keyboardConfiguratorController.getKeyByCustomLightModeKeysIndex(i)
                            ctx.strokeStyle = "#fc9d03"
                            ctx.strokeRect(key.topX,
                                           key.topY,
                                           (key.bottomX - key.topX),
                                           (key.bottomY - key.topY))

                            if (showOrigin && i === 0)
                            {
                                ctx.fillStyle = "#fc9d03"
                                ctx.fillText("\u25CE", key.topX + 3, key.topY + 12)
                            }
                        }

                        if (showOrder)
                        {
                            var labelKeyCount = keyboardConfiguratorController.keyCount()
                            var labels

                            ctx.fillStyle = "#fc9d03"

                            for (i = 0; i < labelKeyCount; i++)
                            {
                                labels = keyboardConfiguratorController.stepLabels(i)
                                if (labels.length === 0) continue

                                key = keyboardConfiguratorController.getKey(i)
                                ctx.fillText(labels, key.topX + 3, key.topY + 12,
                                             key.bottomX - key.topX - 4)
                            }
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

                implicitHeight: Math.max(lightModeControlsLeftColumn.implicitHeight,
                                         lightModeControlsRightColumn.implicitHeight)
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

                            EnhancedButton {
                                text: qsTr("Select all")

                                onClicked: keyboardConfiguratorController.selectAllKeysForCustomMode()
                            }

                            EnhancedButton {
                                text: qsTr("Unselect all")

                                onClicked: keyboardConfiguratorController.unselectAllKeysForCustomMode()
                            }
                        }

                        Row {
                            anchors.horizontalCenter: parent.horizontalCenter

                            spacing: 10

                            visible: keyboardConfiguratorController.isCustomLightModeSelected

                            EnhancedButton {
                                text: qsTr("Save effect")

                                visible: keyboardConfiguratorController.selectedCustomEffectIndex !== -1
                                enabled: keyboardConfiguratorController.customEffectModified &&
                                         ! keyboardConfiguratorController.savingChanges

                                onClicked: keyboardConfiguratorController.saveSelectedCustomEffect()

                                ToolTip.visible: hovered
                                ToolTip.text: qsTr("Save changes to this effect and apply it to the keyboard")
                                ToolTip.delay: mainWindowController.toolTipDelay
                                ToolTip.timeout: mainWindowController.toolTipTimeout
                            }

                            EnhancedButton {
                                text: qsTr("Save as new effect")

                                enabled: ! keyboardConfiguratorController.savingChanges

                                onClicked: customEffectNamePopup.showNew()

                                ToolTip.visible: hovered
                                ToolTip.text: qsTr("Save these keys as a new effect in My Effects")
                                ToolTip.delay: mainWindowController.toolTipDelay
                                ToolTip.timeout: mainWindowController.toolTipTimeout
                            }
                        }

                        CheckBox {
                            anchors.horizontalCenter: parent.horizontalCenter

                            text: qsTr("Animate")

                            visible: keyboardConfiguratorController.isCustomLightModeSelected
                            enabled: keyboardConfiguratorController.animationSupported

                            checked: keyboardConfiguratorController.animEnabled

                            onToggled: keyboardConfiguratorController.animEnabled = checked

                            ToolTip.visible: hovered && !enabled
                            ToolTip.text: qsTr("Animated effects are only supported on single colour keyboards for now")
                            ToolTip.delay: mainWindowController.toolTipDelay
                            ToolTip.timeout: mainWindowController.toolTipTimeout
                        }

                        Grid {
                            id: animationControls

                            anchors.horizontalCenter: parent.horizontalCenter

                            visible: keyboardConfiguratorController.isCustomLightModeSelected &&
                                     keyboardConfiguratorController.animEnabled

                            columns: 3
                            columnSpacing: 10

                            verticalItemAlignment: Grid.AlignVCenter
                            horizontalItemAlignment: Grid.AlignHCenter

                            readonly property bool isBreathing: keyboardConfiguratorController.animPattern === 4
                            readonly property bool shuffleOn: keyboardConfiguratorController.animShuffle &&
                                                              keyboardConfiguratorController.animPattern === 0
                            readonly property bool trailOn: keyboardConfiguratorController.animTrail > 0 &&
                                                            keyboardConfiguratorController.animHasDirection

                            function seconds(ms)
                            {
                                return qsTr("%1 s").arg((ms / 1000).toFixed(1))
                            }

                            Label {
                                text: qsTr("Pattern")
                            }

                            ComboBox {
                                width: 180

                                model: [qsTr("Sequence"), qsTr("Wave left to right"), qsTr("Wave top to bottom"),
                                    qsTr("Ripple"), qsTr("Breathing"), qsTr("Twinkle")]

                                currentIndex: keyboardConfiguratorController.animPattern

                                onActivated: (index) => keyboardConfiguratorController.animPattern = index
                            }

                            // Breathing and twinkle have no direction
                            CheckBox {
                                width: 80

                                text: qsTr("Reverse")
                                enabled: keyboardConfiguratorController.animPattern <= 3 &&
                                         ! animationControls.shuffleOn

                                checked: keyboardConfiguratorController.animReverse

                                onToggled: keyboardConfiguratorController.animReverse = checked

                                ToolTip.visible: hovered
                                ToolTip.text: qsTr("Play backwards: right to left, bottom to top, or a ripple coming inwards")
                                ToolTip.delay: mainWindowController.toolTipDelay
                                ToolTip.timeout: mainWindowController.toolTipTimeout
                            }

                            Label {
                                text: qsTr("Speed")
                                visible: ! animationControls.isBreathing
                            }

                            // Right is faster, so the slider runs from long steps to short ones
                            Slider {
                                width: 180
                                from: 600
                                to: 20
                                stepSize: 10
                                snapMode: Slider.SnapAlways

                                visible: ! animationControls.isBreathing

                                value: keyboardConfiguratorController.animStep

                                onMoved: keyboardConfiguratorController.animStep = value
                            }

                            Label {
                                width: 80
                                elide: Text.ElideRight
                                text: qsTr("%1 ms").arg(keyboardConfiguratorController.animStep)
                                visible: ! animationControls.isBreathing
                            }

                            Label {
                                text: animationControls.isBreathing ? qsTr("Breathe in") : qsTr("Fade in")
                            }

                            Slider {
                                width: 180
                                from: 0
                                to: 3000
                                stepSize: 50
                                snapMode: Slider.SnapAlways

                                value: keyboardConfiguratorController.animFadeIn

                                onMoved: keyboardConfiguratorController.animFadeIn = value
                            }

                            Label {
                                width: 80
                                elide: Text.ElideRight
                                text: (animationControls.isBreathing && keyboardConfiguratorController.animFadeIn === 0)
                                      ? qsTr("Same as out")
                                      : animationControls.seconds(keyboardConfiguratorController.animFadeIn)
                            }

                            Label {
                                text: animationControls.isBreathing ? qsTr("Breathe out") : qsTr("Fade out")
                            }

                            // A trail sets the fade out instead
                            Slider {
                                width: 180
                                from: 0
                                to: 3000
                                stepSize: 50
                                snapMode: Slider.SnapAlways

                                enabled: !animationControls.trailOn

                                value: keyboardConfiguratorController.animEffectiveFade

                                onMoved: keyboardConfiguratorController.animFade = value
                            }

                            Label {
                                width: 80
                                elide: Text.ElideRight
                                text: animationControls.seconds(keyboardConfiguratorController.animEffectiveFade)
                            }

                            Label {
                                text: qsTr("Trail")
                                enabled: keyboardConfiguratorController.animHasDirection
                            }

                            Slider {
                                width: 180
                                from: 0
                                to: 20
                                stepSize: 1
                                snapMode: Slider.SnapAlways

                                enabled: keyboardConfiguratorController.animHasDirection

                                value: keyboardConfiguratorController.animTrail

                                onMoved: keyboardConfiguratorController.animTrail = value

                                ToolTip.visible: hovered
                                ToolTip.text: qsTr("How many keys glow behind the moving light, at any speed. Off uses the fade out time instead.")
                                ToolTip.delay: mainWindowController.toolTipDelay
                                ToolTip.timeout: mainWindowController.toolTipTimeout
                            }

                            Label {
                                width: 80
                                elide: Text.ElideRight
                                enabled: keyboardConfiguratorController.animHasDirection
                                text: keyboardConfiguratorController.animTrail === 0
                                      ? qsTr("Off")
                                      : (keyboardConfiguratorController.animTrail === 1
                                         ? qsTr("1 key") : qsTr("%1 keys").arg(keyboardConfiguratorController.animTrail))
                            }

                            Label {
                                text: qsTr("Fade shape")
                                enabled: !animationControls.isBreathing
                            }

                            ComboBox {
                                width: 180

                                enabled: !animationControls.isBreathing

                                model: [qsTr("Linear"), qsTr("Smooth"), qsTr("Comet")]

                                currentIndex: keyboardConfiguratorController.animCurve

                                onActivated: (index) => keyboardConfiguratorController.animCurve = index

                                ToolTip.visible: hovered
                                ToolTip.text: qsTr("Linear fades evenly, Smooth eases in and out, Comet keeps a bright head with a long faint tail")
                                ToolTip.delay: mainWindowController.toolTipDelay
                                ToolTip.timeout: mainWindowController.toolTipTimeout
                            }

                            Item {
                                width: 80
                                height: 1
                            }

                            Label {
                                text: qsTr("Brightness")
                            }

                            Slider {
                                width: 180
                                from: 1
                                to: 10
                                stepSize: 1
                                snapMode: Slider.SnapAlways

                                value: Math.round(keyboardConfiguratorController.animBrightness / 25)

                                onMoved: keyboardConfiguratorController.animBrightness = value * 25
                            }

                            Label {
                                width: 80
                                elide: Text.ElideRight
                                text: qsTr("%1 / 10").arg(Math.round(keyboardConfiguratorController.animBrightness / 25))
                            }

                            Label {
                                text: qsTr("Loop")
                            }

                            ComboBox {
                                width: 180

                                model: [qsTr("Repeat"), qsTr("Back and forth"), qsTr("Play once")]

                                currentIndex: keyboardConfiguratorController.animLoop

                                onActivated: (index) => keyboardConfiguratorController.animLoop = index
                            }

                            // Only sequences have an order to shuffle
                            CheckBox {
                                width: 80

                                text: qsTr("Shuffle")
                                enabled: keyboardConfiguratorController.animPattern === 0

                                checked: keyboardConfiguratorController.animShuffle

                                onToggled: keyboardConfiguratorController.animShuffle = checked

                                ToolTip.visible: hovered
                                ToolTip.text: qsTr("Play the sequence's steps in a new random order every loop. Pauses stay where they are.")
                                ToolTip.delay: mainWindowController.toolTipDelay
                                ToolTip.timeout: mainWindowController.toolTipTimeout
                            }

                            Label {
                                text: qsTr("Pause")
                            }

                            Slider {
                                width: 180
                                from: 0
                                to: 3000
                                stepSize: 100
                                snapMode: Slider.SnapAlways

                                value: keyboardConfiguratorController.animPause

                                onMoved: keyboardConfiguratorController.animPause = value
                            }

                            Label {
                                width: 80
                                elide: Text.ElideRight
                                text: animationControls.seconds(keyboardConfiguratorController.animPause)
                            }
                        }

                        Label {
                            anchors.horizontalCenter: parent.horizontalCenter

                            width: animationControls.width
                            visible: animationControls.visible

                            wrapMode: Text.WordWrap
                            horizontalAlignment: Text.AlignHCenter
                            font.pixelSize: 12
                            opacity: 0.7

                            text: [
                                qsTr("Click keys in the order they should light up; a key can be used more than once. Shift+click adds a key to the last step so they light together. Right-click a key to remove its last step."),
                                qsTr("Sweeps across the whole keyboard, or only across the keys you pick (pick 2 or more)."),
                                qsTr("Sweeps down the whole keyboard, or only across the keys you pick (pick 2 or more)."),
                                qsTr("Spreads out from the first key you pick, or the middle of the keyboard. Pick more keys to limit it to those."),
                                qsTr("Fades the whole keyboard, or the keys you pick (2 or more), in and out."),
                                qsTr("Keys light up at random. Speed sets how often.")
                            ][keyboardConfiguratorController.animPattern] ?? ""
                        }

                        Column {
                            id: sequenceEditor

                            anchors.horizontalCenter: parent.horizontalCenter

                            visible: keyboardConfiguratorController.isCustomLightModeSelected &&
                                     keyboardConfiguratorController.editingSequence

                            spacing: 8

                            property bool recording: false

                            // Native scan codes currently held down, so chords record as one step
                            property var heldKeys: []

                            onVisibleChanged: if (!visible) recording = false
                            onRecordingChanged: {
                                heldKeys = []
                                lastPressTime = 0
                            }

                            // Turn the gaps between presses into pauses while recording
                            property bool keepTiming: false

                            // When the last step was recorded, in ms since the epoch
                            property real lastPressTime: 0

                            // Takes the focus while recording so presses on the real keyboard become steps
                            Item {
                                id: sequenceRecorder

                                width: 0
                                height: 0

                                focus: sequenceEditor.recording

                                Keys.onPressed: (event) => {
                                    event.accepted = true
                                    if (event.isAutoRepeat) return

                                    var withPrevious = sequenceEditor.heldKeys.length > 0
                                    sequenceEditor.heldKeys.push(event.nativeScanCode)

                                    var now = Date.now()
                                    var gap = 0

                                    if (sequenceEditor.keepTiming && !withPrevious && sequenceEditor.lastPressTime > 0)
                                    {
                                        gap = now - sequenceEditor.lastPressTime
                                    }

                                    var recorded = keyboardConfiguratorController.recordKey(event.nativeScanCode,
                                                                                            withPrevious, gap)
                                    sequenceRecordWarning.visible = !recorded

                                    // Chord keys join the step, they don't restart the gap
                                    if (recorded && !withPrevious) sequenceEditor.lastPressTime = now
                                }

                                Keys.onReleased: (event) => {
                                    event.accepted = true
                                    if (event.isAutoRepeat) return

                                    var i = sequenceEditor.heldKeys.indexOf(event.nativeScanCode)
                                    if (i !== -1) sequenceEditor.heldKeys.splice(i, 1)
                                }

                                onActiveFocusChanged: if (!activeFocus) sequenceEditor.recording = false
                            }

                            Label {
                                anchors.horizontalCenter: parent.horizontalCenter

                                width: animationControls.width
                                wrapMode: Text.WordWrap
                                horizontalAlignment: Text.AlignHCenter

                                text: keyboardConfiguratorController.sequenceLength === 0
                                      ? qsTr("No steps yet")
                                      : qsTr("%1 steps:  %2").arg(keyboardConfiguratorController.sequenceLength)
                                                             .arg(keyboardConfiguratorController.sequenceText)
                            }

                            Row {
                                anchors.horizontalCenter: parent.horizontalCenter

                                spacing: 10

                                TextField {
                                    id: sequenceTextField

                                    width: 200
                                    placeholderText: qsTr("Type keys, e.g. asdffdsa")

                                    enabled: !sequenceEditor.recording

                                    function add()
                                    {
                                        if (text.length === 0) return

                                        var unknown = keyboardConfiguratorController.addSequenceText(text)

                                        sequenceTextWarning.text = unknown.length === 0 ? ""
                                                : qsTr("Not on this keyboard: %1").arg(unknown)
                                        text = ""
                                    }

                                    onAccepted: add()
                                }

                                EnhancedButton {
                                    anchors.verticalCenter: parent.verticalCenter

                                    text: qsTr("Add")
                                    enabled: sequenceTextField.text.length > 0 && !sequenceEditor.recording

                                    onClicked: sequenceTextField.add()
                                }
                            }

                            Label {
                                id: sequenceTextWarning

                                anchors.horizontalCenter: parent.horizontalCenter

                                visible: text.length > 0
                                color: Material.color(Material.Red)
                                font.pixelSize: 12
                            }

                            Row {
                                anchors.horizontalCenter: parent.horizontalCenter

                                spacing: 10

                                EnhancedButton {
                                    text: sequenceEditor.recording ? qsTr("Done") : qsTr("Record")
                                    highlighted: sequenceEditor.recording

                                    // Taking the focus would stop the recording before the click restarts it
                                    focusPolicy: Qt.NoFocus

                                    onClicked: {
                                        sequenceEditor.recording = !sequenceEditor.recording
                                        sequenceRecordWarning.visible = false

                                        if (sequenceEditor.recording)
                                        {
                                            sequenceRecorder.forceActiveFocus()
                                        }
                                    }

                                    ToolTip.visible: hovered
                                    ToolTip.text: qsTr("Press keys on your keyboard to add them as steps. Keys pressed together become one step.")
                                    ToolTip.delay: mainWindowController.toolTipDelay
                                    ToolTip.timeout: mainWindowController.toolTipTimeout
                                }

                                CheckBox {
                                    anchors.verticalCenter: parent.verticalCenter

                                    text: qsTr("Keep timing")

                                    // Changing it mid-recording would steal focus and end the recording
                                    focusPolicy: Qt.NoFocus

                                    checked: sequenceEditor.keepTiming

                                    onToggled: sequenceEditor.keepTiming = checked

                                    ToolTip.visible: hovered
                                    ToolTip.text: qsTr("Record the gaps between your key presses as pauses, so the sequence plays back with your rhythm. A faster Speed records the timing more precisely; changing Speed later plays the whole rhythm faster or slower.")
                                    ToolTip.delay: mainWindowController.toolTipDelay
                                    ToolTip.timeout: mainWindowController.toolTipTimeout
                                }

                                EnhancedButton {
                                    text: qsTr("Add pause")

                                    // Usable while recording without ending it
                                    focusPolicy: Qt.NoFocus

                                    onClicked: keyboardConfiguratorController.addSequencePause()

                                    ToolTip.visible: hovered
                                    ToolTip.text: qsTr("Add a step with nothing lit. It lasts as long as one step; add more for a longer pause.")
                                    ToolTip.delay: mainWindowController.toolTipDelay
                                    ToolTip.timeout: mainWindowController.toolTipTimeout
                                }

                                EnhancedButton {
                                    text: qsTr("Undo step")
                                    enabled: keyboardConfiguratorController.sequenceLength > 0

                                    onClicked: keyboardConfiguratorController.undoSequenceStep()
                                }

                                EnhancedButton {
                                    text: qsTr("Clear")
                                    enabled: keyboardConfiguratorController.sequenceLength > 0

                                    onClicked: keyboardConfiguratorController.clearSequence()
                                }
                            }

                            Label {
                                anchors.horizontalCenter: parent.horizontalCenter

                                visible: sequenceEditor.recording

                                text: qsTr("Recording, press keys on your keyboard. Click Done when finished.")
                                color: Material.accentColor
                            }

                            Label {
                                id: sequenceRecordWarning

                                anchors.horizontalCenter: parent.horizontalCenter

                                visible: false

                                text: qsTr("That key isn't on this keyboard (Fn can't be recorded)")
                                color: Material.color(Material.Red)
                                font.pixelSize: 12
                            }
                        }

                        CheckBox {
                            id: backgroundCheckBox

                            anchors.horizontalCenter: parent.horizontalCenter

                            visible: animationControls.visible

                            text: qsTr("Background layer")

                            checked: keyboardConfiguratorController.animBackground.enabled

                            onToggled: keyboardConfiguratorController.setAnimBackground("enabled", checked)

                            ToolTip.visible: hovered
                            ToolTip.text: qsTr("Play a second pattern across the whole keyboard underneath this one. Where both light a key, the brighter one shows.")
                            ToolTip.delay: mainWindowController.toolTipDelay
                            ToolTip.timeout: mainWindowController.toolTipTimeout
                        }

                        // The same controls as the main layer, minus the ones that only make sense for it
                        Grid {
                            id: backgroundControls

                            anchors.horizontalCenter: parent.horizontalCenter

                            visible: animationControls.visible && background.enabled

                            columns: 3
                            columnSpacing: 10

                            verticalItemAlignment: Grid.AlignVCenter
                            horizontalItemAlignment: Grid.AlignHCenter

                            readonly property var background: keyboardConfiguratorController.animBackground
                            readonly property bool isBreathing: background.pattern === 4

                            function set(name, value)
                            {
                                keyboardConfiguratorController.setAnimBackground(name, value)
                            }

                            Label {
                                text: qsTr("Pattern")
                            }

                            // Every pattern but Sequence, so the index is one less than the pattern
                            ComboBox {
                                width: 180

                                model: [qsTr("Wave left to right"), qsTr("Wave top to bottom"),
                                    qsTr("Ripple"), qsTr("Breathing"), qsTr("Twinkle")]

                                currentIndex: backgroundControls.background.pattern - 1

                                onActivated: (index) => backgroundControls.set("pattern", index + 1)
                            }

                            CheckBox {
                                width: 80

                                text: qsTr("Reverse")
                                enabled: backgroundControls.background.pattern <= 3

                                checked: backgroundControls.background.reverse

                                onToggled: backgroundControls.set("reverse", checked)
                            }

                            Label {
                                text: qsTr("Speed")
                                visible: ! backgroundControls.isBreathing
                            }

                            Slider {
                                width: 180
                                from: 600
                                to: 20
                                stepSize: 10
                                snapMode: Slider.SnapAlways

                                visible: ! backgroundControls.isBreathing

                                value: backgroundControls.background.step

                                onMoved: backgroundControls.set("step", value)
                            }

                            Label {
                                width: 80
                                elide: Text.ElideRight
                                text: qsTr("%1 ms").arg(backgroundControls.background.step)
                                visible: ! backgroundControls.isBreathing
                            }

                            Label {
                                text: backgroundControls.isBreathing ? qsTr("Breathe") : qsTr("Fade out")
                            }

                            Slider {
                                width: 180
                                from: 0
                                to: 3000
                                stepSize: 50
                                snapMode: Slider.SnapAlways

                                value: backgroundControls.background.fade

                                onMoved: backgroundControls.set("fade", value)
                            }

                            Label {
                                width: 80
                                elide: Text.ElideRight
                                text: animationControls.seconds(backgroundControls.background.fade)
                            }

                            Label {
                                text: qsTr("Fade shape")
                                enabled: ! backgroundControls.isBreathing
                            }

                            ComboBox {
                                width: 180

                                enabled: ! backgroundControls.isBreathing

                                model: [qsTr("Linear"), qsTr("Smooth"), qsTr("Comet")]

                                currentIndex: backgroundControls.background.curve

                                onActivated: (index) => backgroundControls.set("curve", index)
                            }

                            Item {
                                width: 80
                                height: 1
                            }

                            Label {
                                text: qsTr("Brightness")
                            }

                            Slider {
                                width: 180
                                from: 1
                                to: 10
                                stepSize: 1
                                snapMode: Slider.SnapAlways

                                value: Math.round(backgroundControls.background.brightness / 25)

                                onMoved: backgroundControls.set("brightness", value * 25)
                            }

                            Label {
                                width: 80
                                elide: Text.ElideRight
                                text: qsTr("%1 / 10").arg(Math.round(backgroundControls.background.brightness / 25))
                            }

                            Label {
                                text: qsTr("Pause")
                            }

                            Slider {
                                width: 180
                                from: 0
                                to: 3000
                                stepSize: 100
                                snapMode: Slider.SnapAlways

                                value: backgroundControls.background.pause

                                onMoved: backgroundControls.set("pause", value)
                            }

                            Label {
                                width: 80
                                elide: Text.ElideRight
                                text: animationControls.seconds(backgroundControls.background.pause)
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

                        EnhancedButton {
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

                    EnhancedButton {
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
