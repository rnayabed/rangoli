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

#include "keyboardconfiguratorcontroller.h"

#include <cstring>
#include <QDesktopServices>

#include "mainwindowcontroller.h"
#include "enhanceddialog.h"
#include "hidconnection.h"
#include "keyinput.h"

using namespace Qt::Literals::StringLiterals;

KeyboardConfiguratorController::KeyboardConfiguratorController(QObject *parent)
    : QObject{parent}, m_selectedKeyboardIndex{-1}, m_randomColours{true},
      m_customColours{new CustomColours{this}},
      m_customLightModeKeys{new CustomLightModeKeys{this}},
      m_unsavedChanges{false}, m_savingChanges{false},
      m_selectedMode{Mode::Light}, m_selectedLightModeIndex{0},
      m_modes{new ModeModel{this}},
      m_customEffects{new CustomEffectModel{this}},
      m_macros{new MacroModel{this}},
      m_selectedCustomEffectIndex{-1}, m_customEffectModified{false},
      m_selectedProfileIndex{-1},
      m_brightness{5}, m_animation{5}, m_sleep{5},
      m_keyCodes{new KeyCodeModel{this}},
      m_keyCodesProxy{new KeyCodeModelProxy{this}},
      m_customMappedKeys{new CustomMappedKeys{this}},
      m_liveFrameInFlight{false}, m_liveFailures{0}, m_playing{false},
      m_startPlaybackAfterSave{false}, m_livePrepared{false}
{
    m_customMappedKeys->setMacros(m_macros);

    // Anything that changes the picked keys changes the preview
    connect(this, &KeyboardConfiguratorController::customLightModeKeysChanged,
            this, [this](){ preparePreview(); });

    m_keyCodesProxy->setSourceModel(m_keyCodes);
    m_keyCodesProxy->setSortRole(KeyCodeModel::KeyRoles::NameRole);
    m_keyCodesProxy->sort(0);
}

const bool &KeyboardConfiguratorController::randomColours()
{
    return m_randomColours;
}

const QColor &KeyboardConfiguratorController::selectedColour()
{
    return m_selectedColour;
}

CustomColours* KeyboardConfiguratorController::customColours()
{
    return m_customColours.get();
}

CustomLightModeKeys *KeyboardConfiguratorController::customLightModeKeys()
{
    return m_customLightModeKeys;
}

const bool &KeyboardConfiguratorController::unsavedChanges()
{
    return m_unsavedChanges;
}

const bool &KeyboardConfiguratorController::savingChanges()
{
    return m_savingChanges;
}

bool KeyboardConfiguratorController::keyMapEnabled()
{
    if (m_selectedKeyboardIndex == -1) return true;

    return selectedKeyboard().keyMapEnabled;
}

bool KeyboardConfiguratorController::macrosSupported()
{
    // The macro format has only been confirmed on some models, see docs/rk96/TESTING.md
    return m_selectedKeyboardIndex != -1 && selectedKeyboard().macrosEnabled;
}

const int &KeyboardConfiguratorController::brightness()
{
    return m_brightness;
}

const int &KeyboardConfiguratorController::animation()
{
    return m_animation;
}

const int &KeyboardConfiguratorController::sleep()
{
    return m_sleep;
}

ModeModel *KeyboardConfiguratorController::modes()
{
    return m_modes.get();
}

const KeyboardConfiguratorController::Mode &KeyboardConfiguratorController::selectedMode()
{
    return m_selectedMode;
}

const int &KeyboardConfiguratorController::selectedLightModeIndex()
{
    return m_selectedLightModeIndex;
}

bool KeyboardConfiguratorController::isCustomLightModeSelected()
{
    return m_modes->isCustomMode(m_selectedLightModeIndex);
}

bool KeyboardConfiguratorController::showColoursColumn()
{
    if (m_selectedKeyboardIndex == -1) return true;

    return selectedKeyboard().rgb
            ? m_modes->getModeBit(m_selectedLightModeIndex) != static_cast<int>(ModeModel::RGBModes::Neon)
            : false;
}

CustomEffectModel *KeyboardConfiguratorController::customEffects()
{
    return m_customEffects.get();
}

const int &KeyboardConfiguratorController::selectedCustomEffectIndex()
{
    return m_selectedCustomEffectIndex;
}

const bool &KeyboardConfiguratorController::customEffectModified()
{
    return m_customEffectModified;
}

bool KeyboardConfiguratorController::animationSupported()
{
    // Live lighting has only been worked out for single colour keyboards
    return m_selectedKeyboardIndex != -1 && !selectedKeyboard().rgb;
}

bool KeyboardConfiguratorController::animEnabled()
{
    return m_anim.enabled;
}

int KeyboardConfiguratorController::animPattern()
{
    return m_anim.pattern;
}

int KeyboardConfiguratorController::animStep()
{
    return m_anim.stepMs;
}

int KeyboardConfiguratorController::animFade()
{
    return m_anim.fadeMs;
}

int KeyboardConfiguratorController::animFadeIn()
{
    return m_anim.fadeInMs;
}

bool KeyboardConfiguratorController::animReverse()
{
    return m_anim.reverse;
}

bool KeyboardConfiguratorController::animShuffle()
{
    return m_anim.shuffle;
}

int KeyboardConfiguratorController::animTrail()
{
    return m_anim.trailKeys;
}

int KeyboardConfiguratorController::animCurve()
{
    return m_anim.fadeCurve;
}

int KeyboardConfiguratorController::animEffectiveFade()
{
    return m_anim.effectiveFadeMs();
}

bool KeyboardConfiguratorController::animHasDirection()
{
    return m_anim.hasDirection();
}

int KeyboardConfiguratorController::animBrightness()
{
    return m_anim.brightness;
}

int KeyboardConfiguratorController::animLoop()
{
    return m_anim.loop;
}

int KeyboardConfiguratorController::animPause()
{
    return m_anim.pauseMs;
}

QVariantMap KeyboardConfiguratorController::animBackground()
{
    const AnimationSettings::Background& b = m_anim.background;

    return {
        {u"enabled"_s, b.enabled},
        {u"pattern"_s, b.pattern},
        {u"step"_s, b.stepMs},
        {u"fade"_s, b.fadeMs},
        {u"curve"_s, b.fadeCurve},
        {u"brightness"_s, b.brightness},
        {u"pause"_s, b.pauseMs},
        {u"reverse"_s, b.reverse}
    };
}

const bool &KeyboardConfiguratorController::playing()
{
    return m_playing;
}

bool KeyboardConfiguratorController::canResumePlayback()
{
    return m_livePrepared && !m_playing;
}

bool KeyboardConfiguratorController::editingSequence()
{
    return m_anim.enabled && m_anim.pattern == AnimationSettings::Sequence;
}

QString KeyboardConfiguratorController::sequenceText()
{
    QStringList steps;

    for (auto&& step : m_sequence)
    {
        if (step.isEmpty())
        {
            steps << QString(QChar(0x2013));
            continue;
        }

        QStringList names;
        for (auto&& keyIndex : step)
        {
            names << KeyCode::Names.value(selectedKeyboard().keys.at(keyIndex).keyCode, u"?"_s);
        }
        steps << names.join(u'+');
    }

    return steps.join(u"  "_s);
}

int KeyboardConfiguratorController::sequenceLength()
{
    return m_sequence.size();
}

