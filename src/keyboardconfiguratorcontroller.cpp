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

using namespace Qt::Literals::StringLiterals;

KeyboardConfiguratorController::KeyboardConfiguratorController(QObject *parent)
    : QObject{parent}, m_selectedKeyboardIndex{-1}, m_randomColours{true},
      m_customColours{new CustomColours{this}},
      m_customLightModeKeys{new CustomLightModeKeys{this}},
      m_unsavedChanges{false}, m_savingChanges{false},
      m_selectedMode{Mode::Light}, m_selectedLightModeIndex{0},
      m_modes{new ModeModel{this}}, m_selectedProfileIndex{-1},
      m_brightness{5}, m_animation{5}, m_sleep{5},
      m_keyCodes{new KeyCodeModel{this}},
      m_keyCodesProxy{new KeyCodeModelProxy{this}},
      m_customMappedKeys{new CustomMappedKeys{this}}
{
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
    });

    connect(&connection, &HIDConnection::failedToSendData, this, [this](const QString& message){
        setSavingChanges(false);

        MainWindowController::showEnhancedDialog(this, EnhancedDialog(
                tr("Error"),
                message));
    });
}

void KeyboardConfiguratorController::load(const int &keyboardIndex)
{
    m_selectedKeyboardIndex = keyboardIndex;

    emit loadName(selectedKeyboard().name);
    emit loadImage(selectedKeyboard().imagePath);

    emit keyMapEnabledChanged();

    m_modes->setRGB(selectedKeyboard().rgb);
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

    loadProfileCustomColours();
    loadProfileSettings();

    saveToKeyboard();
}

void KeyboardConfiguratorController::clickKey(const int &x, const int &y)
{
    int index = getKeyIndexAtLocation(x, y);
    if (index == -1) return;

    if (m_selectedMode == Mode::Light)
    {
        if (selectedKeyboard().rgb)
        {
            m_customLightModeKeys->toggle(index, m_selectedColour);
        }
        else
        {
            m_customLightModeKeys->toggle(index);
        }

        setUnsavedChanges(true);
        emit customLightModeKeysChanged();
    }
    else if (selectedKeyboard().keyMapEnabled)
    {
        m_customMappedKeys->setSelectedKeyIndex(index);
        emit openConfigureKeyMapPopup();
    }
}

void KeyboardConfiguratorController::selectAllKeysForCustomMode()
{
    for (int i = 0; i < selectedKeyboard().keys.size(); i++)
    {
        m_customLightModeKeys->addIfNotExists(i, m_selectedColour);
    }

    setUnsavedChanges(true);
    emit customLightModeKeysChanged();
}

void KeyboardConfiguratorController::unselectAllKeysForCustomMode()
{
    m_customLightModeKeys->clear();

    setUnsavedChanges(true);
    emit customLightModeKeysChanged();
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
    QSettings settings;

    settings.beginGroup(QStringLiteral("profiles/%1").arg(getProfileID()));

    settings.beginGroup(u"custom_colours"_s);

    for (int i = 0; i < m_customColours->defaultRowCount(); i++)
    {
        settings.setValue(QString::number(i), m_customColours->colours().value(i));
    }

    settings.endGroup();

    settings.beginGroup(QString::number(selectedKeyboard().pid));

    settings.setValue(u"brightness"_s, m_brightness);
    settings.setValue(u"animation"_s, m_animation);
    settings.setValue(u"sleep"_s, m_sleep);
    settings.setValue(u"mode"_s, m_selectedLightModeIndex);

    settings.setValue(u"random_colour"_s, m_randomColours);
    settings.setValue(u"selected_colour"_s, m_selectedColour);

    settings.remove(u"custom_light_mode_keys"_s);

    settings.beginGroup(u"custom_light_mode_keys"_s);

    for (int i = 0; i < m_customLightModeKeys->keyIndexes().size(); i++)
    {
        auto keyIndex = m_customLightModeKeys->keyIndexes().at(i);
        settings.setValue(QString::number(selectedKeyboard().keys.at(keyIndex).bufferIndex),
                          m_customLightModeKeys->keys()[keyIndex]);
    }

    settings.endGroup();

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
    setSavingChanges(true);

    unsigned char** buffers = new unsigned char* [1 + CustomLightModeBuffersSize + KeyMapBuffersSize];
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

            int lbi = key.bufferIndex * 3;
            ledFullBuffer[lbi] = colour.red();
            ledFullBuffer[lbi + 1] = colour.green();
            ledFullBuffer[lbi + 2] = colour.blue();
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

    setUnsavedChanges(false);
}

void KeyboardConfiguratorController::loadProfileSettings()
{
    QSettings settings;

    settings.beginGroup(QStringLiteral("profiles/%1/%2").arg(getProfileID(), QString::number(selectedKeyboard().pid)));

    setBrightness(settings.value(u"brightness"_s, 5).toUInt());
    setAnimation(settings.value(u"animation"_s, 3).toUInt());
    setSleep(settings.value(u"sleep"_s, 5).toUInt());
    setSelectedLightModeIndex(settings.value(u"mode"_s, m_modes->neonStreamIndex()).toUInt());

    setRandomColours(settings.value(u"random_colour"_s, true).toBool());
    setSelectedColour(settings.value(u"selected_colour"_s, QColor(Qt::white)).value<QColor>());

    settings.beginGroup(u"custom_light_mode_keys"_s);
    m_customLightModeKeys->clear();
    QStringList customLightModeKeysBufferIndexes = settings.childKeys();
    for (auto&& bufferIndex : customLightModeKeysBufferIndexes)
    {
        for(int i = 0; i < selectedKeyboard().keys.size(); i++)
        {
            const Key& key = selectedKeyboard().keys.at(i);

            if (bufferIndex == QString::number(key.bufferIndex))
            {
                m_customLightModeKeys->add(i, settings.value(bufferIndex).value<QColor>());
                break;
            }
        }
    }
    emit customLightModeKeysChanged();
    settings.endGroup();

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

    settings.endGroup();
}

void KeyboardConfiguratorController::loadProfileCustomColours()
{
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

