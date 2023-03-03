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

#include <QGuiApplication>
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

using namespace Qt::Literals::StringLiterals;

void messageHandler(QtMsgType, const QMessageLogContext &, const QString &);
void closeLogger();

QFile logFile{QStringLiteral("%1/rangoli.log").arg(QDir::homePath())};
QTextStream logStream;

int main(int argc, char *argv[])
{
    MainWindowController mainWindowController;
    KeyboardConfiguratorController keyboardConfiguratorController;
    SettingsController settingsController;

    logFile.open(QIODevice::WriteOnly);
    logStream.setDevice(&logFile);

    qInstallMessageHandler(messageHandler);
    QGuiApplication app(argc, argv);

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

    QQmlApplicationEngine engine;

    QObject::connect(&engine, &QQmlApplicationEngine::quit, closeLogger);

    engine.setContextForObject(&mainWindowController, engine.rootContext());
    engine.rootContext()->setContextProperty(u"mainWindowController"_s, &mainWindowController);

    engine.setContextForObject(&keyboardConfiguratorController, engine.rootContext());
    engine.rootContext()->setContextProperty(u"keyboardConfiguratorController"_s, &keyboardConfiguratorController);

    engine.setContextForObject(&settingsController, engine.rootContext());
    engine.rootContext()->setContextProperty(u"settingsController"_s, &settingsController);

    engine.load(u"qrc:/Rangoli/Main.qml"_s);

    return app.exec();
}

void closeLogger()
{
    qInfo() << "Exit! Bye!";
    logFile.close();
}

void messageHandler(QtMsgType messageType, const QMessageLogContext &context, const QString &message)
{
    Q_UNUSED(context)

    logStream << QDateTime::currentDateTime().toString(u"[dd/MM/yyyy] [hh:mm:ss] "_s);

    switch (messageType)
    {
    case QtInfoMsg:
        logStream << u"[INFO] : "_s;
        break;
    case QtDebugMsg:
        logStream << u"[DEBUG] : "_s;
        break;
    case QtWarningMsg:
        logStream << u"[WARNING] : "_s;
        break;
    case QtCriticalMsg:
        logStream << u"[CRITICAL] : "_s;
        break;
    case QtFatalMsg:
        logStream << u"[FATAL] : "_s;
        break;
    }

    logStream << message << Qt::endl;
}
