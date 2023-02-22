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

#include "mainwindowcontroller.h"
#include "settingscontroller.h"
#include "hidconnection.h"
#include <QFile>
#include <QProcess>
#include <QDesktopServices>

using namespace Qt::Literals::StringLiterals;

MainWindowController::MainWindowController(QObject *parent)
    : QObject{parent}, m_connectedKeyboards{new KeyboardModel{this}}, m_profiles{new ProfileModel{this}},
      m_interruptClose{true}, m_confirmQuitDialogVisible{false}, m_alwaysShowSystemTrayIcon{false},
      m_closeToSystemTrayIcon{false}, m_startToSystemTrayIcon{false}, m_keyboardsScanning{true},
      m_toolTipDelay{1000}, m_toolTipTimeout{15000}, m_linuxUdevPopupProceedButtonEnabled{true}
{}

MainWindowController::~MainWindowController()
{
    HIDConnection::getInstance().stop();
    m_connectedKeyboards.clear();
}

void MainWindowController::showEnhancedDialog(QObject* object, const EnhancedDialog &dialog)
{
    emit qvariant_cast<MainWindowController*>
            (qmlEngine(object)
             ->rootContext()
             ->contextProperty(u"mainWindowController"_s))->loadEnhancedDialog(dialog);
}

const bool &MainWindowController::interruptClose()
{
    return m_interruptClose;
}

const int &MainWindowController::toolTipDelay()
{
    return m_toolTipDelay;
}

const int &MainWindowController::toolTipTimeout()
{
    return m_toolTipTimeout;
}

QPointer<KeyboardModel> MainWindowController::connectedKeyboards()
{
    return m_connectedKeyboards;
}

KeyboardModel *MainWindowController::connectedKeyboardsRaw()
{
    return m_connectedKeyboards.get();
}

bool &MainWindowController::alwaysShowSystemTrayIcon()
{
    return m_alwaysShowSystemTrayIcon;
}

bool &MainWindowController::closeToSystemTrayIcon()
{
    return m_closeToSystemTrayIcon;
}

bool &MainWindowController::startToSystemTrayIcon()
{
    return m_startToSystemTrayIcon;
}

bool &MainWindowController::keyboardsScanning()
{
    return m_keyboardsScanning;
}

bool &MainWindowController::linuxUdevPopupProceedButtonEnabled()
{
    return m_linuxUdevPopupProceedButtonEnabled;
}

bool MainWindowController::isWindows()
{
#ifdef Q_OS_WINDOWS
    return true;
#else
    return false;
#endif
}

QString MainWindowController::linuxUdevRulesPath()
{
    return QStringLiteral(LINUX_UDEV_RULES_PATH);
}

QPointer<ProfileModel> MainWindowController::profiles()
{
    return m_profiles;
}

void MainWindowController::init()
{
    m_startToSystemTrayIcon = QCoreApplication::arguments().contains(u"start_to_system_tray_icon"_s);

    if (m_startToSystemTrayIcon)
        emit hide();
    else
        emit show();

    m_profiles->registerProfiles();

    m_keyboardConfiguratorController = qvariant_cast<KeyboardConfiguratorController*>
            (qmlEngine(this)
             ->rootContext()
             ->contextProperty(u"keyboardConfiguratorController"_s));

    QPointer<SettingsController> m_settingsController = qvariant_cast<SettingsController*>
            (qmlEngine(this)
             ->rootContext()
             ->contextProperty(u"settingsController"_s));

    m_settingsController->init();
    m_keyboardConfiguratorController->init();

    HIDConnection& connection = HIDConnection::getInstance();

    connect(&connection, &HIDConnection::initDone, this, [this](const bool& success){
        if (success)
        {
            refreshKeyboards();
        }
        else
        {
            showFatalError(tr("Unable to initialise HID API. "
                              "Please check if you have permissions and restart Rangoli. "
                              "The program will now exit."));
        }
    }, Qt::SingleShotConnection);

    connect(&connection, &HIDConnection::fatalErrorOccured, this, &MainWindowController::showFatalError);

    connect(&connection, &HIDConnection::keyboardConnected,
            this, [this](const Keyboard& keyboard){
        m_connectedKeyboards->append(keyboard);
        emit adjustOptionsSize(m_connectedKeyboards->rowCount());
    });

    connect(&connection, &HIDConnection::keyboardDisconnected,
            this, [this](const KeyboardUSBID& id){
        m_connectedKeyboards->remove(id);
        emit adjustOptionsSize(m_connectedKeyboards->rowCount());
    });

    connect(&connection, &HIDConnection::keyboardsScanComplete,
            this, [this](){
        m_keyboardsScanning = false;
        emit keyboardsScanningChanged();
    });

    if (m_settingsController->applyDefaultProfileOnStartup())
    {
        connect(&connection, &HIDConnection::keyboardsScanComplete,
                this, [this](){
            QSettings settings;

            for (int i = 0 ; i < m_connectedKeyboards->keyboards().size(); i++)
            {
                m_keyboardConfiguratorController->load(i);
                m_keyboardConfiguratorController->applyDefaultProfile();
            }
        }, Qt::SingleShotConnection);
    }

    connection.start();


#ifdef Q_OS_LINUX
    QSettings settings;

    bool udevRulesWritten = settings
            .value(QStringLiteral("udev_rules_written/%1")
                   .arg(QString::number(VERSION)),false).toBool();

    if (!QFile::exists(QStringLiteral(LINUX_UDEV_RULES_PATH)) || !udevRulesWritten)
    {
        emit openLinuxUdevPopup();
    }
#endif

    if(m_settingsController->checkForUpdatesOnStartup())
    {
        m_settingsController->checkForUpdates(true);
    }
}

