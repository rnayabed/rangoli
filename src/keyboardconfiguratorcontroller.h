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
#include <QTimer>
#include <QElapsedTimer>

#include "customlightmodekeys.h"
#include "customeffectmodel.h"
#include "macromodel.h"
#include "lightanimation.h"
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
    Q_PROPERTY(bool macrosSupported READ macrosSupported NOTIFY keyMapEnabledChanged)

    Q_PROPERTY(int brightness READ brightness WRITE setBrightness NOTIFY brightnessChanged)
    Q_PROPERTY(int animation READ animation WRITE setAnimation NOTIFY animationChanged)
    Q_PROPERTY(int sleep READ sleep WRITE setSleep NOTIFY sleepChanged)

    Q_PROPERTY(ModeModel* modes READ modes CONSTANT)
    Q_PROPERTY(KeyboardConfiguratorController::Mode selectedMode READ selectedMode WRITE setSelectedMode NOTIFY selectedModeChanged)
    Q_PROPERTY(int selectedLightModeIndex READ selectedLightModeIndex WRITE setSelectedLightModeIndex NOTIFY selectedLightModeIndexChanged)
    Q_PROPERTY(bool isCustomLightModeSelected READ isCustomLightModeSelected NOTIFY isCustomLightModeSelectedChanged)
    Q_PROPERTY(bool showColoursColumn READ showColoursColumn NOTIFY showColoursColumnChanged)

    Q_PROPERTY(CustomEffectModel* customEffects READ customEffects CONSTANT)
    Q_PROPERTY(int selectedCustomEffectIndex READ selectedCustomEffectIndex WRITE setSelectedCustomEffectIndex NOTIFY selectedCustomEffectIndexChanged)
    Q_PROPERTY(bool customEffectModified READ customEffectModified NOTIFY customEffectModifiedChanged)

    Q_PROPERTY(bool animationSupported READ animationSupported NOTIFY animationSupportedChanged)
    Q_PROPERTY(bool animEnabled READ animEnabled WRITE setAnimEnabled NOTIFY animSettingsChanged)
    Q_PROPERTY(int animPattern READ animPattern WRITE setAnimPattern NOTIFY animSettingsChanged)
    Q_PROPERTY(int animStep READ animStep WRITE setAnimStep NOTIFY animSettingsChanged)
    Q_PROPERTY(int animFade READ animFade WRITE setAnimFade NOTIFY animSettingsChanged)
    Q_PROPERTY(int animFadeIn READ animFadeIn WRITE setAnimFadeIn NOTIFY animSettingsChanged)
    Q_PROPERTY(bool animReverse READ animReverse WRITE setAnimReverse NOTIFY animSettingsChanged)
    Q_PROPERTY(bool animShuffle READ animShuffle WRITE setAnimShuffle NOTIFY animSettingsChanged)
    Q_PROPERTY(int animTrail READ animTrail WRITE setAnimTrail NOTIFY animSettingsChanged)
    Q_PROPERTY(int animCurve READ animCurve WRITE setAnimCurve NOTIFY animSettingsChanged)
    Q_PROPERTY(int animEffectiveFade READ animEffectiveFade NOTIFY animSettingsChanged)
    Q_PROPERTY(bool animHasDirection READ animHasDirection NOTIFY animSettingsChanged)
    Q_PROPERTY(int animBrightness READ animBrightness WRITE setAnimBrightness NOTIFY animSettingsChanged)
    Q_PROPERTY(int animLoop READ animLoop WRITE setAnimLoop NOTIFY animSettingsChanged)
    Q_PROPERTY(int animPause READ animPause WRITE setAnimPause NOTIFY animSettingsChanged)
    Q_PROPERTY(QVariantMap animBackground READ animBackground NOTIFY animSettingsChanged)
    Q_PROPERTY(bool playing READ playing NOTIFY playingChanged)
    Q_PROPERTY(bool canResumePlayback READ canResumePlayback NOTIFY playingChanged)

    Q_PROPERTY(bool editingSequence READ editingSequence NOTIFY animSettingsChanged)
    Q_PROPERTY(QString sequenceText READ sequenceText NOTIFY sequenceChanged)
    Q_PROPERTY(int sequenceLength READ sequenceLength NOTIFY sequenceChanged)

    Q_PROPERTY(ProfileModel* profiles READ profiles CONSTANT)
    Q_PROPERTY(int selectedProfileIndex READ selectedProfileIndex WRITE setSelectedProfileIndex NOTIFY selectedProfileIndexChanged)

    Q_PROPERTY(QSortFilterProxyModel* keyCodesProxy READ keyCodesProxy CONSTANT)

    Q_PROPERTY(CustomMappedKeys* customMappedKeys READ customMappedKeys CONSTANT)

    Q_PROPERTY(MacroModel* macros READ macros CONSTANT)
    Q_PROPERTY(int editingMacroSlot READ editingMacroSlot NOTIFY macroEditorChanged)
    Q_PROPERTY(QString editingMacroName READ editingMacroName WRITE setEditingMacroName NOTIFY macroEditorChanged)
    Q_PROPERTY(QVariantList editingMacroEvents READ editingMacroEvents NOTIFY macroEditorChanged)
    Q_PROPERTY(bool editingMacroModified READ editingMacroModified NOTIFY macroEditorChanged)

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
    static constexpr int LiveFramePacketCount = 3;

    const bool& randomColours();
    const QColor& selectedColour();
    CustomColours* customColours();
    CustomLightModeKeys* customLightModeKeys();
    const bool& unsavedChanges();
    const bool& savingChanges();
    bool keyMapEnabled();
    bool macrosSupported();

    const int& brightness();
    const int& animation();
    const int& sleep();

    ModeModel* modes();

    const KeyboardConfiguratorController::Mode& selectedMode();
    const int& selectedLightModeIndex();
    bool isCustomLightModeSelected();
    bool showColoursColumn();

    CustomEffectModel* customEffects();
    const int& selectedCustomEffectIndex();
    const bool& customEffectModified();

    bool animationSupported();
    bool animEnabled();
    int animPattern();
    int animStep();
    int animFade();
    int animFadeIn();
    bool animReverse();
    bool animShuffle();
    int animTrail();
    int animCurve();
    int animEffectiveFade();
    bool animHasDirection();
    int animBrightness();
    int animLoop();
    int animPause();
    QVariantMap animBackground();
    const bool& playing();
    bool canResumePlayback();

    bool editingSequence();
    QString sequenceText();
    int sequenceLength();

    // Whether the default profile plays an animation on this keyboard
    bool defaultProfileAnimated(const Keyboard& keyboard);

    // Stops live lighting and hands the keyboard back to its saved lighting,
    // waiting until that is sent. Safe to call when nothing is playing.
    void stopPlaybackBlocking();

    ProfileModel* profiles();
    const int& selectedProfileIndex();

    QSortFilterProxyModel* keyCodesProxy();
    CustomMappedKeys* customMappedKeys();

    MacroModel* macros();
    int editingMacroSlot();
    QString editingMacroName();
    // Each event as {text, delay}
    QVariantList editingMacroEvents();
    bool editingMacroModified();

    // Starts editing a copy of a macro slot
    Q_INVOKABLE void editMacro(int slot);
    // Adds a key going down or up. gapMs is the time since the previous event,
    // used as its delay; 0 or less uses defaultDelayMs. False if the key is
    // unknown or the macro is full.
    Q_INVOKABLE bool recordMacroKey(int nativeScanCode, bool release, int gapMs, int defaultDelayMs);
    // Adds the presses to type text, returns the characters with no key
    Q_INVOKABLE QString addMacroText(const QString& text, int delayMs);
    Q_INVOKABLE void setMacroEventDelay(int index, int delayMs);
    Q_INVOKABLE void setAllMacroDelays(int delayMs);
    Q_INVOKABLE void removeMacroEvent(int index);
    Q_INVOKABLE void clearMacroEvents();
    Q_INVOKABLE void saveEditingMacro();
    Q_INVOKABLE void deleteMacro(int slot);
    // Makes the key being remapped play a macro slot
    Q_INVOKABLE void setSelectedKeyMacro(int slot);

    const int& topLeftX();
    const int& topLeftY();
    const int& bottomRightX();
    const int& bottomRightY();

    Q_INVOKABLE Key getKeyByCustomLightModeKeysIndex(int index);
    Q_INVOKABLE Key getKeyByCustomMappedKeysIndex(int index);

    Q_INVOKABLE int keyCount();
    Q_INVOKABLE Key getKey(int index);
    Q_INVOKABLE int previewLevel(int keyIndex);
    Q_INVOKABLE void advancePreview();

    // Step numbers a key lights up at in the sequence, e.g. "1·8"
    Q_INVOKABLE QString stepLabels(int keyIndex);
    // Adds a step per typed character, returns the characters with no key
    Q_INVOKABLE QString addSequenceText(const QString& text);
    // Adds a step for a physical key press, or adds it to the last step when
    // it was pressed together with other keys. False if the key is unknown.
    // gapMs is the time since the previous press, turned into pauses so the
    // sequence keeps the rhythm it was typed in; 0 or less adds no pauses
    Q_INVOKABLE bool recordKey(int nativeScanCode, bool withPrevious, int gapMs = 0);

