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

#include "startatboot.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QPointer>

using namespace Qt::Literals::StringLiterals;

bool StartAtBoot::createStarter()
{
#if defined(Q_OS_LINUX)
    QDir serviceDir{QStringLiteral("%1/.local/share/systemd/user/").arg(QDir::homePath())};

    if (!serviceDir.exists())
    {
        if (!serviceDir.mkpath("."))
        {
            return false;
        }
    }

    QFile service{serviceDir.absoluteFilePath(u"rangoli.service"_s)};

    if (service.open(QIODevice::WriteOnly))
    {
        QTextStream ts{&service};

        ts << u"[Unit]\n"
              "Description=Rangoli\n"
              "[Service]\n"
              "Type=simple\n"
              "ExecStartPre=/bin/sleep 3\n"
              "Environment=\"DISPLAY=:0\"\n"
              "ExecStart=\""_s << QCoreApplication::arguments().at(0) << u"\" start_to_system_tray_icon\n"
              "[Install]\n"
              "WantedBy=default.target\n"_s;

        service.close();

        QPointer<QProcess> daemonReload{new QProcess};
        QObject::connect(daemonReload, &QProcess::finished, StartAtBoot::linuxSystemDDaemonReloadFinished);
        QObject::connect(daemonReload, &QProcess::finished, daemonReload, &QProcess::deleteLater);

        daemonReload->start(u"systemctl --user daemon-reload"_s);

        return true;
    }
    else
    {
        return false;
    }
#elif defined(Q_OS_WIN)
    QFile starter{QStringLiteral("%1/Microsoft/Windows/Start Menu/Programs/Startup/rangoli.vbs").arg(qgetenv("APPDATA"))};

    if (starter.open(QIODevice::WriteOnly))
    {
        QTextStream ts{&starter};

        ts << u"CreateObject(\"WScript.Shell\").Run chr(34) & \""_s << QCoreApplication::arguments().at(0) << u" start_to_system_tray_icon\"\r\n"_s;

        starter.close();

        return true;
    }
    else
    {
        return false;
    }
#elif defined(Q_OS_MACOS)
    QString plistPath{QStringLiteral("%1/Library/LaunchAgents/io.github.rnayabed.rangoli.plist").arg(QDir::homePath())};

    QFile plist{plistPath};

    if (plist.open(QIODevice::WriteOnly))
    {
        QTextStream ts{&plist};

        ts << u"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
              "<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n"
              "<plist version=\"1.0\">\n"
              "  <dict>\n"
              "    <key>Label</key>\n"
              "    <string>io.github.rnayabed.rangoli</string>\n"
              "    <key>Program</key>\n"
              "    <string>"_s << QCoreApplication::arguments().at(0) << u"</string>\n"
              "    <key>ProgramArguments</key>\n"
              "    <array><string>start_to_system_tray_icon</string></array>\n"
              "    <key>RunAtLoad</key>\n"
              "    <true/>\n"
              "  </dict>\n"
              "</plist>"_s;

        plist.close();

        QPointer<QProcess> load{new QProcess};
        QObject::connect(load, &QProcess::finished, StartAtBoot::macOSLaunchCtlLoadFinished);
        QObject::connect(load, &QProcess::finished, load, &QProcess::deleteLater);
        load->start(QStringLiteral("launchctl load -w '%1'").arg(plistPath));

        return true;
    }
    else
    {
        return false;
    }
#endif
    return false;
}

bool StartAtBoot::deleteStarter()
{
#if defined(Q_OS_LINUX)
    bool result = QFile{QStringLiteral("%1/.local/share/systemd/user/rangoli.service").arg(QDir::homePath())}.remove();

    QPointer<QProcess> systemDReload{new QProcess};
    QObject::connect(systemDReload, &QProcess::finished, systemDReload, &QProcess::deleteLater);
    systemDReload->start(u"systemctl --user daemon-reload"_s);

    return result;
#elif defined(Q_OS_WIN)
    QFile starter{QStringLiteral("%1/Microsoft/Windows/Start Menu/Programs/Startup/rangoli.vbs").arg(qgetenv("APPDATA"))};

    return starter.remove();
#elif defined(Q_OS_MACOS)
    QString plistPath{QStringLiteral("%1/Library/LaunchAgents/io.github.rnayabed.rangoli.plist").arg(QDir::homePath())};

    bool result = QFile{plistPath}.remove();

    QPointer<QProcess> unload{new QProcess};
    QObject::connect(unload, &QProcess::finished, unload, &QProcess::deleteLater);
    unload->start(QStringLiteral("launchctl unload -w '%1'").arg(plistPath));

    return result;
#endif
    return false;
}

void StartAtBoot::linuxSystemDDaemonReloadFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitCode)
    Q_UNUSED(exitStatus)

    QPointer<QProcess> enableService{new QProcess};
    QObject::connect(enableService, &QProcess::finished, enableService, &QProcess::deleteLater);
    enableService->start(u"systemctl --user enable rangoli.service"_s);
}

void StartAtBoot::macOSLaunchCtlLoadFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitCode)
    Q_UNUSED(exitStatus)

    QPointer<QProcess> stop{new QProcess};
    QObject::connect(stop, &QProcess::finished, stop, &QProcess::deleteLater);
    stop->start(u"launchctl stop io.github.rnayabed.rangoli"_s);
}
