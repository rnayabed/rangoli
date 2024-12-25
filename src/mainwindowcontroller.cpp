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
#include <QFile>
#include <QProcess>
#include <QDesktopServices>
#include "settingscontroller.h"
#include "hidconnection.h"

using namespace Qt::Literals::StringLiterals;

MainWindowController::MainWindowController(QObject *parent)
    : QObject{parent}, m_connectedKeyboards{new KeyboardModel{this}}, m_profiles{new ProfileModel{this}},
      m_interruptClose{true}, m_confirmQuitDialogVisible{false}, m_alwaysShowSystemTrayIcon{false},
      m_closeToSystemTrayIcon{false}, m_startToSystemTrayIcon{false}, m_keyboardsScanning{true},
      m_toolTipDelay{1000}, m_toolTipTimeout{15000}, m_linuxUdevPopupProceedButtonEnabled{true}
{}

void MainWindowController::showEnhancedDialog(QObject* object, const EnhancedDialog &dialog)
{
    emit qvariant_cast<MainWindowController*>
            (qmlEngine(object)
             ->rootContext()
             ->contextProperty(u"mainWindowController"_s))->loadEnhancedDialog(dialog);
}

QString MainWindowController::macOSPermissionNotice()
{
    return tr("You need to provide access to input devices. "
              "This can be done by going to "
              "Settings > Security & Privacy > Privacy > "
              "Input Monitoring > Select Rangoli or add it's entry.\n\n"
              "You will need to restart Rangoli for changes to take effect.");
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
    {
        qInfo() << "Asked to start to System Tray Icon";
        emit hide();
    }
    else
    {
        emit show();
    }

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

    connect(&connection, &HIDConnection::HIDColInitFailed, this, [this](){
        showFatalError(tr("Unable to open HID Cols Config file"),
                       tr("The program will now exit."));
    }, Qt::SingleShotConnection);

    connect(&connection, &HIDConnection::HIDAPIInitFailed, this, [this](){
        showFatalError(tr("Unable to initialise HID API"),
                       tr("Please check if you have permissions and restart Rangoli. "
                          "The program will now exit."));
    }, Qt::SingleShotConnection);

    connect(&connection, &HIDConnection::initSuccessful, this, &MainWindowController::refreshKeyboards,
            Qt::SingleShotConnection);

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

            qInfo() << "Apply default profile";

            for (int i = 0 ; i < m_connectedKeyboards->keyboards().size(); i++)
            {
                loadKeyboardWithDefaultProfile(i);
            }
        }, Qt::SingleShotConnection);
    }

    QDir keyboards{u"keyboards"_s};
    if (!keyboards.exists())
    {
        qCritical() << "Keyboards folder not found";

#ifdef Q_OS_MACOS
        showFatalError(tr("Keyboards folder not found"),
                       tr("If you are using the portable version, you need to "
                          "whitelist the app from quarantine by running "
                          "the following command:\n"
                          "xattr -dr com.apple.quarantine <path to rangoli.app>\n\n"
                          "If you are using Rangoli as an installed applcation, "
                          "you need to re-install it."));
#else
        showFatalError(tr("Keyboards folder not found"),
                       tr("If you are using Rangoli as an installed applcation, "
                          "you need to re-install it. Otherwise, please download "
                          "Rangoli again."));
#endif

        return;
    }

    connection.start();

#ifdef Q_OS_LINUX
    if (!QFile::exists(QStringLiteral(LINUX_UDEV_RULES_PATH))
            || !m_settingsController->udevRulesWritten())
    {
        qInfo() << "Show Linux udev prompt";
        emit openLinuxUdevPopup();
    }
#endif

    if(m_settingsController->firstTimeUse())
    {
        qInfo() << "First time use";

#ifdef Q_OS_MACOS
        emit loadEnhancedDialog(EnhancedDialog{tr("Notice"), macOSPermissionNotice()});
#endif

        EnhancedDialog d{
            tr("Check for updates on startup?"),
            tr("Would you like to check for updates on startup?\n"
               "No user data is collected whatsoever.\n\n"
               "You can always change this in settings."),
            EnhancedDialog::ButtonsType::YES_NO
        };

        d.setOnAccepted([m_settingsController](){
            m_settingsController->setCheckForUpdatesOnStartup(true);
            m_settingsController->checkForUpdates(true);
        });

        d.setOnRejected([m_settingsController](){
            m_settingsController->setCheckForUpdatesOnStartup(false);
        });

        d.setOnClosed([m_settingsController](){
            m_settingsController->save();
            m_settingsController->setFirstTimeUse(false);
        });

        emit loadEnhancedDialog(d);
    }
    else if(m_settingsController->checkForUpdatesOnStartup())
    {
        m_settingsController->checkForUpdates(true);
    }
}

void MainWindowController::quit()
{
    setInterruptClose(false);
    HIDConnection::getInstance().stop();
    m_connectedKeyboards.clear();
    emit close();
}

void MainWindowController::refreshKeyboards()
{
    m_keyboardsScanning = true;
    emit keyboardsScanningChanged();

    emit HIDConnection::getInstance().refreshKeyboards(m_connectedKeyboards);
}