void MainWindowController::quit()
{
    setInterruptClose(false);
    emit close();
}

void MainWindowController::refreshKeyboards()
{
    m_keyboardsScanning = true;
    emit keyboardsScanningChanged();

    emit HIDConnection::getInstance().refreshKeyboards(m_connectedKeyboards);
}

void MainWindowController::loadKeyboard(const int &index)
{
    m_keyboardConfiguratorController->load(index);

    if (m_keyboardConfiguratorController->selectedProfileIndex() == -1)
    {
        m_keyboardConfiguratorController->applyDefaultProfile();
    }
}

void MainWindowController::setInterruptClose(const bool &interruptClose)
{
    if (m_interruptClose == interruptClose) return;

    m_interruptClose = interruptClose;
    emit interruptCloseChanged();
}

void MainWindowController::closeInterrupted()
{
    if (!m_keyboardConfiguratorController->unsavedChanges())
    {
        quit();
        return;
    }

    if (!m_confirmQuitDialogVisible)
    {
        m_confirmQuitDialogVisible = true;

        EnhancedDialog d(tr("Warning"),
                         tr("There are unsaved changes. Are you sure you want to quit?"),
                         EnhancedDialog::ButtonsType::YES_NO);

        d.setOnAccepted([this](){quit();});
        d.setOnRejected([this](){m_confirmQuitDialogVisible = false;});

        emit loadEnhancedDialog(d);
    }
}

void MainWindowController::setTheme(const int& theme)
{
    if (theme == Theme::System)
    {
        emit setSystemTheme();
    }
    else if (theme == Theme::Light)
    {
        emit setLightTheme();
    }
    else if (theme == Theme::Dark)
    {
        emit setDarkTheme();
    }
}

void MainWindowController::setAlwaysShowSystemTrayIcon(const bool &alwaysShowSystemTrayIcon)
{
    if(m_alwaysShowSystemTrayIcon == alwaysShowSystemTrayIcon) return;

    m_alwaysShowSystemTrayIcon = alwaysShowSystemTrayIcon;
    emit alwaysShowSystemTrayIconChanged();
}

void MainWindowController::setCloseToSystemTrayIcon(const bool &closeToSystemTrayIcon)
{
    if(m_closeToSystemTrayIcon == closeToSystemTrayIcon) return;

    m_closeToSystemTrayIcon = closeToSystemTrayIcon;
    emit closeToSystemTrayIconChanged();
}

