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

#ifndef KEYBOARDCONFIGURATORCONTROLLER_H
#define KEYBOARDCONFIGURATORCONTROLLER_H

#include <QObject>
#include <QQmlEngine>
#include <QPointer>
#include <QSettings>
#include <QColor>
#include <QSortFilterProxyModel>

#include "customlightmodekeys.h"
#include "modemodel.h"
#include "key.h"
#include "keyboard.h"
#include "keyboardmodel.h"
#include "profilemodel.h"
#include "customcolours.h"
#include "keycodemodel.h"
#include "keycodemodelproxy.h"
#include "custommappedkeys.h"


class KeyboardConfiguratorController : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(bool randomColours READ randomColours WRITE setRandomColours NOTIFY randomColoursChanged)
    Q_PROPERTY(QColor selectedColour READ selectedColour WRITE setSelectedColour NOTIFY selectedColourChanged)
    Q_PROPERTY(CustomColours* customColours READ customColours CONSTANT)
    Q_PROPERTY(CustomLightModeKeys* customLightModeKeys READ customLightModeKeys CONSTANT)
    Q_PROPERTY(bool unsavedChanges READ unsavedChanges WRITE setUnsavedChanges NOTIFY unsavedChangesChanged)
    Q_PROPERTY(bool savingChanges READ savingChanges WRITE setSavingChanges NOTIFY savingChangesChanged)
    Q_PROPERTY(bool keyMapEnabled READ keyMapEnabled NOTIFY keyMapEnabledChanged)

    Q_PROPERTY(int brightness READ brightness WRITE setBrightness NOTIFY brightnessChanged)
    Q_PROPERTY(int animation READ animation WRITE setAnimation NOTIFY animationChanged)
    Q_PROPERTY(int sleep READ sleep WRITE setSleep NOTIFY sleepChanged)

    Q_PROPERTY(ModeModel* modes READ modes CONSTANT)
    Q_PROPERTY(KeyboardConfiguratorController::Mode selectedMode READ selectedMode WRITE setSelectedMode NOTIFY selectedModeChanged)
    Q_PROPERTY(int selectedLightModeIndex READ selectedLightModeIndex WRITE setSelectedLightModeIndex NOTIFY selectedLightModeIndexChanged)
    Q_PROPERTY(bool isCustomLightModeSelected READ isCustomLightModeSelected NOTIFY isCustomLightModeSelectedChanged)
    Q_PROPERTY(bool showColoursColumn READ showColoursColumn NOTIFY showColoursColumnChanged)

    Q_PROPERTY(ProfileModel* profiles READ profiles CONSTANT)
    Q_PROPERTY(int selectedProfileIndex READ selectedProfileIndex WRITE setSelectedProfileIndex NOTIFY selectedProfileIndexChanged)

    Q_PROPERTY(QSortFilterProxyModel* keyCodesProxy READ keyCodesProxy CONSTANT)

    Q_PROPERTY(CustomMappedKeys* customMappedKeys READ customMappedKeys CONSTANT)

    Q_PROPERTY(int topLeftX READ topLeftX CONSTANT)
    Q_PROPERTY(int topLeftY READ topLeftY CONSTANT)
    Q_PROPERTY(int bottomRightX READ bottomRightX CONSTANT)
    Q_PROPERTY(int bottomRightY READ bottomRightY CONSTANT)

public:

    enum Mode {
        Light,
        KeyMap
    };

    Q_ENUM(Mode)

    explicit KeyboardConfiguratorController(QObject *parent = nullptr);    

    static constexpr int BufferSize = 65;

    const bool& randomColours();
    const QColor& selectedColour();
    CustomColours* customColours();
    CustomLightModeKeys* customLightModeKeys();
    const bool& unsavedChanges();
    const bool& savingChanges();
    bool keyMapEnabled();

    const int& brightness();
    const int& animation();
    const int& sleep();

    ModeModel* modes();

    const KeyboardConfiguratorController::Mode& selectedMode();
    const int& selectedLightModeIndex();
    bool isCustomLightModeSelected();
    bool showColoursColumn();

    ProfileModel* profiles();
    const int& selectedProfileIndex();

    QSortFilterProxyModel* keyCodesProxy();
    CustomMappedKeys* customMappedKeys();

    const int& topLeftX();
    const int& topLeftY();
    const int& bottomRightX();
    const int& bottomRightY();

    Q_INVOKABLE Key getKeyByCustomLightModeKeysIndex(int index);
    Q_INVOKABLE Key getKeyByCustomMappedKeysIndex(int index);

signals:
    void loadName(const QString& name);
    void loadImage(const QString& imagePath);
    void randomColoursChanged();
    void selectedColourChanged();
    void customLightModeKeysChanged();
    void unsavedChangesChanged();
    void savingChangesChanged();
    void keyMapEnabledChanged();
    void brightnessChanged();
    void animationChanged();
    void sleepChanged();
    void selectedModeChanged();
    void selectedLightModeIndexChanged();
    void isCustomLightModeSelectedChanged();
    void showColoursColumnChanged();
    void selectedProfileIndexChanged();
    void openConfigureKeyMapPopup();
    void requestKbCanvasPaint();

public slots:
    void init();
    void load(const int& keyboardIndex);
    void applyDefaultProfile();
    void save();
    void reset();
    void setRandomColours(const bool& randomColours);
    void setSelectedColour(const QColor& colour);
    void setUnsavedChanges(const bool& unsavedChanges);
    void setSavingChanges(const bool& savingChanges);
    void setBrightness(const int& brightness);
    void setAnimation(const int& animation);
    void setSleep(const int& sleep);
    void setSelectedMode(const KeyboardConfiguratorController::Mode& selectedMode);
    void setSelectedLightModeIndex(const int& selectedLightModeIndex);
    void setSelectedProfileIndex(const int& selectedProfileIndex);
    void clickKey(const int& x, const int& y);
    void selectAllKeysForCustomMode();
    void unselectAllKeysForCustomMode();

    void createAndSelectNewProfile(const QString& name);
    void setProfileName(const int& row, const QString& name);
    void deleteProfile(const int& row);

    void setKeyCodeModelFilter(const QString& filter);

private:
    QPointer<KeyboardModel> m_connectedKeyboards;
    int m_selectedKeyboardIndex;

    bool m_randomColours;
    QColor m_selectedColour;
    QPointer<CustomColours> m_customColours;
    QPointer<CustomLightModeKeys> m_customLightModeKeys;
    bool m_unsavedChanges;
    bool m_savingChanges;

    KeyboardConfiguratorController::Mode m_selectedMode;
    int m_selectedLightModeIndex;
    QPointer<ModeModel> m_modes;

    QPointer<ProfileModel> m_profiles;
    int m_selectedProfileIndex;

    int m_brightness;
    int m_animation;
    int m_sleep;

    QPointer<KeyCodeModel> m_keyCodes;
    QPointer<KeyCodeModelProxy> m_keyCodesProxy;
    QPointer<CustomMappedKeys> m_customMappedKeys;

    static constexpr int KeyMapBuffersSize = 9;
    static constexpr int CustomLightModeBuffersSize = 7;

    QByteArray getStandardLightBuffer();
    void setStandardLightBufferBetter(unsigned char* buffer);

    void setBufferKey(unsigned char* buffer, const int& keyCode);

    void saveProfile();
    void saveToKeyboard();

    void loadProfileSettings();
    void loadProfileCustomColours();

    QString getProfileID();

    int getKeyIndexAtLocation(const int& x, const int& y);

    const Keyboard& selectedKeyboard();

};

#endif // KEYBOARDCONFIGURATORCONTROLLER_H
