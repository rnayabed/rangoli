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

#ifndef SINGLEINSTANCE_H
#define SINGLEINSTANCE_H

#include <QLocalServer>
#include <QLockFile>
#include <QObject>

// Keeps Rangoli to one copy per user. Two copies streaming live lighting and
// saves to the same keyboard flood it with reports until it stops responding.
// The lock is per user, not per settings folder, so an installed copy, an
// AppImage and a test build all count.
class SingleInstance : public QObject
{
    Q_OBJECT

public:
    explicit SingleInstance(QObject* parent = nullptr);

    // False when another copy is already running. Works before QApplication exists.
    bool tryLock();

    // Asks the running copy to show its window
    static void showRunningInstance();

    // Lets later copies reach this one. Needs the application object.
    void listen();

signals:
    void showRequested();

private:
    QLockFile m_lock;
    QLocalServer m_server;

    static QString serverName();
};

#endif // SINGLEINSTANCE_H
