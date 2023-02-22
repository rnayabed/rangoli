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

#include "hidconnection.h"

HIDConnection::HIDConnection(QObject *parent)
    : QObject{parent}, m_worker{new HIDConnectionWorker()}
{
    m_worker->moveToThread(&m_thread);

    connect(&m_thread, &QThread::started, m_worker, &HIDConnectionWorker::init);

    connect(m_worker, &HIDConnectionWorker::initDone, this, [this](const bool& success){
        emit initDone(success);
    });

    connect(this, &HIDConnection::refreshKeyboards,
            m_worker, &HIDConnectionWorker::refreshKeyboards);

    connect(m_worker, &HIDConnectionWorker::keyboardConnected,
            this, [this](const Keyboard& keyboard){
        emit keyboardConnected(keyboard);
    });

    connect(m_worker, &HIDConnectionWorker::keyboardDisconnected,
            this, [this](const KeyboardUSBID& id){
        emit keyboardDisconnected(id);
    });

    connect(m_worker, &HIDConnectionWorker::keyboardsScanComplete,
            this, [this](){
        emit keyboardsScanComplete();
    });

    connect(this, &HIDConnection::sendData,
            m_worker, &HIDConnectionWorker::sendData);

    connect(m_worker, &HIDConnectionWorker::dataSentSuccessfully,
            this, [this](){
        emit dataSentSuccessfully();
    });

    connect(m_worker, &HIDConnectionWorker::failedToSendData,
            this, [this](const QString& message){
        emit failedToSendData(message);
    });

    connect(m_worker, &HIDConnectionWorker::fatalErrorOccured,
            this, [this](const QString& message){
        emit fatalErrorOccured(message);
    });
}

HIDConnection &HIDConnection::getInstance()
{
    static HIDConnection instance;
    return instance;
}

void HIDConnection::start()
{
    m_thread.start();
}

void HIDConnection::stop()
{
    if (m_thread.isRunning())
    {
        m_thread.quit();
        m_thread.wait();
        delete m_worker;
    }
}