bool KeyboardConfiguratorController::defaultProfileAnimated(const Keyboard &keyboard)
{
    if (keyboard.rgb || !m_profiles || m_profiles->rowCount() == 0) return false;

    const QString keyboardID = QStringLiteral("%1:%2").arg(QString::number(keyboard.id.vid),
                                                            QString::number(keyboard.id.pid));
    const QString profileID = m_profiles->data(m_profiles->index(m_profiles->defaultProfileIndex(), 0),
                                               ProfileModel::ProfileModelRoles::IDRole).toString();

    QSettings settings;
    settings.beginGroup(QStringLiteral("profiles/%1/%2").arg(profileID, keyboardID));

    if (settings.value(u"selected_light_mode_index"_s).toInt() != m_modes->customModeIndex())
    {
        return false;
    }

    const QString effectID = settings.value(u"selected_custom_effect"_s).toString();
    bool animated = AnimationSettings::read(settings).enabled;
    settings.endGroup();

    settings.beginGroup(QStringLiteral("custom_effects/%1").arg(keyboardID));
    if (!effectID.isEmpty() && settings.childGroups().contains(effectID))
    {
        settings.beginGroup(effectID);
        animated = AnimationSettings::read(settings).enabled;
        settings.endGroup();
    }
    settings.endGroup();

    return animated;
}

void KeyboardConfiguratorController::stopPlaybackBlocking()
{
    m_startPlaybackAfterSave = false;

    if (!m_playing) return;

    qInfo() << "Stop live lighting before quitting";

    m_liveTimer->stop();
    m_playing = false;
    emit playingChanged();

    unsigned char** buffers = takeRestoreBuffers();
    if (buffers)
    {
        HIDConnection::getInstance().sendDataBlocking(m_livePath, buffers, 1);
    }
}

ProfileModel *KeyboardConfiguratorController::profiles()
{
    return m_profiles.get();
}

const int &KeyboardConfiguratorController::selectedProfileIndex()
{
    return m_selectedProfileIndex;
}

QSortFilterProxyModel *KeyboardConfiguratorController::keyCodesProxy()
{
    return m_keyCodesProxy.get();
}

CustomMappedKeys *KeyboardConfiguratorController::customMappedKeys()
{
    return m_customMappedKeys.get();
}

MacroModel *KeyboardConfiguratorController::macros()
{
    return m_macros.get();
}

int KeyboardConfiguratorController::editingMacroSlot()
{
    return m_editingMacroSlot;
}

QString KeyboardConfiguratorController::editingMacroName()
{
    return m_editingMacro.name;
}

QVariantList KeyboardConfiguratorController::editingMacroEvents()
{
    QVariantList events;

    for (auto&& event : m_editingMacro.events)
    {
        events << QVariantMap{{u"text"_s, event.text()}, {u"delay"_s, event.delayMs}};
    }

    return events;
}

bool KeyboardConfiguratorController::editingMacroModified()
{
    return m_editingMacroModified;
}

void KeyboardConfiguratorController::editMacro(int slot)
{
    if (slot < 0 || slot >= MacroModel::SlotCount) return;

    m_editingMacroSlot = slot;
    m_editingMacro = m_macros->macro(slot);
    m_editingMacroModified = false;

    emit macroEditorChanged();
}

void KeyboardConfiguratorController::setEditingMacroName(const QString &name)
{
    if (m_editingMacro.name == name) return;

    m_editingMacro.name = name;
    m_editingMacroModified = true;

    emit macroEditorChanged();
}

bool KeyboardConfiguratorController::addMacroEvent(const MacroEvent &event, int gapMs, int defaultDelayMs)
{
    if (m_editingMacroSlot == -1 || m_editingMacro.events.size() >= MacroModel::MaxEvents) return false;

    // The delay belongs to the event before, it is the wait after it
    if (!m_editingMacro.events.isEmpty())
    {
        m_editingMacro.events.last().delayMs = qBound(0, gapMs > 0 ? gapMs : defaultDelayMs,
                                                      MacroModel::MaxDelayMs);
    }

    m_editingMacro.events << event;
    m_editingMacro.events.last().delayMs = qBound(0, defaultDelayMs, MacroModel::MaxDelayMs);
    m_editingMacroModified = true;

    return true;
}

bool KeyboardConfiguratorController::recordMacroKey(int nativeScanCode, bool release, int gapMs, int defaultDelayMs)
{
    const int code = KeyInput::codeForNativeScanCode(nativeScanCode);

    MacroEvent event;
    event.release = release;

    if (code >= 0x010000 && code < 0x01000000)
    {
        event.modifier = true;
        event.code = quint8(code >> 16);
    }
    else if (code >= 0x0100 && code < 0x010000)
    {
        event.code = quint8(code >> 8);
    }
    else
    {
        return false;
    }

    const bool added = addMacroEvent(event, gapMs, defaultDelayMs);
    emit macroEditorChanged();
    return added;
}

QString KeyboardConfiguratorController::addMacroText(const QString &text, int delayMs)
{
    QString unknown;

    auto add = [this, delayMs](bool release, bool modifier, quint8 code) {
        MacroEvent event;
        event.release = release;
        event.modifier = modifier;
        event.code = code;
        return addMacroEvent(event, 0, delayMs);
    };

    // Keys that can't be typed go in braces by name, e.g. {Tab} or {F5}
    auto namedKey = [](const QString& name) {
        const QString wanted = name.simplified().remove(u' ').toLower();

        for (auto it = KeyCode::Names.cbegin(); it != KeyCode::Names.cend(); ++it)
        {
            const int code = it.key();

            // Only plain keys; modifiers on their own would just be tapped
            if (code < 0x0100 || code >= 0x010000) continue;

            if (QString(it.value()).remove(u' ').toLower() == wanted) return it.key();
        }

        return KeyCode::Key_Invalid;
    };

    for (int i = 0; i < text.size(); i++)
    {
        const QChar character = text.at(i);
        KeyCode::Code code = KeyCode::Key_Invalid;
        bool shift = false;

        const int close = character == u'{' ? text.indexOf(u'}', i + 1) : -1;

        if (close != -1)
        {
            const QString name = text.mid(i + 1, close - i - 1);
            code = namedKey(name);
            i = close;

            if (code == KeyCode::Key_Invalid)
            {
                unknown += u"{%1}"_s.arg(name);
                continue;
            }
        }
        else
        {
            code = KeyInput::codeForCharacter(character);
            shift = KeyInput::needsShift(character);

            if (code == KeyCode::Key_Invalid)
            {
                if (!unknown.contains(character)) unknown += character;
                continue;
            }
        }

        const int needed = shift ? 4 : 2;

        // Never leave a key or Shift held down because the macro ran out of room
        if (m_editingMacro.events.size() + needed > MacroModel::MaxEvents) break;

        if (shift) add(false, true, 0x02);
        add(false, false, quint8(code >> 8));
        add(true, false, quint8(code >> 8));
        if (shift) add(true, true, 0x02);
    }

    emit macroEditorChanged();
    return unknown;
}

void KeyboardConfiguratorController::setMacroEventDelay(int index, int delayMs)
{
    if (index < 0 || index >= m_editingMacro.events.size()) return;

    m_editingMacro.events[index].delayMs = qBound(0, delayMs, MacroModel::MaxDelayMs);
    m_editingMacroModified = true;

    emit macroEditorChanged();
}

void KeyboardConfiguratorController::setAllMacroDelays(int delayMs)
{
    for (auto& event : m_editingMacro.events)
    {
        event.delayMs = qBound(0, delayMs, MacroModel::MaxDelayMs);
    }

    m_editingMacroModified = true;

    emit macroEditorChanged();
}

void KeyboardConfiguratorController::removeMacroEvent(int index)
{
    if (index < 0 || index >= m_editingMacro.events.size()) return;

    m_editingMacro.events.removeAt(index);
    m_editingMacroModified = true;

    emit macroEditorChanged();
}

void KeyboardConfiguratorController::clearMacroEvents()
{
    m_editingMacro.events.clear();
    m_editingMacroModified = true;

    emit macroEditorChanged();
}

