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

#ifndef SETTINGSCONTROLLER_H
#define SETTINGSCONTROLLER_H

#include <QObject>
#include <QStringList>
#include "mainwindowcontroller.h"
#include "profilemodel.h"
#include "keyboardconfiguratorcontroller.h"

class SettingsController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList themes READ themes CONSTANT)
    Q_PROPERTY(int selectedThemeIndex READ selectedThemeIndex WRITE setSelectedThemeIndex NOTIFY selectedThemeIndexChanged)
    Q_PROPERTY(QColor accentColour READ accentColour WRITE setAccentColour NOTIFY accentColourChanged)
    Q_PROPERTY(bool startOnBootSupported READ startOnBootSupported CONSTANT)
    Q_PROPERTY(bool startOnBoot READ startOnBoot WRITE setStartOnBoot NOTIFY startOnBootChanged)
    Q_PROPERTY(bool applyDefaultProfileOnStartup READ applyDefaultProfileOnStartup WRITE setApplyDefaultProfileOnStartup NOTIFY applyDefaultProfileOnStartupChanged)
    Q_PROPERTY(bool checkForUpdatesOnStartup READ checkForUpdatesOnStartup WRITE setCheckForUpdatesOnStartup NOTIFY checkForUpdatesOnStartupChanged)
    Q_PROPERTY(bool alwaysShowSystemTrayIcon READ alwaysShowSystemTrayIcon WRITE setAlwaysShowSystemTrayIcon NOTIFY alwaysShowSystemTrayIconChanged)
    Q_PROPERTY(bool closeToSystemTrayIcon READ closeToSystemTrayIcon WRITE setCloseToSystemTrayIcon NOTIFY closeToSystemTrayIconChanged)

    Q_PROPERTY(bool unsavedChanges READ unsavedChanges WRITE setUnsavedChanges NOTIFY unsavedChangesChanged)
    Q_PROPERTY(ProfileModel* profiles READ profiles CONSTANT)
    Q_PROPERTY(int selectedDefaultProfileIndex READ selectedDefaultProfileIndex WRITE setSelectedDefaultProfileIndex NOTIFY selectedDefaultProfileIndexChanged)
    Q_PROPERTY(QString version READ version CONSTANT)
    Q_PROPERTY(QString authorUPI READ authorUPI CONSTANT)
    Q_PROPERTY(QString authorEmail READ authorEmail CONSTANT)
    Q_PROPERTY(bool checkForUpdatesButtonEnabled READ checkForUpdatesButtonEnabled WRITE setCheckForUpdatesButtonEnabled NOTIFY checkForUpdatesButtonEnabledChanged)

public:
    explicit SettingsController(QObject *parent = nullptr);

    QStringList& themes();
    int& selectedThemeIndex();
    bool& unsavedChanges();
    QColor& accentColour();
    const bool& startOnBootSupported();
    bool& startOnBoot();
    bool& applyDefaultProfileOnStartup();
    bool& checkForUpdatesOnStartup();
    bool& alwaysShowSystemTrayIcon();
    bool& closeToSystemTrayIcon();

    bool udevRulesWritten();
    void setUdevRulesWritten(const bool& written);

    bool firstTimeUse();
    void setFirstTimeUse(const bool& firstTimeUse);

    ProfileModel* profiles();
    int& selectedDefaultProfileIndex();
    QString version();
    QString authorUPI();
    QString authorEmail();

    bool& checkForUpdatesButtonEnabled();

signals:
    void selectedThemeIndexChanged();
    void accentColourChanged();
    void startOnBootChanged();
    void applyDefaultProfileOnStartupChanged();
    void checkForUpdatesOnStartupChanged();
    void alwaysShowSystemTrayIconChanged();
    void closeToSystemTrayIconChanged();

    void unsavedChangesChanged();
    void selectedDefaultProfileIndexChanged();
    void checkForUpdatesButtonEnabledChanged();

public slots:
    void init();
    void setSelectedThemeIndex(const int& selectedThemeIndex);
    void setAccentColour(const QColor& accentColour);
    void setStartOnBoot(const bool& startOnBoot);
    void setApplyDefaultProfileOnStartup(const bool& applyDefaultProfileOnStartup);
    void setCheckForUpdatesOnStartup(const bool& checkForUpdatesOnStartup);
    void setAlwaysShowSystemTrayIcon(const bool& alwaysShowSystemTrayIcon);
    void setCloseToSystemTrayIcon(const bool& closeToSystemTrayIcon);

    void setUnsavedChanges(const bool& unsavedChanges);
    void setSelectedDefaultProfileIndex(const int& selectedDefaultProfileIndex);

    void openGitRepository();
    void openWebsite();
    void reportIssue();
    void openLicense();
    void openAuthorWebsite();
    void openAuthorKoFi();
    void emailAuthor();

    void load();
    void save();

    void checkForUpdates(const bool& quiet = false);

    void setCheckForUpdatesButtonEnabled(const bool& checkForUpdatesButtonEnabled);

private slots:
    void updatesFetched(QNetworkReply* reply);

private:
    QStringList m_themes;
    int m_selectedThemeIndex;
    QPointer<MainWindowController> m_mainWindow;
    QPointer<KeyboardConfiguratorController> m_keyboardConfiguratorController;
    QPointer<ProfileModel> m_profiles;
    QColor m_accentColour;
    bool m_startOnBoot;
    bool m_applyDefaultProfileOnStartup;
    bool m_checkForUpdatesOnStartup;
    bool m_alwaysShowSystemTrayIcon;
    bool m_closeToSystemTrayIcon;

    int m_selectedDefaultProfileIndex;
    bool m_unsavedChanges;
    bool m_checkForUpdatesQuietMode;
    bool m_checkForUpdatesButtonEnabled;

    void applyVisualSettings();
};

#endif // SETTINGSCONTROLLER_H