signals:
    void loadName(const QString& name);
    void loadImage(const QString& imagePath);
    void macroEditorChanged();
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
    void selectedCustomEffectIndexChanged();
    void customEffectModifiedChanged();
    void animationSupportedChanged();
    void animSettingsChanged();
    void playingChanged();
    void sequenceChanged();
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
    void setEditingMacroName(const QString& name);
    void setSavingChanges(const bool& savingChanges);
    void setBrightness(const int& brightness);
    void setAnimation(const int& animation);
    void setSleep(const int& sleep);
    void setSelectedMode(const KeyboardConfiguratorController::Mode& selectedMode);
    void setSelectedLightModeIndex(const int& selectedLightModeIndex);
    void setSelectedProfileIndex(const int& selectedProfileIndex);
    void clickKey(const int& x, const int& y);
    void clickKeySecondary(const int& x, const int& y);
    // Adds the key to (or takes it out of) the last sequence step
    void clickKeyGrouped(const int& x, const int& y);
    void selectAllKeysForCustomMode();
    void unselectAllKeysForCustomMode();

    void setSelectedCustomEffectIndex(const int& selectedCustomEffectIndex);
    void saveAsNewCustomEffect(const QString& name);
    void saveSelectedCustomEffect();
    void setCustomEffectName(const int& row, const QString& name);
    void duplicateCustomEffect(const int& row, const QString& name);
    void deleteCustomEffect(const int& row);

    void setAnimEnabled(const bool& enabled);
    void setAnimPattern(const int& pattern);
    void setAnimStep(const int& stepMs);
    void setAnimFade(const int& fadeMs);
    void setAnimFadeIn(const int& fadeInMs);
    void setAnimReverse(const bool& reverse);
    void setAnimShuffle(const bool& shuffle);
    void setAnimTrail(const int& trailKeys);
    void setAnimCurve(const int& fadeCurve);
    void setAnimBrightness(const int& brightness);
    void setAnimLoop(const int& loop);
    void setAnimPause(const int& pauseMs);

    // name is one of the keys of animBackground
    void setAnimBackground(const QString& name, const QVariant& value);

    void stopPlayback();
    // Plays the last applied animation again after it was stopped
    void resumePlayback();

    void undoSequenceStep();
    void addSequencePause();
    void clearSequence();

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

    QPointer<CustomEffectModel> m_customEffects;

    QPointer<MacroModel> m_macros;
    int m_editingMacroSlot{-1};
    Macro m_editingMacro;
    bool m_editingMacroModified{false};
    // The macro area went out with the save in progress
    bool m_macroAreaPending{false};

    bool addMacroEvent(const MacroEvent& event, int gapMs, int defaultDelayMs);
    int m_selectedCustomEffectIndex;
    bool m_customEffectModified;

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

    void setCustomEffectModified(const bool& customEffectModified);
    void markCustomLightModeKeysEdited();
    QHash<int, QColor> customLightModeKeysByBufferIndex();
    QList<int> customLightModeKeysOrder();
    void loadCustomLightModeKeysByBufferIndex(const QHash<int, QColor>& keys, const QList<int>& order);

    AnimationSettings m_anim;
    QList<QList<int>> m_sequence;
    LightAnimation m_previewAnimation;
    QElapsedTimer m_previewClock;
    QList<int> m_previewLevels;

    LightAnimation m_liveAnimation;
    QPointer<QTimer> m_liveTimer;
    QElapsedTimer m_liveClock;
    QList<Key> m_liveKeys;
    QString m_livePath;
    QByteArray m_liveRestoreBuffer;
    bool m_liveFrameInFlight;
    int m_liveFailures;
    bool m_playing;
    bool m_startPlaybackAfterSave;
    // m_liveAnimation holds an applied animation that can be resumed
    bool m_livePrepared;

    void setAnimSettings(const AnimationSettings& anim, const bool& edited);

    void setSequence(const QList<QList<int>>& sequence, const bool& edited);
    void toggleKeyInLastStep(const int& keyIndex);
    void syncSelectionFromSequence();
    QList<QList<int>> sequenceByBufferIndex();
    void loadSequenceByBufferIndex(const QList<QList<int>>& sequence);
    int keyIndexForCode(const KeyCode::Code& code);
    CustomEffect currentEffectContent();
    void preparePreview();
    void startPlayback();
    void stopPlayback(const bool& restoreLighting);
    void sendLiveFrame();
    unsigned char** takeRestoreBuffers();

    QString getProfileID();

    int getKeyIndexAtLocation(const int& x, const int& y);

    const Keyboard& selectedKeyboard();

};

#endif // KEYBOARDCONFIGURATORCONTROLLER_H