void KeyboardConfiguratorController::saveEditingMacro()
{
    if (m_editingMacroSlot == -1) return;

    m_macros->setMacro(m_editingMacroSlot, m_editingMacro);
    m_editingMacroModified = false;

    // Macros reach the keyboard with the next apply
    setUnsavedChanges(true);

    emit macroEditorChanged();
}

void KeyboardConfiguratorController::deleteMacro(int slot)
{
    m_macros->setMacro(slot, Macro{});

    if (slot == m_editingMacroSlot) editMacro(slot);

    setUnsavedChanges(true);
}

void KeyboardConfiguratorController::setSelectedKeyMacro(int slot)
{
    if (slot < 0)
    {
        m_customMappedKeys->clear();
    }
    else
    {
        m_customMappedKeys->setMacro(slot);
    }

    setUnsavedChanges(true);
}

const int &KeyboardConfiguratorController::topLeftX()
{
    return selectedKeyboard().topLeftX;
}

const int &KeyboardConfiguratorController::topLeftY()
{
    return selectedKeyboard().topLeftY;
}

const int &KeyboardConfiguratorController::bottomRightX()
{
    return selectedKeyboard().bottomRightX;
}

const int &KeyboardConfiguratorController::bottomRightY()
{
    return selectedKeyboard().bottomRightY;
}

Key KeyboardConfiguratorController::getKeyByCustomLightModeKeysIndex(int index)
{
    return selectedKeyboard().keys.at(m_customLightModeKeys->keyIndexes().at(index));
}

Key KeyboardConfiguratorController::getKeyByCustomMappedKeysIndex(int index)
{
    return selectedKeyboard().keys.at(m_customMappedKeys->keyIndexes().at(index));
}

int KeyboardConfiguratorController::keyCount()
{
    if (m_selectedKeyboardIndex == -1) return 0;

    return selectedKeyboard().keys.size();
}

Key KeyboardConfiguratorController::getKey(int index)
{
    return selectedKeyboard().keys.at(index);
}

int KeyboardConfiguratorController::previewLevel(int keyIndex)
{
    return m_previewLevels.value(keyIndex, 0);
}

QString KeyboardConfiguratorController::stepLabels(int keyIndex)
{
    QStringList labels;

    for (int n = 0; n < m_sequence.size(); n++)
    {
        if (m_sequence.at(n).contains(keyIndex)) labels << QString::number(n + 1);
    }

    return labels.join(QChar(0x00B7));
}

QString KeyboardConfiguratorController::addSequenceText(const QString &text)
{
    QList<QList<int>> sequence = m_sequence;
    QString unknown;

    for (auto&& c : text)
    {
        const int keyIndex = keyIndexForCode(KeyInput::codeForCharacter(c));

        if (keyIndex == -1)
        {
            if (!unknown.contains(c)) unknown += c;
        }
        else
        {
            sequence << QList<int>{keyIndex};
        }
    }

    if (sequence.size() != m_sequence.size())
    {
        setSequence(sequence, true);
    }

    return unknown;
}

static constexpr int MaxRecordedGapMs = 5000;

bool KeyboardConfiguratorController::recordKey(int nativeScanCode, bool withPrevious, int gapMs)
{
    const int keyIndex = keyIndexForCode(KeyInput::codeForNativeScanCode(nativeScanCode));

    if (keyIndex == -1) return false;

    QList<QList<int>> sequence = m_sequence;

    if (withPrevious && !sequence.isEmpty() && !sequence.last().isEmpty())
    {
        if (!sequence.last().contains(keyIndex)) sequence.last() << keyIndex;
    }
    else
    {
        // The previous step already lasts one step, the rest of the gap is pauses.
        // Long gaps are capped so walking away doesn't add a minute of nothing.
        if (gapMs > 0 && !sequence.isEmpty())
        {
            const int pauses = qRound(qMin(gapMs, MaxRecordedGapMs) / double(qMax(1, m_anim.stepMs))) - 1;
            for (int n = 0; n < pauses; n++) sequence << QList<int>{};
        }

        sequence << QList<int>{keyIndex};
    }

    setSequence(sequence, true);

    return true;
}

void KeyboardConfiguratorController::advancePreview()
{
    // Animations that play once are replayed after a second so the preview keeps going
    if (m_previewAnimation.finished(m_previewClock.elapsed() - 1000))
    {
        m_previewClock.restart();
    }

    m_previewLevels = m_previewAnimation.render(m_previewClock.elapsed());
}

void KeyboardConfiguratorController::init()
{
    auto mainWindowController = qvariant_cast<MainWindowController*>(qmlEngine(this)
                                                                     ->rootContext()
                                                                     ->contextProperty("mainWindowController"));
    m_profiles = mainWindowController->profiles();

    m_connectedKeyboards = mainWindowController->connectedKeyboards();

    HIDConnection& connection = HIDConnection::getInstance();

    connect(&connection, &HIDConnection::dataSentSuccessfully, this, [this](){
        setSavingChanges(false);

        if (m_macroAreaPending)
        {
            m_macroAreaPending = false;
            m_macros->markAreaWritten();
        }

        if (m_startPlaybackAfterSave)
        {
            m_startPlaybackAfterSave = false;
            startPlayback();
        }
    });

    m_liveTimer = new QTimer{this};
    m_liveTimer->setInterval(40);
    connect(m_liveTimer, &QTimer::timeout, this, &KeyboardConfiguratorController::sendLiveFrame);

    connect(&connection, &HIDConnection::liveFrameSent, this, [this](const bool& ok){
        m_liveFrameInFlight = false;

        if (ok)
        {
            m_liveFailures = 0;
        }
        else if (m_playing && ++m_liveFailures >= 5)
        {
            qWarning() << "Live lighting keeps failing, stop playing";
            stopPlayback(false);
        }
    });

    connect(&connection, &HIDConnection::failedToSendData, this, [this](const QString& message){
        setSavingChanges(false);
        m_macroAreaPending = false;

        if (m_startPlaybackAfterSave)
        {
            m_startPlaybackAfterSave = false;
            startPlayback();
        }

        MainWindowController::showEnhancedDialog(this, EnhancedDialog(
                tr("Error"),
                message));
    });
}

void KeyboardConfiguratorController::load(const int &keyboardIndex)
{
    m_selectedKeyboardIndex = keyboardIndex;

    qInfo() << "Load keyboard" << selectedKeyboard().id;

    emit loadName(selectedKeyboard().name);
    emit loadImage(selectedKeyboard().imagePath);

    emit keyMapEnabledChanged();

    m_modes->setRGB(selectedKeyboard().rgb);

    m_customEffects->load(QStringLiteral("%1:%2")
                          .arg(QString::number(selectedKeyboard().id.vid),
                               QString::number(selectedKeyboard().id.pid)));

    m_macros->load(QStringLiteral("%1:%2")
                   .arg(QString::number(selectedKeyboard().id.vid),
                        QString::number(selectedKeyboard().id.pid)));
    m_editingMacroSlot = -1;
    m_editingMacroModified = false;
    emit macroEditorChanged();

    m_selectedCustomEffectIndex = -1;
    emit selectedCustomEffectIndexChanged();
    setCustomEffectModified(false);

    emit animationSupportedChanged();
}

void KeyboardConfiguratorController::applyDefaultProfile()
{
    setSelectedProfileIndex(m_profiles->defaultProfileIndex());
}

void KeyboardConfiguratorController::save()
{
    saveProfile();
    saveToKeyboard();
}

void KeyboardConfiguratorController::reset()
{
    loadProfileSettings();
}

void KeyboardConfiguratorController::setRandomColours(const bool &randomColours)
{
    if (m_randomColours == randomColours) return;

    m_randomColours = randomColours;
    emit randomColoursChanged();
}

void KeyboardConfiguratorController::setSelectedColour(const QColor &colour)
{
    if (m_selectedColour == colour) return;

    m_selectedColour = colour;
    emit selectedColourChanged();
}

