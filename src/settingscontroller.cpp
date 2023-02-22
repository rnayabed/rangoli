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

#include "settingscontroller.h"

#include <QDesktopServices>
#include <QNetworkAccessManager>
#include "startatboot.h"

using namespace Qt::Literals::StringLiterals;

SettingsController::SettingsController(QObject *parent)
    : QObject{parent},
      m_themes{tr("System"), tr("Light"), tr("Dark")},
      m_selectedThemeIndex{MainWindowController::Theme::System},
      m_startOnBoot{false},
      m_applyDefaultProfileOnStartup{false},
      m_checkForUpdatesOnStartup{false},
      m_alwaysShowSystemTrayIcon{false},
      m_closeToSystemTrayIcon{false},
      m_selectedDefaultProfileIndex{0},
      m_unsavedChanges{false},
      m_checkForUpdatesQuietMode{false},
      m_checkForUpdatesButtonEnabled{true}
{}

QStringList &SettingsController::themes()
{
    return m_themes;
}

int &SettingsController::selectedThemeIndex()
{
    return m_selectedThemeIndex;
}

bool &SettingsController::unsavedChanges()
{
    return m_unsavedChanges;
}

QColor &SettingsController::accentColour()
{
    return m_accentColour;
}

const bool &SettingsController::startOnBootSupported()
{
    return StartAtBoot::Supported;
}

bool &SettingsController::startOnBoot()
{
    return m_startOnBoot;
}

bool &SettingsController::applyDefaultProfileOnStartup()
{
    return m_applyDefaultProfileOnStartup;
}

bool &SettingsController::checkForUpdatesOnStartup()
{
    return m_checkForUpdatesOnStartup;
}

bool &SettingsController::alwaysShowSystemTrayIcon()
{
    return m_alwaysShowSystemTrayIcon;
}

bool &SettingsController::closeToSystemTrayIcon()
{
    return m_closeToSystemTrayIcon;
}

ProfileModel *SettingsController::profiles()
{
    return m_profiles.get();
}

int &SettingsController::selectedDefaultProfileIndex()
{
    return m_selectedDefaultProfileIndex;
}

QString SettingsController::version()
{
    return QString::number(VERSION);
}

QString SettingsController::authorUPI()
{
    return QStringLiteral(AUTHOR_UPI);
}

QString SettingsController::authorEmail()
{
    return QStringLiteral(AUTHOR_EMAIL);
}

bool &SettingsController::checkForUpdatesButtonEnabled()
{
    return m_checkForUpdatesButtonEnabled;
}

void SettingsController::init()
{
    m_mainWindow = qvariant_cast<MainWindowController*>(qmlEngine(this)
                                                        ->rootContext()
                                                        ->contextProperty("mainWindowController"));

    m_keyboardConfiguratorController = qvariant_cast<KeyboardConfiguratorController*>(qmlEngine(this)
                                                        ->rootContext()
                                                        ->contextProperty("keyboardConfiguratorController"));

    m_profiles = m_mainWindow->profiles();

    connect(m_profiles.get(), &ProfileModel::defaultProfileIndexChanged, this, [this](){
        setSelectedDefaultProfileIndex(m_profiles->defaultProfileIndex());
    });

    load();
}

void SettingsController::setSelectedThemeIndex(const int &selectedThemeIndex)
{
    if(m_selectedThemeIndex == selectedThemeIndex) return;

    m_selectedThemeIndex = selectedThemeIndex;
    emit selectedThemeIndexChanged();
}

void SettingsController::setAccentColour(const QColor &accentColour)
{
    if(m_accentColour == accentColour) return;

    m_accentColour = accentColour;
    emit accentColourChanged();
}

void SettingsController::setStartOnBoot(const bool &startOnBoot)
{
    if(m_startOnBoot == startOnBoot) return;

    m_startOnBoot = startOnBoot;
    emit startOnBootChanged();
}

void SettingsController::setApplyDefaultProfileOnStartup(const bool &applyDefaultProfileOnStartup)
{
    if(m_applyDefaultProfileOnStartup == applyDefaultProfileOnStartup) return;

    m_applyDefaultProfileOnStartup = applyDefaultProfileOnStartup;
    emit applyDefaultProfileOnStartupChanged();
}