void MainWindowController::loadKeyboardWithDefaultProfile(const int &index) {
    m_keyboardConfiguratorController->load(index);

    m_keyboardConfiguratorController->applyDefaultProfile();
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
        qInfo() << "Show unsaved changes prompt";

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
        qInfo() << "Set System theme";
        emit setSystemTheme();
    }
    else if (theme == Theme::Light)
    {
        qInfo() << "Set Light theme";
        emit setLightTheme();
    }
    else if (theme == Theme::Dark)
    {
        qInfo() << "Set Dark theme";
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
    qInfo() << "Launch Linux udev writer";

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
    QString exitKeyboardsReadFailed{u"1"_s};
    QString exitRuleWriteFailed{u"2"_s};

    QString oldID;
    QString outputPath{QStringLiteral("%1_OUTPUT").arg(QStringLiteral(LINUX_UDEV_RULE_WRITER_NAME))};
    QFile oldOutput{outputPath};

    if (oldOutput.exists())
    {
        if (!oldOutput.open(QIODevice::ReadOnly))
        {
            qCritical() << "Failed to old output" << outputPath;
            emit loadEnhancedDialog(EnhancedDialog {
                                        tr("Error"),
                                        tr("Unable to read old ID.")
                                    });

            setLinuxUdevPopupProceedButtonEnabled(true);
            return;
        }

        oldID = oldOutput.readAll().split(':').at(0);

        oldOutput.close();
    }

    QPointer<QProcess> udevWriter{new QProcess{this}};
    connect(udevWriter, &QProcess::finished, this,
            [this, outputPath, exitOK, exitKeyboardsReadFailed, exitRuleWriteFailed, oldID]
            (int exitCode, QProcess::ExitStatus exitStatus) {
        Q_UNUSED(exitStatus)
        Q_UNUSED(exitCode)

        setLinuxUdevPopupProceedButtonEnabled(true);

        QFile output{outputPath};

        if (!output.open(QIODevice::ReadOnly))
        {
            qCritical() << "Failed to new output" << outputPath;
            emit loadEnhancedDialog(EnhancedDialog {
                                        tr("Error"),
                                        tr("The udev rules writer failed to run. Did you provide incorrect password?")
                                    });
            return;
        }

        QByteArray outRaw = output.readAll();
        qDebug() << "Output File raw:" << outRaw;

        output.close();

        QList<QByteArray> outputs{outRaw.split(':')};
        qDebug() << "Outputs length:" << outputs.length();

        QString newID = outputs.at(0);
        qDebug() << "New ID:" << newID;
        qDebug() << "Old ID:" << oldID;

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
        qDebug() << "New exit code:" << newExitCode;

        if (newExitCode == exitOK)
        {
            emit closeLinuxUdevPopup();

            emit loadEnhancedDialog(EnhancedDialog {
                                        tr("Successful"),
                                        tr("udev rules have been applied and reloaded! Enjoy Rangoli!")
                                    });

            qvariant_cast<SettingsController*>
                        (qmlEngine(this)
                         ->rootContext()
                         ->contextProperty(u"settingsController"_s))->setUdevRulesWritten(true);
        }
        else if (newExitCode == exitKeyboardsReadFailed)
        {
            qCritical() << "Failed to read keyboards information";

            emit loadEnhancedDialog(EnhancedDialog {
                                        tr("Error"),
                                        tr("Failed to read keyboards information")
                                    });
        }
        else if (newExitCode == exitRuleWriteFailed)
        {
            qCritical() << "Malformed output from udev writer";

            emit loadEnhancedDialog(EnhancedDialog {
                                        tr("Error"),
                                        tr("Malformed output from udev writer.")
                                    });
        }
        else
        {
            qCritical() << "Malformed exit code from udev writer";

            emit loadEnhancedDialog(EnhancedDialog {
                                        tr("Error"),
                                        tr("Malformed exit code from udev writer.")
                                    });
        }
    });

    connect(udevWriter, &QProcess::errorOccurred, this,
            [this](QProcess::ProcessError error){

        setLinuxUdevPopupProceedButtonEnabled(true);

        qCritical() << "Failed to start" << LINUX_TERMINAL << ". Error Code:" << error;

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
                               QStringLiteral("%1/keyboards").arg(QDir::currentPath()),
                               QStringLiteral(LINUX_UDEV_RULES_PATH),
                               outputPath, exitOK, exitKeyboardsReadFailed, exitRuleWriteFailed));
}

void MainWindowController::setLinuxUdevPopupProceedButtonEnabled(const bool &linuxUdevPopupProceedButtonEnabled)
{
    if (m_linuxUdevPopupProceedButtonEnabled == linuxUdevPopupProceedButtonEnabled) return;

    m_linuxUdevPopupProceedButtonEnabled = linuxUdevPopupProceedButtonEnabled;
    emit linuxUdevPopupProceedButtonEnabledChanged();
}

void MainWindowController::openKeyboardsSupportStatusList()
{
    QDesktopServices::openUrl(QUrl(QStringLiteral(KEYBOARDS_SUPPORT_STATUS_LIST)));
}

void MainWindowController::showFatalError(const QString& title, const QString& message)
{
    EnhancedDialog d{title, message};

    qCritical() << message;

    d.setOnAccepted([this](){
        setCloseToSystemTrayIcon(false);
        quit();
    });

    emit loadEnhancedDialog(d);
}