void KeyboardConfiguratorController::setUnsavedChanges(const bool &unsavedChanges)
{
    if (m_unsavedChanges == unsavedChanges) return;

    m_unsavedChanges = unsavedChanges;
    emit unsavedChangesChanged();
}

void KeyboardConfiguratorController::setSavingChanges(const bool &savingChanges)
{
    if (m_savingChanges == savingChanges) return;

    m_savingChanges = savingChanges;
    emit savingChangesChanged();
}

void KeyboardConfiguratorController::setBrightness(const int &brightness)
{
    if (m_brightness == brightness) return;

    m_brightness = brightness;
    emit brightnessChanged();
}

void KeyboardConfiguratorController::setAnimation(const int &animation)
{
    if (m_animation == animation) return;

    m_animation = animation;
    emit animationChanged();
}

void KeyboardConfiguratorController::setSleep(const int &sleep)
{
    if (m_sleep == sleep) return;

    m_sleep = sleep;
    emit sleepChanged();
}

void KeyboardConfiguratorController::setSelectedMode(const KeyboardConfiguratorController::Mode& selectedMode)
{
    if (m_selectedMode == selectedMode) return;

    m_selectedMode = selectedMode;

    emit selectedModeChanged();
}

void KeyboardConfiguratorController::setSelectedLightModeIndex(const int &selectedLightModeIndex)
{
    if (m_selectedLightModeIndex == selectedLightModeIndex) return;

    bool customModeChanged = isCustomLightModeSelected() || m_modes->isCustomMode(selectedLightModeIndex);

    m_selectedLightModeIndex = selectedLightModeIndex;
    emit selectedLightModeIndexChanged();

    if (customModeChanged)
    {
        emit isCustomLightModeSelectedChanged();
    }

    emit showColoursColumnChanged();
}

void KeyboardConfiguratorController::setSelectedProfileIndex(const int &selectedProfileIndex)
{
    if (m_selectedProfileIndex == selectedProfileIndex) return;

    m_selectedProfileIndex = selectedProfileIndex;
    emit selectedProfileIndexChanged();

    QSettings settings;
    settings.setValue(u"last_profile"_s, getProfileID());

    loadProfileCustomColours();
    loadProfileSettings();

    saveToKeyboard();
}

void KeyboardConfiguratorController::clickKey(const int &x, const int &y)
{
    int index = getKeyIndexAtLocation(x, y);
    if (index == -1) return;

    if (m_selectedMode == Mode::Light && editingSequence())
    {
        QList<QList<int>> sequence = m_sequence;
        sequence << QList<int>{index};
        setSequence(sequence, true);
    }
    else if (m_selectedMode == Mode::Light)
    {
        if (selectedKeyboard().rgb)
        {
            m_customLightModeKeys->toggle(index, m_selectedColour);
        }
        else
        {
            m_customLightModeKeys->toggle(index);
        }

        markCustomLightModeKeysEdited();
        setUnsavedChanges(true);
        emit customLightModeKeysChanged();
    }
    else if (selectedKeyboard().keyMapEnabled)
    {
        m_customMappedKeys->setSelectedKeyIndex(index);
        emit openConfigureKeyMapPopup();
    }
}

void KeyboardConfiguratorController::clickKeySecondary(const int &x, const int &y)
{
    if (!(m_selectedMode == Mode::Light && editingSequence()))
    {
        clickKey(x, y);
        return;
    }

    const int index = getKeyIndexAtLocation(x, y);
    if (index == -1) return;

    // Take the key out of the last step it is in
    QList<QList<int>> sequence = m_sequence;

    for (int n = sequence.size() - 1; n >= 0; n--)
    {
        if (sequence[n].removeOne(index))
        {
            if (sequence.at(n).isEmpty()) sequence.removeAt(n);

            setSequence(sequence, true);
            return;
        }
    }
}

void KeyboardConfiguratorController::clickKeyGrouped(const int &x, const int &y)
{
    if (!(m_selectedMode == Mode::Light && editingSequence()))
    {
        clickKey(x, y);
        return;
    }

    const int index = getKeyIndexAtLocation(x, y);
    if (index == -1) return;

    toggleKeyInLastStep(index);
}

void KeyboardConfiguratorController::selectAllKeysForCustomMode()
{
    if (editingSequence())
    {
        QList<QList<int>> sequence;
        for (int i = 0; i < selectedKeyboard().keys.size(); i++) sequence << QList<int>{i};

        setSequence(sequence, true);
        return;
    }

    for (int i = 0; i < selectedKeyboard().keys.size(); i++)
    {
        m_customLightModeKeys->addIfNotExists(i, m_selectedColour);
    }

    markCustomLightModeKeysEdited();
    setUnsavedChanges(true);
    emit customLightModeKeysChanged();
}

void KeyboardConfiguratorController::unselectAllKeysForCustomMode()
{
    if (editingSequence())
    {
        setSequence({}, true);
        return;
    }

    m_customLightModeKeys->clear();

    markCustomLightModeKeysEdited();
    setUnsavedChanges(true);
    emit customLightModeKeysChanged();
}

void KeyboardConfiguratorController::setSelectedCustomEffectIndex(const int &selectedCustomEffectIndex)
{
    if (m_selectedCustomEffectIndex == selectedCustomEffectIndex) return;

    m_selectedCustomEffectIndex = selectedCustomEffectIndex;
    emit selectedCustomEffectIndexChanged();

    setCustomEffectModified(false);

    if (m_selectedCustomEffectIndex == -1) return;

    qInfo() << "Select custom effect" << m_customEffects->effectName(m_selectedCustomEffectIndex);

    const CustomEffect& effect = m_customEffects->effect(m_selectedCustomEffectIndex);

    setSelectedLightModeIndex(m_modes->customModeIndex());
    setAnimSettings(effect.animation, false);
    loadCustomLightModeKeysByBufferIndex(effect.keys, effect.order);
    loadSequenceByBufferIndex(effect.sequence);
}

void KeyboardConfiguratorController::saveAsNewCustomEffect(const QString &name)
{
    CustomEffect effect = currentEffectContent();
    effect.name = name;

    m_selectedCustomEffectIndex = m_customEffects->createEffect(effect);
    emit selectedCustomEffectIndexChanged();

    setCustomEffectModified(false);

    save();
}

void KeyboardConfiguratorController::saveSelectedCustomEffect()
{
    if (m_selectedCustomEffectIndex == -1) return;

    m_customEffects->setEffectContent(m_selectedCustomEffectIndex, currentEffectContent());

    setCustomEffectModified(false);

    save();
}

void KeyboardConfiguratorController::setCustomEffectName(const int &row, const QString &name)
{
    m_customEffects->setEffectName(row, name);
}

void KeyboardConfiguratorController::duplicateCustomEffect(const int &row, const QString &name)
{
    // Copy first, creating an effect can move the one being copied
    CustomEffect effect = m_customEffects->effect(row);
    effect.name = name;
    m_customEffects->createEffect(effect);
}

void KeyboardConfiguratorController::deleteCustomEffect(const int &row)
{
    m_customEffects->deleteEffect(row);

    if (m_selectedCustomEffectIndex == row)
    {
        // Keep the keys on screen, they just no longer belong to a saved effect
        m_selectedCustomEffectIndex = -1;
        emit selectedCustomEffectIndexChanged();
        setCustomEffectModified(false);
    }
    else if (m_selectedCustomEffectIndex > row)
    {
        m_selectedCustomEffectIndex--;
        emit selectedCustomEffectIndexChanged();
    }
}

void KeyboardConfiguratorController::setCustomEffectModified(const bool &customEffectModified)
{
    if (m_customEffectModified == customEffectModified) return;

    m_customEffectModified = customEffectModified;
    emit customEffectModifiedChanged();
}

