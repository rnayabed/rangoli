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

#ifndef HIDCONNECTIONWORKER_H
#define HIDCONNECTIONWORKER_H

#include <QDebug>
#include <QFile>
#include <QObject>
#include <QXmlStreamReader>
#include <QPointer>
#include <hidapi.h>
#include <QJsonObject>
#include <list>

#include "keyboard.h"
#include "keyboardmodel.h"

class HIDConnectionWorker : public QObject
{
    Q_OBJECT
public:
    explicit HIDConnectionWorker(QObject *parent = nullptr);

signals:
    void HIDColInitFailed();
    void HIDAPIInitFailed();
    void initSuccessful();

    void keyboardConnected(const Keyboard& keyboard);
    void keyboardDisconnected(const KeyboardUSBID& id);
    void keyboardsScanComplete();
    void dataSentSuccessfully();
    void failedToSendData(const QString& message);

public slots:
    void init();
    void exit();

    void refreshKeyboards(QPointer<KeyboardModel> connectedKeyboards);

    void sendData(const QString& path, unsigned char** buffers, int buffersLength);

private:
    bool m_HIDInitSuccessful;
    QJsonObject m_hidColConfig;

    bool initHIDCol();
};

#endif // HIDCONNECTIONWORKER_H
