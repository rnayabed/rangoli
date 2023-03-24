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
#include <QJsonArray>

#include "keyboardconfiguratorcontroller.h"
#include "mainwindowcontroller.h"

using namespace Qt::Literals::StringLiterals;

HIDConnectionWorker::HIDConnectionWorker(QObject *parent)
    : QObject{parent}, m_HIDInitSuccessful{false}
{
}

void HIDConnectionWorker::init()
{
    qDebug() << "Init HID Connection Worker";

#ifdef Q_OS_WINDOWS
    if (!initHIDCol())
    {
        emit HIDColInitFailed();
        return;
    }
#endif

    m_HIDInitSuccessful = ! hid_init();
    if (!m_HIDInitSuccessful)
    {
        emit HIDAPIInitFailed();
        return;
    }

    emit initSuccessful();
}

void HIDConnectionWorker::exit()
{
    qDebug() << "Exit HID Connection Worker";

    if (m_HIDInitSuccessful)
    {
        hid_exit();
    }
}

void HIDConnectionWorker::refreshKeyboards(QPointer<KeyboardModel> connectedKeyboards)
{
    qInfo() << "Refresh keyboards";

    struct hid_device_info* devs = hid_enumerate(0x0, 0x0);

    struct hid_device_info* devIterator = devs;

    QList<KeyboardUSBID> disconnectedHIDPIDs = connectedKeyboards->keyboardUSBIDs();
    QList<KeyboardUSBID> connectedHIDPIDs;

#ifdef Q_OS_WIN
    QString defaultCol = m_hidColConfig["default"].toString();
#endif

    while (devIterator)
    {
        qDebug() << "HID Device found: " << devIterator->path;

        auto usbID = KeyboardUSBID{devIterator->vendor_id, devIterator->product_id};

        QFile configFile{QStringLiteral("keyboards/%1/configs/%2.json")
                    .arg(QString::number(usbID.vid, 16),
                         QString::number(usbID.pid, 16))};

        if (configFile.exists()
#ifdef Q_OS_WIN
                && QString(devIterator->path).contains(QStringLiteral("&Col%1")
                                                       .arg(m_hidColConfig[QStringLiteral("%1:%2")
                                                            .arg(QString::number(usbID.vid, 16),
                                                                 QString::number(usbID.pid, 16))].toString(defaultCol)),
                                                       Qt::CaseInsensitive)
#else
                && devIterator->usage == 0x0080 && devIterator->usage_page == 0x0001
#endif
                && !connectedHIDPIDs.contains(usbID))
        {
            connectedHIDPIDs << usbID;

            if (disconnectedHIDPIDs.contains(usbID))
            {
                qDebug() << "Remove" << usbID << "from disconnected hidpids";
                disconnectedHIDPIDs.removeAll(usbID);
                continue;
            }

            if (!configFile.open(QIODevice::ReadOnly))
            {
                qCritical() << "Unable to open config file for" << usbID;

                MainWindowController::showEnhancedDialog(
                            this,
                            EnhancedDialog {
                                tr("Error"),
                                tr("Unable to open configuration file of keyboard with VID '%1' and PID '%2'.")
                                          .arg(QString::number(usbID.vid, 16),
                                               QString::number(usbID.pid, 16))
                            });

                return;
            }

            if (!QFile(QStringLiteral("keyboards/%1/images/%2.png")
                       .arg(QString::number(usbID.vid, 16),
                            QString::number(usbID.pid, 16))).exists())
            {
                qCritical() << "Unable to find image for" << usbID;

                MainWindowController::showEnhancedDialog(
                            this,
                            EnhancedDialog {
                                tr("Error"),
                                tr("Unable to find image of keyboard with VID '%1' and PID '%2'.")
                                          .arg(QString::number(usbID.vid, 16),
                                               QString::number(usbID.pid, 16))
                            });

                return;
            }

            QJsonDocument configDocument{QJsonDocument::fromJson(configFile.readAll())};

            configFile.close();

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

void HIDConnectionWorker::sendData(const QString &path, unsigned char** buffers, int buffersLength)
{
    qInfo() << "Sending" << buffersLength << "feature reports to keyboard at HID path" << path;
    qDebug() << "Each buffer is of size" << KeyboardConfiguratorController::BufferSize;

    hid_device* handle = hid_open_path(qPrintable(path));

    if (!handle)
    {
        qCritical() << "Unable to open device";
#ifdef Q_OS_MACOS
        emit failedToSendData(tr("Insufficient Permissions.\n%1")
                              .arg(MainWindowController::macOSPermissionNotice()));
#else
        emit failedToSendData(tr("Failed to open keyboard device. "
                                 "Make sure you have sufficient permissions."));
#endif
        return;
    }

    for(int i = 0; i < buffersLength; i++)
    {
        if (hid_send_feature_report(handle, buffers[i], KeyboardConfiguratorController::BufferSize)
                != KeyboardConfiguratorController::BufferSize)
        {
            qCritical() << "Unable to send report" << i;

            emit failedToSendData(tr("Failed to send feature report to keyboard. "
                                     "Make sure you have sufficient permissions."));
            return;
        }

        delete [] buffers[i];
    }

    delete [] buffers;

    emit dataSentSuccessfully();

    hid_close(handle);
}

bool HIDConnectionWorker::initHIDCol()
{
    QFile hidColConfigFile{u"keyboards/hid-col.json"_s};

    if (!hidColConfigFile.open(QIODevice::ReadOnly))
    {
        return false;
    }

    QJsonDocument configDocument{QJsonDocument::fromJson(hidColConfigFile.readAll())};

    hidColConfigFile.close();

    m_hidColConfig = configDocument.object();

    return true;
}