void KeyboardConfiguratorController::markCustomLightModeKeysEdited()
{
    if (m_selectedCustomEffectIndex != -1)
    {
        setCustomEffectModified(true);
    }
}

QHash<int, QColor> KeyboardConfiguratorController::customLightModeKeysByBufferIndex()
{
    QHash<int, QColor> keys;

    for (auto&& keyIndex : m_customLightModeKeys->keyIndexes())
    {
        keys[selectedKeyboard().keys.at(keyIndex).bufferIndex] = m_customLightModeKeys->keys()[keyIndex];
    }

    return keys;
}

QList<int> KeyboardConfiguratorController::customLightModeKeysOrder()
{
    QList<int> order;

    for (auto&& keyIndex : m_customLightModeKeys->keyIndexes())
    {
        order << selectedKeyboard().keys.at(keyIndex).bufferIndex;
    }

    return order;
}

void KeyboardConfiguratorController::loadCustomLightModeKeysByBufferIndex(const QHash<int, QColor> &keys,
                                                                          const QList<int> &order)
{
    m_customLightModeKeys->clear();

    QHash<int, int> keyIndexByBufferIndex;
    for (int i = 0; i < selectedKeyboard().keys.size(); i++)
    {
        keyIndexByBufferIndex[selectedKeyboard().keys.at(i).bufferIndex] = i;
    }

    // Picked order first, then anything saved before the order was recorded
    for (auto&& bufferIndex : order)
    {
        if (keys.contains(bufferIndex) && keyIndexByBufferIndex.contains(bufferIndex))
        {
            m_customLightModeKeys->addIfNotExists(keyIndexByBufferIndex.value(bufferIndex), keys.value(bufferIndex));
        }
    }

    for (int i = 0; i < selectedKeyboard().keys.size(); i++)
    {
        const int bufferIndex = selectedKeyboard().keys.at(i).bufferIndex;

        if (keys.contains(bufferIndex))
        {
            m_customLightModeKeys->addIfNotExists(i, keys.value(bufferIndex));
        }
    }

    emit customLightModeKeysChanged();
}

void KeyboardConfiguratorController::setAnimEnabled(const bool &enabled)
{
    AnimationSettings anim = m_anim;
    anim.enabled = enabled;
    setAnimSettings(anim, true);
}

void KeyboardConfiguratorController::setAnimPattern(const int &pattern)
{
    AnimationSettings anim = m_anim;
    anim.pattern = pattern;
    setAnimSettings(anim, true);
}

void KeyboardConfiguratorController::setAnimStep(const int &stepMs)
{
    AnimationSettings anim = m_anim;
    anim.stepMs = stepMs;
    setAnimSettings(anim, true);
}

void KeyboardConfiguratorController::setAnimFade(const int &fadeMs)
{
    AnimationSettings anim = m_anim;
    anim.fadeMs = fadeMs;
    setAnimSettings(anim, true);
}

void KeyboardConfiguratorController::setAnimFadeIn(const int &fadeInMs)
{
    AnimationSettings anim = m_anim;
    anim.fadeInMs = fadeInMs;
    setAnimSettings(anim, true);
}

void KeyboardConfiguratorController::setAnimReverse(const bool &reverse)
{
    AnimationSettings anim = m_anim;
    anim.reverse = reverse;
    setAnimSettings(anim, true);
}

void KeyboardConfiguratorController::setAnimShuffle(const bool &shuffle)
{
    AnimationSettings anim = m_anim;
    anim.shuffle = shuffle;
    setAnimSettings(anim, true);
}

void KeyboardConfiguratorController::setAnimTrail(const int &trailKeys)
{
    AnimationSettings anim = m_anim;
    anim.trailKeys = trailKeys;
    setAnimSettings(anim, true);
}

void KeyboardConfiguratorController::setAnimCurve(const int &fadeCurve)
{
    AnimationSettings anim = m_anim;
    anim.fadeCurve = fadeCurve;
    setAnimSettings(anim, true);
}

void KeyboardConfiguratorController::setAnimBrightness(const int &brightness)
{
    AnimationSettings anim = m_anim;
    anim.brightness = brightness;
    setAnimSettings(anim, true);
}

void KeyboardConfiguratorController::setAnimLoop(const int &loop)
{
    AnimationSettings anim = m_anim;
    anim.loop = loop;
    setAnimSettings(anim, true);
}

void KeyboardConfiguratorController::setAnimPause(const int &pauseMs)
{
    AnimationSettings anim = m_anim;
    anim.pauseMs = pauseMs;
    setAnimSettings(anim, true);
}

void KeyboardConfiguratorController::setAnimBackground(const QString &name, const QVariant &value)
{
    AnimationSettings anim = m_anim;
    AnimationSettings::Background& b = anim.background;

    if (name == u"enabled"_s) b.enabled = value.toBool();
    else if (name == u"pattern"_s) b.pattern = value.toInt();
    else if (name == u"step"_s) b.stepMs = value.toInt();
    else if (name == u"fade"_s) b.fadeMs = value.toInt();
    else if (name == u"curve"_s) b.fadeCurve = value.toInt();
    else if (name == u"brightness"_s) b.brightness = value.toInt();
    else if (name == u"pause"_s) b.pauseMs = value.toInt();
    else if (name == u"reverse"_s) b.reverse = value.toBool();
    else return;

    setAnimSettings(anim, true);
}

void KeyboardConfiguratorController::stopPlayback()
{
    stopPlayback(true);
}

void KeyboardConfiguratorController::resumePlayback()
{
    if (!m_livePrepared || m_playing || !m_liveTimer) return;

    qInfo() << "Resume live lighting";

    m_liveFrameInFlight = false;
    m_liveFailures = 0;
    m_liveClock.restart();
    m_liveTimer->start();

    m_playing = true;
    emit playingChanged();
}

void KeyboardConfiguratorController::setAnimSettings(const AnimationSettings &anim, const bool &edited)
{
    m_anim = anim;
    emit animSettingsChanged();

    // Switching to a sequence starts from the keys already picked
    if (editingSequence() && m_sequence.isEmpty() && !m_customLightModeKeys->keyIndexes().isEmpty())
    {
        for (auto&& keyIndex : m_customLightModeKeys->keyIndexes()) m_sequence << QList<int>{keyIndex};
        emit sequenceChanged();
    }

    preparePreview();

    if (edited)
    {
        markCustomLightModeKeysEdited();
        setUnsavedChanges(true);
    }
}

void KeyboardConfiguratorController::undoSequenceStep()
{
    if (m_sequence.isEmpty()) return;

    QList<QList<int>> sequence = m_sequence;
    sequence.removeLast();
    setSequence(sequence, true);
}

void KeyboardConfiguratorController::addSequencePause()
{
    QList<QList<int>> sequence = m_sequence;
    sequence << QList<int>{};
    setSequence(sequence, true);
}

void KeyboardConfiguratorController::clearSequence()
{
    setSequence({}, true);
}

void KeyboardConfiguratorController::setSequence(const QList<QList<int>> &sequence, const bool &edited)
{
    m_sequence = sequence;

    // The picked keys follow the sequence, they are what stays lit when playback stops
    syncSelectionFromSequence();

    emit sequenceChanged();

    if (edited)
    {
        markCustomLightModeKeysEdited();
        setUnsavedChanges(true);
    }
}

void KeyboardConfiguratorController::toggleKeyInLastStep(const int &keyIndex)
{
    QList<QList<int>> sequence = m_sequence;

    // Nothing to join yet (or the last step is a pause): start a new step
    if (sequence.isEmpty() || sequence.last().isEmpty())
    {
        sequence << QList<int>{keyIndex};
    }
    else if (sequence.last().contains(keyIndex))
    {
        sequence.last().removeOne(keyIndex);
        if (sequence.last().isEmpty()) sequence.removeLast();
    }
    else
    {
        sequence.last() << keyIndex;
    }

    setSequence(sequence, true);
}