void MainWindowController::launchLinuxUdevWriter()
{
    setLinuxUdevPopupProceedButtonEnabled(false);

    if (!QFile::exists(QStringLiteral(LINUX_UDEV_RULE_WRITER_NAME)))
    {
        emit loadEnhancedDialog(EnhancedDialog {
                                    tr("Error"),
                                    tr("Unable to find udev rule writer.")
                                });

        setLinuxUdevPopupProceedButtonEnabled(true);
        return;
    }

    QString exitOK{u"0"_s};
    QString exitKeyboardReadFailed{u"1"_s};
    QString exitRuleWriteFailed{u"2"_s};

    QString oldID;
    QString exitFile{QStringLiteral("%1_OUTPUT").arg(QStringLiteral(LINUX_UDEV_RULE_WRITER_NAME))};
    QFile oldOutput{exitFile};

    if (oldOutput.exists())
    {
        if (!oldOutput.open(QIODevice::ReadOnly))
        {
            emit loadEnhancedDialog(EnhancedDialog {
                                        tr("Error"),
                                        tr("Unable to read old ID.")
                                    });

            setLinuxUdevPopupProceedButtonEnabled(true);
            return;
        }

        oldID = oldOutput.readAll().split(':').at(0);
    }

    QPointer<QProcess> udevWriter{new QProcess{this}};
    connect(udevWriter, &QProcess::finished, this,
            [this, exitFile, exitOK, exitKeyboardReadFailed, exitRuleWriteFailed, oldID]
            (int exitCode, QProcess::ExitStatus exitStatus) {
        Q_UNUSED(exitStatus)
        Q_UNUSED(exitCode)

        setLinuxUdevPopupProceedButtonEnabled(true);

        QFile output{exitFile};

        if (!output.open(QIODevice::ReadOnly))
        {
            emit loadEnhancedDialog(EnhancedDialog {
                                        tr("Error"),
                                        tr("The udev rules writer failed to run. Did you provide incorrect password?")
                                    });
            return;
        }

        QList<QByteArray> outputs{output.readAll().split(':')};

        QString newID = outputs.at(0);

        if (newID == oldID)
        {
            emit loadEnhancedDialog(EnhancedDialog {
                                        tr("Error"),
                                        tr("The udev rules writer failed to run. Did you provide incorrect password?")
                                    });
            return;
        }

        if (outputs.length() != 2)
        {
            emit loadEnhancedDialog(EnhancedDialog {
                                        tr("Error"),
                                        tr("Malformed output from udev writer.")
                                    });
            return;
        }

        QString newExitCode = outputs.at(1);

        if (newExitCode == exitOK)
        {
            emit closeLinuxUdevPopup();

            emit loadEnhancedDialog(EnhancedDialog {
                                        tr("Successful"),
                                        tr("udev rules have been applied and reloaded! Enjoy Rangoli!")
                                    });

            QSettings settings;
            settings.remove(u"udev_rules_written"_s);
            settings.setValue(QStringLiteral("udev_rules_written/%1").arg(QString::number(VERSION)), true);
        }
        else if (newExitCode == exitKeyboardReadFailed)
        {
            emit loadEnhancedDialog(EnhancedDialog {
                                        tr("Error"),
                                        tr("Malformed output from udev writer.")
                                    });
        }
        else if (newExitCode == exitRuleWriteFailed)
        {
            emit loadEnhancedDialog(EnhancedDialog {
                                        tr("Error"),
                                        tr("Malformed output from udev writer.")
                                    });
        }
        else
        {
            emit loadEnhancedDialog(EnhancedDialog {
                                        tr("Error"),
                                        tr("Malformed exit code from udev writer.")
                                    });
        }
    });

    connect(udevWriter, &QProcess::errorOccurred, this,
            [this](QProcess::ProcessError error){

        setLinuxUdevPopupProceedButtonEnabled(true);

        if (error == QProcess::FailedToStart)
        {
            emit loadEnhancedDialog(EnhancedDialog{
                                        tr("Unable to start udev rules writer"),
                                        tr("You need to have '%1' installed!").arg(LINUX_TERMINAL)
                                    });
        }
        else
        {
            emit loadEnhancedDialog(EnhancedDialog{
                                        tr("Unable to start udev rules writer."),
                                        tr("Error Code: %1").arg(QString::number(error))
                                    });
        }
    });

    udevWriter->start(QStringLiteral(LINUX_TERMINAL), QStringList() << u"-e"_s <<
                          QStringLiteral("echo -n \"\\033]0;%1\\007\" && sudo ./%2 %3 %4 %5 %6 %7 %8")
                          .arg(tr("Rangoli - Create udev Rules"),
                               QStringLiteral(LINUX_UDEV_RULE_WRITER_NAME),
                               QStringLiteral(KEYBOARDS_LIST_PATH),
                               QStringLiteral(LINUX_UDEV_RULES_PATH),
                               exitFile, exitOK, exitKeyboardReadFailed, exitRuleWriteFailed));
}

void MainWindowController::setLinuxUdevPopupProceedButtonEnabled(const bool &linuxUdevPopupProceedButtonEnabled)
{
    if (m_linuxUdevPopupProceedButtonEnabled == linuxUdevPopupProceedButtonEnabled) return;

    m_linuxUdevPopupProceedButtonEnabled = linuxUdevPopupProceedButtonEnabled;
    emit linuxUdevPopupProceedButtonEnabledChanged();
}

void MainWindowController::openSupportedKeyboardsList()
{
    QDesktopServices::openUrl(QUrl(QStringLiteral(SUPPORTED_KEYBOARDS)));
}

void MainWindowController::showFatalError(const QString &message)
{
    EnhancedDialog d{tr("Fatal error occurred"), message};
    d.setOnAccepted([this](){
        setCloseToSystemTrayIcon(false);
        quit();
    });

    emit loadEnhancedDialog(d);
}