void SettingsController::setCheckForUpdatesOnStartup(const bool &checkForUpdatesOnStartup)
{
    if(m_checkForUpdatesOnStartup == checkForUpdatesOnStartup) return;

    m_checkForUpdatesOnStartup = checkForUpdatesOnStartup;
    emit checkForUpdatesOnStartupChanged();
}

void SettingsController::setAlwaysShowSystemTrayIcon(const bool &alwaysShowSystemTrayIcon)
{
    if(m_alwaysShowSystemTrayIcon == alwaysShowSystemTrayIcon) return;

    m_alwaysShowSystemTrayIcon = alwaysShowSystemTrayIcon;
    emit alwaysShowSystemTrayIconChanged();
}

void SettingsController::setCloseToSystemTrayIcon(const bool &closeToSystemTrayIcon)
{
    if(m_closeToSystemTrayIcon == closeToSystemTrayIcon) return;

    m_closeToSystemTrayIcon = closeToSystemTrayIcon;
    emit closeToSystemTrayIconChanged();
}

void SettingsController::setUnsavedChanges(const bool &unsavedChanges)
{
    if(m_unsavedChanges == unsavedChanges) return;

    m_unsavedChanges = unsavedChanges;
    emit unsavedChangesChanged();
}

void SettingsController::setSelectedDefaultProfileIndex(const int &selectedDefaultProfileIndex)
{
    if(m_selectedDefaultProfileIndex == selectedDefaultProfileIndex) return;

    m_selectedDefaultProfileIndex = selectedDefaultProfileIndex;
    emit selectedDefaultProfileIndexChanged();
}

void SettingsController::openGitRepository()
{
    QDesktopServices::openUrl(QUrl(QStringLiteral(GIT_REPOSITORY)));
}

void SettingsController::openWebsite()
{
    QDesktopServices::openUrl(QUrl(QStringLiteral(WEBSITE)));
}

void SettingsController::reportIssue()
{
    QDesktopServices::openUrl(QUrl(QStringLiteral(REPORT_ISSUE)));
}

void SettingsController::openLicense()
{
    QDesktopServices::openUrl(QUrl(QStringLiteral(LICENSE)));
}

void SettingsController::openAuthorWebsite()
{
    QDesktopServices::openUrl(QUrl(QStringLiteral(AUTHOR_WEBSITE)));
}

void SettingsController::openAuthorKoFi()
{
    QDesktopServices::openUrl(QUrl(QStringLiteral(AUTHOR_KO_FI)));
}

void SettingsController::emailAuthor()
{
    QDesktopServices::openUrl(QUrl(QStringLiteral("mailto:%1").arg(AUTHOR_EMAIL)));
}

void SettingsController::load()
{
    QSettings settings;

    setSelectedThemeIndex(settings.value(u"theme"_s, MainWindowController::Theme::System).toInt());
    setAccentColour(settings.value(u"accent_colour"_s, QColor(DEFAULT_ACCENT_RED, DEFAULT_ACCENT_GREEN, DEFAULT_ACCENT_BLUE)).value<QColor>());

    setSelectedDefaultProfileIndex(m_profiles->defaultProfileIndex());
    setStartOnBoot(settings.value(u"start_on_boot"_s, false).toBool());
    setApplyDefaultProfileOnStartup(settings.value(u"apply_default_profile_on_startup"_s, false).toBool());
    setCheckForUpdatesOnStartup(settings.value(u"check_for_updates_on_startup"_s, true).toBool());
    setAlwaysShowSystemTrayIcon(settings.value(u"system_tray/always"_s, true).toBool());
    setCloseToSystemTrayIcon(settings.value(u"system_tray/close_to"_s, true).toBool());

    setUnsavedChanges(false);
    applyVisualSettings();
}