void KeyboardConfiguratorController::syncSelectionFromSequence()
{
    m_customLightModeKeys->clear();

    for (auto&& step : m_sequence)
    {
        for (auto&& keyIndex : step)
        {
            m_customLightModeKeys->addIfNotExists(keyIndex, QColor(Qt::white));
        }
    }

    emit customLightModeKeysChanged();
}

QList<QList<int>> KeyboardConfiguratorController::sequenceByBufferIndex()
{
    QList<QList<int>> sequence;

    for (auto&& step : m_sequence)
    {
        QList<int> bufferIndexes;
        for (auto&& keyIndex : step) bufferIndexes << selectedKeyboard().keys.at(keyIndex).bufferIndex;
        sequence << bufferIndexes;
    }

    return sequence;
}

void KeyboardConfiguratorController::loadSequenceByBufferIndex(const QList<QList<int>> &sequence)
{
    QHash<int, int> keyIndexByBufferIndex;
    for (int i = 0; i < selectedKeyboard().keys.size(); i++)
    {
        keyIndexByBufferIndex[selectedKeyboard().keys.at(i).bufferIndex] = i;
    }

    m_sequence.clear();

    for (auto&& step : sequence)
    {
        QList<int> keyIndexes;
        for (auto&& bufferIndex : step)
        {
            if (keyIndexByBufferIndex.contains(bufferIndex)) keyIndexes << keyIndexByBufferIndex.value(bufferIndex);
        }

        // Keep pauses, drop steps whose keys this keyboard doesn't have
        if (step.isEmpty() || !keyIndexes.isEmpty()) m_sequence << keyIndexes;
    }

    // Saved before sequences had steps: one step per picked key
    if (m_sequence.isEmpty())
    {
        for (auto&& keyIndex : m_customLightModeKeys->keyIndexes()) m_sequence << QList<int>{keyIndex};
    }

    emit sequenceChanged();
    preparePreview();
}

int KeyboardConfiguratorController::keyIndexForCode(const KeyCode::Code &code)
{
    if (code == KeyCode::Key_Invalid || m_selectedKeyboardIndex == -1) return -1;

    for (int i = 0; i < selectedKeyboard().keys.size(); i++)
    {
        if (selectedKeyboard().keys.at(i).keyCode == code) return i;
    }

    return -1;
}

CustomEffect KeyboardConfiguratorController::currentEffectContent()
{
    CustomEffect effect;

    effect.keys = customLightModeKeysByBufferIndex();
    effect.order = customLightModeKeysOrder();
    effect.sequence = sequenceByBufferIndex();
    effect.animation = m_anim;

    return effect;
}

void KeyboardConfiguratorController::preparePreview()
{
    if (m_selectedKeyboardIndex == -1) return;

    m_previewAnimation.prepare(selectedKeyboard().keys, m_customLightModeKeys->keyIndexes(), m_sequence, m_anim);
    m_previewClock.restart();
}

void KeyboardConfiguratorController::startPlayback()
{
    if (!m_liveTimer) return;

    qInfo() << "Start live lighting on" << selectedKeyboard().id;

    m_liveKeys = selectedKeyboard().keys;
    m_livePath = selectedKeyboard().path;
    m_liveAnimation.prepare(m_liveKeys, m_customLightModeKeys->keyIndexes(), m_sequence, m_anim);
    m_livePrepared = true;

    m_liveFrameInFlight = false;
    m_liveFailures = 0;
    m_liveClock.restart();
    m_liveTimer->start();

    if (!m_playing)
    {
        m_playing = true;
        emit playingChanged();
    }
}

void KeyboardConfiguratorController::stopPlayback(const bool &restoreLighting)
{
    m_startPlaybackAfterSave = false;

    if (!m_playing) return;

    qInfo() << "Stop live lighting";

    m_liveTimer->stop();
    m_playing = false;
    emit playingChanged();

    emit HIDConnection::getInstance().closeLiveConnection();

    if (restoreLighting)
    {
        unsigned char** buffers = takeRestoreBuffers();
        if (buffers)
        {
            emit HIDConnection::getInstance().sendData(m_livePath, buffers, 1);
        }
    }
}

void KeyboardConfiguratorController::sendLiveFrame()
{
    // Wait for the keyboard to take the previous frame instead of queueing more
    if (m_liveFrameInFlight) return;

    const qint64 ms = m_liveClock.elapsed();

    if (m_liveAnimation.finished(ms))
    {
        stopPlayback(true);
        return;
    }

    const QList<int> levels = m_liveAnimation.render(ms);

    // Live frames start with one byte before LED 0
    QByteArray frame(LiveFramePacketCount * (BufferSize - 3), 0);

    for (int i = 0; i < m_liveKeys.size(); i++)
    {
        const int position = m_liveKeys.at(i).bufferIndex + 1;

        if (position < frame.size())
        {
            frame[position] = static_cast<char>(levels.at(i));
        }
    }

    m_liveFrameInFlight = true;
    emit HIDConnection::getInstance().sendLiveFrame(m_livePath, frame);
}

unsigned char **KeyboardConfiguratorController::takeRestoreBuffers()
{
    if (m_liveRestoreBuffer.size() != BufferSize) return nullptr;

    // Re-sending the light mode makes the keyboard leave live lighting
    unsigned char** buffers = new unsigned char* [1];
    buffers[0] = new unsigned char[BufferSize];
    std::memcpy(buffers[0], m_liveRestoreBuffer.constData(), BufferSize);

    return buffers;
}

void KeyboardConfiguratorController::createAndSelectNewProfile(const QString &name)
{
    setSelectedProfileIndex(m_profiles->createNewProfile(name));
}

void KeyboardConfiguratorController::setProfileName(const int &row, const QString &name)
{
    m_profiles->setProfileName(row, name);
}

void KeyboardConfiguratorController::deleteProfile(const int &row)
{
    m_profiles->deleteProfile(row);

    if (m_selectedProfileIndex >= row)
    {
        m_selectedProfileIndex--;
        emit selectedProfileIndexChanged();
    }
}

void KeyboardConfiguratorController::setKeyCodeModelFilter(const QString &filter)
{
    m_keyCodesProxy->setFilterFixedString(filter);
}

QByteArray KeyboardConfiguratorController::getStandardLightBuffer()
{
    QByteArray buffer(BufferSize, 0x00);

    buffer[0] = 0x0a;
    buffer[1] = 0x01;
    buffer[2] = 0x01;
    buffer[3] = 0x02;
    buffer[4] = 0x29;
    buffer[5] = m_modes->getModeBit(m_selectedLightModeIndex);
    buffer[7] = m_animation;
    buffer[8] = m_brightness;

    if (! m_randomColours)
    {
        buffer[9] = m_selectedColour.red();
        buffer[10] = m_selectedColour.green();
        buffer[11] = m_selectedColour.blue();
    }

    buffer[12] = m_randomColours ? 0x01 : 0x00;
    buffer[13] = m_sleep;

    return buffer;
}

void KeyboardConfiguratorController::setStandardLightBufferBetter(unsigned char* buffer)
{
    std::fill_n(buffer, BufferSize, 0x00);

    buffer[0] = 0x0a;
    buffer[1] = 0x01;
    buffer[2] = 0x01;
    buffer[3] = 0x02;
    buffer[4] = 0x29;
    buffer[5] = m_modes->getModeBit(m_selectedLightModeIndex);
    buffer[7] = m_animation;
    buffer[8] = m_brightness;

    if (! m_randomColours)
    {
        buffer[9] = m_selectedColour.red();
        buffer[10] = m_selectedColour.green();
        buffer[11] = m_selectedColour.blue();
    }

    buffer[12] = m_randomColours ? 0x01 : 0x00;
    buffer[13] = m_sleep;
}

