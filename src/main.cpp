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

#include <QApplication>
#include <QQmlApplicationEngine>
#include <QLocale>
#include <QIcon>
#include <QTranslator>
#include <QQmlContext>
#include <QQuickWindow>
#include <QDir>
#include "keyboardconfiguratorcontroller.h"
#include "mainwindowcontroller.h"
#include "settingscontroller.h"
#include "icons.h"
#include "messagehandler.h"
#include "hidconnection.h"
#include "systemtray.h"
#include "singleinstance.h"

#ifdef Q_OS_UNIX
#include <csignal>
#include <sys/socket.h>
#include <unistd.h>
#include <QSocketNotifier>

static int quitSignalFds[2];

static void handleQuitSignal(int)
{
    char c = 1;
    [[maybe_unused]] auto written = ::write(quitSignalFds[0], &c, sizeof(c));
}
#endif

using namespace Qt::Literals::StringLiterals;

int main(int argc, char *argv[])
{
    // Checked before the log opens, which would wipe the running copy's log
    SingleInstance singleInstance;

    if (!singleInstance.tryLock())
    {
        QCoreApplication app(argc, argv);

        qInfo("Rangoli is already running");

        // Starting at login gives way quietly, anything else brings the running copy forward
        if (!app.arguments().contains(u"start_to_system_tray_icon"_s))
        {
            SingleInstance::showRunningInstance();
        }

        return 0;
    }

    if (MessageHandler::init())
    {
        qInstallMessageHandler(MessageHandler::handler);
    }
    else
    {
        qCritical("Unable to start logging to file!");
    }

    qInfo() << "Rangoli" << VERSION;

    if (QQuickWindow::graphicsApi() == QSGRendererInterface::Software)
    {
        qInfo() << "Graphics: Software";
    }
    else
    {
        qInfo() << "Graphics: Hardware";
    }

    MainWindowController mainWindowController;
    KeyboardConfiguratorController keyboardConfiguratorController;
    SettingsController settingsController;

    QApplication app(argc, argv);

    qInfo() << "Platform:" << app.platformName();
    qInfo() << "Qt" << qVersion();

    app.setOrganizationName(u"rnayabed"_s);
    app.setOrganizationDomain(u"rnayabed.github.io"_s);
    app.setApplicationName(u"rangoli"_s);
    app.setApplicationDisplayName(u"Rangoli"_s);
    app.setApplicationVersion(QString::number(VERSION));
    app.setWindowIcon(QIcon(Icons::get(Icons::Rangoli).remove(0,3)));

#ifdef Q_OS_MACOS
    QDir::setCurrent(QStringLiteral("%1/../../..").arg(app.applicationDirPath()));
#else
    QDir::setCurrent(app.applicationDirPath());
#endif

    // Live lighting has to hand the keyboard back before the HID thread goes away
    QObject::connect(&app, &QCoreApplication::aboutToQuit, &app, [&keyboardConfiguratorController](){
        keyboardConfiguratorController.stopPlaybackBlocking();
        HIDConnection::getInstance().stop();
    });

#ifdef Q_OS_UNIX
    // Logging out or shutting down sends SIGTERM; turn it into a normal quit
    if (::socketpair(AF_UNIX, SOCK_STREAM, 0, quitSignalFds) == 0)
    {
        auto notifier = new QSocketNotifier(quitSignalFds[1], QSocketNotifier::Read, &app);
        QObject::connect(notifier, &QSocketNotifier::activated, &app, &QCoreApplication::quit);

        struct sigaction action {};
        action.sa_handler = handleQuitSignal;
        sigemptyset(&action.sa_mask);
        action.sa_flags = SA_RESTART;

        sigaction(SIGTERM, &action, nullptr);
        sigaction(SIGINT, &action, nullptr);
        sigaction(SIGHUP, &action, nullptr);
    }
#endif

    QQmlApplicationEngine engine;

    QObject::connect(&engine, &QQmlApplicationEngine::quit, MessageHandler::close);

    engine.setContextForObject(&mainWindowController, engine.rootContext());
    engine.rootContext()->setContextProperty(u"mainWindowController"_s, &mainWindowController);

    engine.setContextForObject(&keyboardConfiguratorController, engine.rootContext());
    engine.rootContext()->setContextProperty(u"keyboardConfiguratorController"_s, &keyboardConfiguratorController);

    SystemTray systemTray(&mainWindowController, &keyboardConfiguratorController);
    engine.rootContext()->setContextProperty(u"systemTray"_s, &systemTray);

    QObject::connect(&singleInstance, &SingleInstance::showRequested,
                     &systemTray, &SystemTray::showWindowRequested);
    singleInstance.listen();

    engine.setContextForObject(&settingsController, engine.rootContext());
    engine.rootContext()->setContextProperty(u"settingsController"_s, &settingsController);

    engine.load(u"qrc:/Rangoli/Main.qml"_s);

    return app.exec();
}