void SettingsController::save()
{
    QSettings settings;

    settings.setValue(u"theme"_s, m_selectedThemeIndex);
    settings.setValue(u"accent_colour"_s, m_accentColour);

    if (m_keyboardConfiguratorController->selectedMode() == KeyboardConfiguratorController::KeyMap
            && m_keyboardConfiguratorController->customMappedKeys()->keysCount() > 0)
    {
        emit m_keyboardConfiguratorController->requestKbCanvasPaint();
    }

    m_profiles->setDefaultProfileIndex(selectedDefaultProfileIndex());

    if (settings.value(u"start_on_boot"_s, false).toBool() != m_startOnBoot)
    {
        if (m_startOnBoot)
        {
            if (! StartAtBoot::createStarter())
            {
                emit m_mainWindow->loadEnhancedDialog(
                            EnhancedDialog{
                                tr("Error"),
                                tr("Unable to setup start on boot. Make sure you have enough permissions.")
                            });

                setStartOnBoot(!m_startOnBoot);
            }
        }
        else
        {
            if (! StartAtBoot::deleteStarter())
            {
                emit m_mainWindow->loadEnhancedDialog(
                            EnhancedDialog{
                                tr("Error"),
                                tr("Unable to turn off start on boot. Make sure you have enough permissions.")
                            });
            }
        }
    }

    settings.setValue(u"start_on_boot"_s, m_startOnBoot);
    settings.setValue(u"apply_default_profile_on_startup"_s, m_applyDefaultProfileOnStartup);
    settings.setValue(u"check_for_updates_on_startup"_s, m_checkForUpdatesOnStartup);
    settings.setValue(u"system_tray/always"_s, m_alwaysShowSystemTrayIcon);
    settings.setValue(u"system_tray/close_to"_s, m_closeToSystemTrayIcon);

    setUnsavedChanges(false);
    applyVisualSettings();
}

void SettingsController::checkForUpdates(const bool& quiet)
{
    setCheckForUpdatesButtonEnabled(false);

    m_checkForUpdatesQuietMode = quiet;

    QPointer<QNetworkAccessManager> nam{new QNetworkAccessManager{this}};
    connect(nam, &QNetworkAccessManager::finished, this, &SettingsController::updatesFetched, Qt::SingleShotConnection);
    nam->get(QNetworkRequest{QUrl{QStringLiteral(GITHUB_RELEASES)}});
}

void SettingsController::setCheckForUpdatesButtonEnabled(const bool &checkForUpdatesButtonEnabled)
{
    if (m_checkForUpdatesButtonEnabled == checkForUpdatesButtonEnabled) return;

    m_checkForUpdatesButtonEnabled = checkForUpdatesButtonEnabled;
    emit checkForUpdatesButtonEnabledChanged();
}

void SettingsController::updatesFetched(QNetworkReply *reply)
{
    setCheckForUpdatesButtonEnabled(true);

    if (reply->error() != QNetworkReply::NoError)
    {
        if (!m_checkForUpdatesQuietMode)
        {
            emit m_mainWindow->loadEnhancedDialog(EnhancedDialog{
                                                        tr("Error"),
                                                        tr("Unable to check for updates. Check your internet connection and try again.")
                                                  });
        }
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());

    QJsonArray array = doc.array();

    bool updateFound = false;

    for (int i = 0; i < array.size(); i++)
    {
        auto obj = array.at(i).toObject();

        if (!obj["prerelease"].toBool() && !obj["draft"].toBool())
        {
            QString tagName = obj["tag_name"].toString();
            bool ok;
            float version = tagName.toFloat(&ok);
            if (ok && version > VERSION)
            {
                EnhancedDialog d{
                    tr("Update available"),
                    tr("Version %1 is available for download. Open release page?").arg(version),
                    EnhancedDialog::ButtonsType::YES_NO
                };

                auto url = obj["html_url"].toString();

                d.setOnAccepted([&, url](){
                    QDesktopServices::openUrl(QUrl(url));
                });

                emit m_mainWindow->loadEnhancedDialog(d);

                updateFound = true;

                break;
            }
        }
    }

    if (!updateFound && !m_checkForUpdatesQuietMode)
    {
        emit m_mainWindow->loadEnhancedDialog(EnhancedDialog{
                                                    tr("Up to date"),
                                                    tr("You are running the latest version of Rangoli")
                                              });
    }

    reply->deleteLater();
}

void SettingsController::applyVisualSettings()
{
    emit m_mainWindow->setAccentColour(m_accentColour);
    m_mainWindow->setTheme(m_selectedThemeIndex);
    m_mainWindow->setAlwaysShowSystemTrayIcon(m_alwaysShowSystemTrayIcon);
    m_mainWindow->setCloseToSystemTrayIcon(m_closeToSystemTrayIcon);
}