void KeyboardConfiguratorController::setBufferKey(unsigned char *buffer, const int &keyCode)
{
    if (keyCode >= 0x01000000)
    {
        buffer[0] = (keyCode >> 24) & 0xff;
        buffer[1] = (keyCode >> 16) & 0xff;
        buffer[2] = (keyCode >> 8) & 0xff;
        buffer[3] = (keyCode) & 0xff;
    }
    else if (keyCode >= 0x010000)
    {
        buffer[0] = 0x00;
        buffer[1] = (keyCode >> 16) & 0xff;
        buffer[2] = (keyCode >> 8) & 0xff;
        buffer[3] = (keyCode) & 0xff;
    }
    else if (keyCode >= 0x0100)
    {
        buffer[0] = 0x00;
        buffer[1] = 0x00;
        buffer[2] = (keyCode >> 8) & 0xff;
        buffer[3] = (keyCode) & 0xff;
    }
    else if (keyCode >= 0x10)
    {
        buffer[0] = 0x00;
        buffer[1] = 0x00;
        buffer[2] = 0x00;
        buffer[3] = (keyCode) & 0xff;
    }
}

void KeyboardConfiguratorController::saveProfile()
{
    qInfo() << "Save profile" << profiles()->profileName(selectedProfileIndex());;

    QSettings settings;

    settings.beginGroup(QStringLiteral("profiles/%1").arg(getProfileID()));

    settings.beginGroup(u"custom_colours"_s);

    for (int i = 0; i < m_customColours->defaultRowCount(); i++)
    {
        settings.setValue(QString::number(i), m_customColours->colours().value(i));
    }

    settings.endGroup();

    settings.beginGroup(QStringLiteral("%1:%2")
                        .arg(QString::number(selectedKeyboard().id.vid),
                             QString::number(selectedKeyboard().id.pid)));

    qDebug() << "Brightness:" << m_brightness;
    qDebug() << "Animation:" << m_animation;
    qDebug() << "Sleep:" << m_sleep;
    qDebug() << "Mode:" << m_selectedLightModeIndex;
    qDebug() << "Random Colour:" << m_randomColours;
    qDebug() << "Selected Colour:" << m_selectedColour;

    settings.setValue(u"brightness"_s, m_brightness);
    settings.setValue(u"animation"_s, m_animation);
    settings.setValue(u"sleep"_s, m_sleep);
    settings.setValue(u"selected_light_mode_index"_s, m_selectedLightModeIndex);
    settings.setValue(u"random_colours"_s, m_randomColours);
    settings.setValue(u"selected_colour"_s, m_selectedColour);

    if (m_selectedCustomEffectIndex == -1)
    {
        settings.remove(u"selected_custom_effect"_s);
    }
    else
    {
        settings.setValue(u"selected_custom_effect"_s,
                          m_customEffects->effect(m_selectedCustomEffectIndex).id);
    }

    settings.remove(u"custom_light_mode_keys"_s);

    settings.beginGroup(u"custom_light_mode_keys"_s);

    for (int i = 0; i < m_customLightModeKeys->keyIndexes().size(); i++)
    {
        auto keyIndex = m_customLightModeKeys->keyIndexes().at(i);
        settings.setValue(QString::number(selectedKeyboard().keys.at(keyIndex).bufferIndex),
                          m_customLightModeKeys->keys()[keyIndex]);
    }

    settings.endGroup();

    QStringList customLightModeOrder;
    for (auto&& bufferIndex : customLightModeKeysOrder())
    {
        customLightModeOrder << QString::number(bufferIndex);
    }
    settings.setValue(u"custom_light_mode_order"_s, customLightModeOrder);

    settings.setValue(u"custom_sequence"_s, LightAnimation::sequenceToStrings(sequenceByBufferIndex()));
    m_anim.write(settings);

    settings.remove(u"custom_mapped_keys"_s);

    settings.beginGroup(u"custom_mapped_keys"_s);

    for (int i = 0; i < m_customMappedKeys->keyIndexes().size(); i++)
    {
        auto index = m_customMappedKeys->keyIndexes().at(i);

        settings.beginWriteArray(QString::number(selectedKeyboard().keys.at(index).bufferIndex));

        const QList<KeyCode::Code> keyCodes = m_customMappedKeys->keys()[index];

        for (int j = 0; j < keyCodes.size(); j++)
        {
            settings.setArrayIndex(j);
            settings.setValue(u"key_code"_s, QString::number(keyCodes.at(j)));
        }

        settings.endArray();
    }

    settings.endGroup();

    settings.endGroup();

    settings.endGroup();
}

void KeyboardConfiguratorController::saveToKeyboard()
{
    qInfo() << "Save to keyboard" << selectedKeyboard().id;

    setSavingChanges(true);

    // Live frames mixed in with a save make the keyboard refuse reports
    if (m_playing)
    {
        m_liveTimer->stop();
    }

    unsigned char** buffers = new unsigned char* [1 + CustomLightModeBuffersSize
            + MacroModel::AreaPackets + KeyMapBuffersSize];
    int buffersIndex = 0;

    unsigned char* buffer = new unsigned char[BufferSize];
    setStandardLightBufferBetter(buffer);
    buffers[buffersIndex++] = buffer;

    if (isCustomLightModeSelected())
    {
        unsigned char ledFullBuffer[CustomLightModeBuffersSize * BufferSize];
        std::fill_n(ledFullBuffer, CustomLightModeBuffersSize * BufferSize, 0x00);

        for (int i = 0; i < m_customLightModeKeys->keyIndexes().size(); i++)
        {
            auto keyIndex = m_customLightModeKeys->keyIndexes().at(i);
            const Key& key = selectedKeyboard().keys.at(keyIndex);
            const QColor& colour = m_customLightModeKeys->keys()[keyIndex];

            if (selectedKeyboard().rgb)
            {
                int lbi = key.bufferIndex * 3;
                ledFullBuffer[lbi] = colour.red();
                ledFullBuffer[lbi + 1] = colour.green();
                ledFullBuffer[lbi + 2] = colour.blue();
            }
            else
            {
                // Single colour keyboards use one byte (on/off) per LED
                ledFullBuffer[key.bufferIndex] = 0xff;
            }
        }

        int ledBufferIndex = 0;

        for (int i = 0; i < CustomLightModeBuffersSize; i++)
        {
            unsigned char* buffer = new unsigned char[BufferSize];
            std::fill_n(buffer, BufferSize, 0x00);

            buffer[0] = 0x0a;
            buffer[1] = CustomLightModeBuffersSize;
            buffer[2] = i + 1;

            if (i == 0)
            {
                buffer[3] = 0x03;
                buffer[4] = 0x7e;
                buffer[5] = 0x01;
            }

            for (int bufferIndex = ((i == 0) ? 6 : 3 ); bufferIndex < BufferSize; bufferIndex++)
            {
                buffer[bufferIndex] = ledFullBuffer[ledBufferIndex];
                ledBufferIndex++;
            }

            buffers[buffersIndex++] = buffer;
        }
    }

    // Macros go before the key map that points at them, and only when they
    // changed, so the keyboard's flash isn't rewritten on every apply
    if (selectedKeyboard().macrosEnabled && m_macros->areaChanged())
    {
        qInfo() << "Macros changed, send them too";

        const QByteArray area = m_macros->area();

        for (int i = 0; i < MacroModel::AreaPackets; i++)
        {
            unsigned char* buffer = new unsigned char[BufferSize];
            std::fill_n(buffer, BufferSize, 0x00);

            buffer[0] = 0x0a;
            buffer[1] = MacroModel::AreaPackets;
            buffer[2] = i + 1;
            std::copy_n(area.constData() + i * 62, 62, buffer + 3);

            buffers[buffersIndex++] = buffer;
        }

        m_macroAreaPending = true;
    }

    if (selectedKeyboard().keyMapEnabled)
    {
        unsigned char mapFullBuffer[KeyMapBuffersSize * BufferSize];
        std::fill_n(mapFullBuffer, KeyMapBuffersSize * BufferSize, 0x00);

        for (int i = 0; i < selectedKeyboard().keys.size(); i++)
        {
            setBufferKey(mapFullBuffer + (selectedKeyboard().keys.at(i).bufferIndex * 4),
                         selectedKeyboard().keys.at(i).keyCode);
        }

        for (int i = 0; i < m_customMappedKeys->keyIndexes().size(); i++)
        {
            int keyIndex = m_customMappedKeys->keyIndexes().at(i);

            int bufferIndex = selectedKeyboard().keys.at(keyIndex).bufferIndex;

            int finalKey = 0;

            for (auto&& eachMappedKey : m_customMappedKeys->keys()[keyIndex])
            {
                finalKey |= eachMappedKey;
            }

            // Leave the key's default in place rather than point it at a macro slot
            // on a model whose macro format isn't known
            if (!selectedKeyboard().macrosEnabled && MacroModel::isMacroCode(finalKey)) continue;

            setBufferKey((mapFullBuffer + (bufferIndex * 4)), finalKey);
        }

        int mapBufferIndex = 0;

        for (int i = 0; i < KeyMapBuffersSize; i++)
        {
            unsigned char* buffer = new unsigned char[BufferSize];
            std::fill_n(buffer, BufferSize, 0x00);

            buffer[0] = 0x0a;
            buffer[1] = KeyMapBuffersSize;
            buffer[2] = i + 1;

            if (i == 0)
            {
                buffer[3] = 0x01;
                buffer[4] = 0xf8;
            }

            for (int bufferIndex = ((i == 0) ? 5 : 3 ); bufferIndex < BufferSize; bufferIndex++)
            {
                buffer[bufferIndex] = mapFullBuffer[mapBufferIndex];
                mapBufferIndex++;
            }

            buffers[buffersIndex++] = buffer;
        }
    }

    emit HIDConnection::getInstance().sendData(selectedKeyboard().path, buffers, buffersIndex);

    // Kept so live lighting can hand the keyboard back to these settings
    unsigned char lightBuffer[BufferSize];
    setStandardLightBufferBetter(lightBuffer);
    m_liveRestoreBuffer = QByteArray(reinterpret_cast<const char*>(lightBuffer), BufferSize);

    // The keys just written double as the still version shown when playback stops.
    // Playback (re)starts once the keyboard has taken the save.
    if (isCustomLightModeSelected() && m_anim.enabled && animationSupported())
    {
        m_startPlaybackAfterSave = true;
    }
    else
    {
        m_startPlaybackAfterSave = false;
        stopPlayback(false);

        // The keyboard has new still lighting, the old animation no longer applies
        m_livePrepared = false;
        emit playingChanged();
    }

    setUnsavedChanges(false);
}

