/*
 * Copyright (C) 2026 Rangoli contributors
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

#include "singleinstance.h"

#include <QDir>
#include <QLocalSocket>
#include <QStandardPaths>

using namespace Qt::Literals::StringLiterals;

static QString lockPath()
{
#ifdef Q_OS_LINUX
    // Per user and cleared at logout
    QString directory = QStandardPaths::writableLocation(QStandardPaths::RuntimeLocation);
    if (directory.isEmpty()) directory = QDir::tempPath();
#else
    QString directory = QDir::tempPath();
#endif

    return QStringLiteral("%1/rangoli-%2.lock")
            .arg(directory, qEnvironmentVariable("USER", qEnvironmentVariable("USERNAME")));
}

SingleInstance::SingleInstance(QObject *parent)
    : QObject{parent},
      m_lock{lockPath()}
{
    // Only a lock whose process has died counts as stale, however old it is
    m_lock.setStaleLockTime(0);

    connect(&m_server, &QLocalServer::newConnection, this, [this](){
        while (QLocalSocket* socket = m_server.nextPendingConnection())
        {
            connect(socket, &QLocalSocket::disconnected, socket, &QObject::deleteLater);
            emit showRequested();
        }
    });
}

bool SingleInstance::tryLock()
{
    if (m_lock.tryLock(0)) return true;

    // Only refuse to start when another copy really holds the lock. If the lock
    // can't be made at all (permissions, odd file systems), start anyway.
    return m_lock.error() != QLockFile::LockFailedError;
}

void SingleInstance::showRunningInstance()
{
    QLocalSocket socket;
    socket.connectToServer(serverName());

    if (socket.waitForConnected(1000))
    {
        socket.disconnectFromServer();
    }
}

void SingleInstance::listen()
{
    // This copy holds the lock, so any existing server is left over from a crash
    QLocalServer::removeServer(serverName());
    m_server.setSocketOptions(QLocalServer::UserAccessOption);

    if (!m_server.listen(serverName()))
    {
        qWarning() << "Unable to listen for other Rangoli copies:" << m_server.errorString();
    }
}

QString SingleInstance::serverName()
{
    return QStringLiteral("rangoli-%1").arg(qEnvironmentVariable("USER", qEnvironmentVariable("USERNAME")));
}
