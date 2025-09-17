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

#ifndef MAINWINDOWCONTROLLER_H
#define MAINWINDOWCONTROLLER_H

#include <QFile>
#include <QObject>
#include <QTimer>
#include <QQmlEngine>
#include <QXmlStreamReader>

#include <QQmlContext>

#include <QPointer>
#include <hidapi.h>
#include <QtQml>

#include "enhanceddialog.h"
#include "keyboardmodel.h"
#include "keyboardconfiguratorcontroller.h"

class MainWindowController : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(bool interruptClose READ interruptClose WRITE setInterruptClose NOTIFY interruptCloseChanged)
    Q_PROPERTY(int toolTipDelay READ toolTipDelay CONSTANT)
    Q_PROPERTY(int toolTipTimeout READ toolTipTimeout CONSTANT)
    Q_PROPERTY(KeyboardModel* connectedKeyboards READ connectedKeyboardsRaw CONSTANT)
    Q_PROPERTY(bool alwaysShowSystemTrayIcon READ alwaysShowSystemTrayIcon WRITE setAlwaysShowSystemTrayIcon NOTIFY alwaysShowSystemTrayIconChanged)
    Q_PROPERTY(bool closeToSystemTrayIcon READ closeToSystemTrayIcon WRITE setCloseToSystemTrayIcon NOTIFY closeToSystemTrayIconChanged)
    Q_PROPERTY(bool startToSystemTrayIcon READ startToSystemTrayIcon CONSTANT)
    Q_PROPERTY(bool keyboardsScanning READ keyboardsScanning NOTIFY keyboardsScanningChanged)
    Q_PROPERTY(QString linuxUdevRulesPath READ linuxUdevRulesPath CONSTANT)
    Q_PROPERTY(bool linuxUdevPopupProceedButtonEnabled READ linuxUdevPopupProceedButtonEnabled WRITE setLinuxUdevPopupProceedButtonEnabled NOTIFY linuxUdevPopupProceedButtonEnabledChanged)
    Q_PROPERTY(bool isWindows READ isWindows CONSTANT)

public:
    explicit MainWindowController(QObject *parent = nullptr);

    static void showEnhancedDialog(QObject* object, const EnhancedDialog &dialog);

    enum Theme: int {
        System,
        Light,
        Dark
    };

    static QString macOSPermissionNotice();

    const bool& interruptClose();

    const int& toolTipDelay();
    const int& toolTipTimeout();

    KeyboardModel* connectedKeyboardsRaw();
    QPointer<KeyboardModel> connectedKeyboards();

    bool& alwaysShowSystemTrayIcon();
    bool& closeToSystemTrayIcon();
    bool& startToSystemTrayIcon();

    bool& keyboardsScanning();
    bool& linuxUdevPopupProceedButtonEnabled();

    bool isWindows();

    QString linuxUdevRulesPath();

    QPointer<ProfileModel> profiles();

signals:
    void show();
    void hide();
    void close();
    void interruptCloseChanged();
    void adjustOptionsSize(const int& length);
    void loadEnhancedDialog(const EnhancedDialog& dialog);

    void setSystemTheme();
    void setLightTheme();
    void setDarkTheme();

    void setAccentColour(const QColor& accentColour);

    void alwaysShowSystemTrayIconChanged();
    void closeToSystemTrayIconChanged();

    void keyboardsScanningChanged();

    void openLinuxUdevPopup();
    void closeLinuxUdevPopup();

    void linuxUdevPopupProceedButtonEnabledChanged();

public slots:
    void init();
    void quit();
    void refreshKeyboards();
    void loadKeyboardWithDefaultProfile(const int &index);
    void loadKeyboard(const int& index);
    void setInterruptClose(const bool& interruptClose);
    void closeInterrupted();
    void setTheme(const int& theme);
    void setAlwaysShowSystemTrayIcon(const bool& alwaysShowSystemTrayIcon);
    void setCloseToSystemTrayIcon(const bool& closeToSystemTrayIcon);

    void launchLinuxUdevWriter();

    void setLinuxUdevPopupProceedButtonEnabled(const bool& linuxUdevPopupProceedButtonEnabled);

    void openKeyboardsSupportStatusList();

private slots:
    void showFatalError(const QString& title, const QString& message);

private:
    QPointer<KeyboardModel> m_connectedKeyboards;
    QPointer<ProfileModel> m_profiles;
    bool m_interruptClose;
    bool m_confirmQuitDialogVisible;
    QPointer<KeyboardConfiguratorController> m_keyboardConfiguratorController;

    bool m_alwaysShowSystemTrayIcon;
    bool m_closeToSystemTrayIcon;
    bool m_startToSystemTrayIcon;

    bool m_keyboardsScanning;

    const int m_toolTipDelay;
    const int m_toolTipTimeout;

    bool m_linuxUdevPopupProceedButtonEnabled;
    std::unique_ptr<QProcess> udevWriterProcess;
};

#endif // MAINWINDOWCONTROLLER_H