void KeyboardConfiguratorController::loadProfileSettings()
{
    qInfo() << "Load settings from profile" << profiles()->profileName(selectedProfileIndex());

    QSettings settings;

    settings.beginGroup(QStringLiteral("profiles/%1/%2:%3")
                        .arg(getProfileID(),
                             QString::number(selectedKeyboard().id.vid),
                             QString::number(selectedKeyboard().id.pid)));

    setBrightness(settings.value(u"brightness"_s, 5).toUInt());
    setAnimation(settings.value(u"animation"_s, 3).toUInt());
    setSleep(settings.value(u"sleep"_s, 5).toUInt());
    setSelectedLightModeIndex(settings.value(u"selected_light_mode_index"_s, m_modes->neonStreamIndex()).toUInt());

    setRandomColours(settings.value(u"random_colours"_s, true).toBool());
    setSelectedColour(settings.value(u"selected_colour"_s, QColor(Qt::white)).value<QColor>());

    QHash<int, QColor> customLightModeKeys;
    settings.beginGroup(u"custom_light_mode_keys"_s);
    for (auto&& bufferIndex : settings.childKeys())
    {
        customLightModeKeys[bufferIndex.toInt()] = settings.value(bufferIndex).value<QColor>();
    }
    settings.endGroup();

    QList<int> customLightModeOrder;
    for (auto&& bufferIndex : settings.value(u"custom_light_mode_order"_s).toStringList())
    {
        customLightModeOrder << bufferIndex.toInt();
    }

    setAnimSettings(AnimationSettings::read(settings), false);
    loadCustomLightModeKeysByBufferIndex(customLightModeKeys, customLightModeOrder);
    loadSequenceByBufferIndex(LightAnimation::sequenceFromStrings(
                                  settings.value(u"custom_sequence"_s).toStringList()));

    settings.beginGroup(u"custom_mapped_keys"_s);
    m_customMappedKeys->clearAll();
    QStringList customMappedKeysBufferIndexes = settings.childGroups();
    for (auto&& bufferIndex : customMappedKeysBufferIndexes)
    {
        int size = settings.beginReadArray(bufferIndex);

        QList<KeyCode::Code> codes;

        for (int i = 0; i < size; i++)
        {
            settings.setArrayIndex(i);

            codes << static_cast<KeyCode::Code>(settings.value(u"key_code"_s).toInt());
        }

        settings.endArray();

        for(int i = 0; i < selectedKeyboard().keys.size(); i++)
        {
            const Key& key = selectedKeyboard().keys.at(i);

            if (bufferIndex == QString::number(key.bufferIndex))
            {
                m_customMappedKeys->addKeys(i, codes);
                break;
            }
        }
    }
    settings.endGroup();

    // A saved effect wins over the profile's own copy of the custom keys,
    // so edits to the effect show up in every profile using it
    const int effectIndex = m_customEffects->indexOf(settings.value(u"selected_custom_effect"_s).toString());

    m_selectedCustomEffectIndex = isCustomLightModeSelected() ? effectIndex : -1;
    emit selectedCustomEffectIndexChanged();
    setCustomEffectModified(false);

    if (m_selectedCustomEffectIndex != -1)
    {
        const CustomEffect& effect = m_customEffects->effect(m_selectedCustomEffectIndex);

        setAnimSettings(effect.animation, false);
        loadCustomLightModeKeysByBufferIndex(effect.keys, effect.order);
        loadSequenceByBufferIndex(effect.sequence);
    }

    settings.endGroup();
}

void KeyboardConfiguratorController::loadProfileCustomColours()
{
    qInfo() << "Load colours from profile" << profiles()->profileName(selectedProfileIndex());

    QSettings settings;

    QList<QColor> newColours;

    settings.beginGroup(QStringLiteral("profiles/%1").arg(getProfileID()));

    settings.beginGroup(u"custom_colours"_s);

    for (int i = 0; i < m_customColours->defaultRowCount(); i++)
    {
        newColours << settings.value(QString::number(i), QColor(Qt::white)).value<QColor>();
    }

    m_customColours->setColours(std::move(newColours));

    settings.endGroup();

    settings.endGroup();
}

QString KeyboardConfiguratorController::getProfileID()
{
    return m_profiles->data(m_profiles->index(m_selectedProfileIndex, 0), ProfileModel::ProfileModelRoles::IDRole).toString();
}

int KeyboardConfiguratorController::getKeyIndexAtLocation(const int &x, const int &y)
{
    for(int i = 0; i < selectedKeyboard().keys.size(); i++)
    {
        const Key& key = selectedKeyboard().keys.at(i);

        if (x >= key.topX && y >= key.topY
                && x <= key.bottomX && y <= key.bottomY)
        {
            return i;
        }
    }

    return -1;
}

const Keyboard &KeyboardConfiguratorController::selectedKeyboard()
{
    return m_connectedKeyboards->keyboards().at(m_selectedKeyboardIndex);
}

