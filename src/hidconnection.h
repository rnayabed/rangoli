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

#ifndef HIDCONNECTION_H
#define HIDCONNECTION_H

#include <QObject>
#include <QThread>
#include <QPointer>

#include <list>
#include "keyboard.h"
#include "keyboardmodel.h"
#include "hidconnectionworker.h"

class HIDConnection : public QObject
{
    Q_OBJECT
public:
    explicit HIDConnection(QObject *parent = nullptr);
    static HIDConnection& getInstance();

private:
    QThread m_thread;
    QPointer<HIDConnectionWorker> m_worker;

signals:
    void initDone(const bool& success);

    void refreshKeyboards(QPointer<KeyboardModel> connectedKeyboards);

    void keyboardConnected(const Keyboard& keyboard);
    void keyboardDisconnected(const KeyboardUSBID& id);

    void keyboardsScanComplete();

    void sendData(const QString& path, unsigned char** buffers, int bufferLength);

    void dataSentSuccessfully();
    void failedToSendData(const QString& message);

    void exit();


public slots:
    void start();
    void stop();
};

#endif // HIDCONNECTION_H
