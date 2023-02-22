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

#include "hidconnectionworker.h"

#include <QtGlobal>
#include <QVariantList>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "keyboardconfiguratorcontroller.h"

using namespace Qt::Literals::StringLiterals;

HIDConnectionWorker::HIDConnectionWorker(QObject *parent)
    : QObject{parent}
{
}

HIDConnectionWorker::~HIDConnectionWorker()
{
    if (m_HIDInitSuccessful)
    {
        hid_exit();
    }
}

void HIDConnectionWorker::init()
{
    m_HIDInitSuccessful = ! hid_init();

    emit initDone(m_HIDInitSuccessful);
}

void HIDConnectionWorker::refreshKeyboards(QPointer<KeyboardModel> connectedKeyboards)
{
    struct hid_device_info* devs = hid_enumerate(0x258A, 0x0);

    struct hid_device_info* devIterator = devs;

    QList<KeyboardUSBID> disconnectedHIDPIDs = connectedKeyboards->keyboardUSBIDs();
    QList<KeyboardUSBID> connectedHIDPIDs;

    while (devIterator)
    {
        auto usbID = KeyboardUSBID{devIterator->vendor_id, devIterator->product_id};

        QFile configFile{QStringLiteral("keyboards/%1/configs/%2.json")
                    .arg(QString::number(devIterator->vendor_id, 16),
                         QString::number(devIterator->product_id, 16))};

        if (configFile.exists()
        #ifdef Q_OS_WIN
                && QString(devIterator->path).contains(u"&Col05"_s, Qt::CaseInsensitive)
        #else
                && devIterator->usage == 0x0080 && devIterator->usage_page == 0x0001
        #endif
                && !connectedHIDPIDs.contains(usbID))
        {
            connectedHIDPIDs << usbID;

            if (disconnectedHIDPIDs.contains(usbID))
            {
                disconnectedHIDPIDs.removeAll(usbID);
                continue;
            }

            if (!configFile.open(QIODevice::ReadOnly))
            {
                emit fatalErrorOccured(QString(tr("Unable to open configuration file of keyboard with VID '%1' and PID '%2'."))
                                      .arg(QString::number(devIterator->vendor_id, 16),
                                           QString::number(devIterator->product_id, 16)));

                return;
            }

            if (!QFile(QStringLiteral("keyboards/%1/images/%2.png")
                       .arg(QString::number(devIterator->vendor_id, 16),
                            QString::number(devIterator->product_id, 16))).exists())
            {
                emit fatalErrorOccured(QString(tr("Cannot find image of keyboard with VID '%1' and PID '%2'."))
                                      .arg(QString::number(devIterator->vendor_id, 16),
                                           QString::number(devIterator->product_id, 16)));

                return;
            }

            QJsonDocument configDocument{QJsonDocument::fromJson(configFile.readAll())};

            QJsonObject configObj = configDocument.object();

            QString name = configObj["name"].toString();

            QJsonArray topObj = configObj["top"].toArray();

            QJsonArray bottomObj = configObj["bottom"].toArray();

            QList<Key> keys;
            bool keyMapEnabled = true;

            QJsonArray keyObjs = configObj["keys"].toArray();

            for (auto&& keyObjRef : keyObjs)
            {
                QJsonObject keyObj = keyObjRef.toObject();

                QJsonArray keyTopObj = keyObj["top"].toArray();
                QJsonArray keyBottomObj = keyObj["bottom"].toArray();

                auto keyCodeStr = keyObj["keyCode"].toString();
                auto bufferIndex = keyObj["bIndex"].toInt();

                KeyCode::Code keyCode = KeyCode::CodeStrings[keyCodeStr];

                if (keyCode == KeyCode::Code::Key_Invalid)
                {
                    qDebug() << "Ignoring invalid key code" << keyCodeStr
                             << "with buffer index" << bufferIndex
                             << "in keyboard" << name;
                    continue;
                }

                if (KeyCode::isUnknownKey(keyCode))
                {
                    keyMapEnabled = false;
                }

                Key ll{bufferIndex,
                            keyCode,
                            keyTopObj[0].toInt(), keyTopObj[1].toInt(),
                            keyBottomObj[0].toInt(), keyBottomObj[1].toInt()};

                keys << ll;
            }

            emit keyboardConnected(
                        Keyboard { usbID,
                                   QString(devIterator->path), name,
                                   keys, configObj["rgb"].toBool(), keyMapEnabled,
                                   topObj[0].toInt(), topObj[1].toInt(),
                                   bottomObj[0].toInt(), bottomObj[1].toInt() });

            configFile.close();
        }

        devIterator = devIterator->next;
    }

    hid_free_enumeration(devs);

    for (auto keyboard : disconnectedHIDPIDs)
    {
        emit keyboardDisconnected(keyboard);
    }

    emit keyboardsScanComplete();
}

void HIDConnectionWorker::sendData(const QString &path, unsigned char** buffers, int bufferLength)
{
    hid_device* handle = hid_open_path(qPrintable(path));

    if (!handle)
    {
        emit failedToSendData(tr("Failed to open keyboard device."));
        return;
    }

    for(int i = 0; i < bufferLength; i++)
    {
        if (hid_send_feature_report(handle, buffers[i], KeyboardConfiguratorController::BufferSize)
                != KeyboardConfiguratorController::BufferSize)
        {
            emit failedToSendData(tr("Failed to send feature report to keyboard."));
            return;
        }

        delete [] buffers[i];
    }

    delete [] buffers;

    emit dataSentSuccessfully();

    hid_close(handle);
}
