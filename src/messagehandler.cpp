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

#include "messagehandler.h"

#include <QDateTime>
#include <QDir>

using namespace Qt::Literals::StringLiterals;

bool MessageHandler::init()
{
    return m_logFile.open(QIODevice::WriteOnly);
}

void MessageHandler::close()
{
    qInfo() << "Exit";

    m_logFile.close();
}

void MessageHandler::handler(QtMsgType messageType, const QMessageLogContext &context, const QString &message)
{
    Q_UNUSED(context)

    m_logStream << QDateTime::currentDateTime().toString(u"[dd/MM/yyyy] [hh:mm:ss] "_s);

    switch (messageType)
    {
    case QtInfoMsg:
        m_logStream << u"[INFO] : "_s;
        break;
    case QtDebugMsg:
        m_logStream << u"[DEBUG] : "_s;
        break;
    case QtWarningMsg:
        m_logStream << u"[WARNING] : "_s;
        break;
    case QtCriticalMsg:
        m_logStream << u"[CRITICAL] : "_s;
        break;
    case QtFatalMsg:
        m_logStream << u"[FATAL] : "_s;
        break;
    }

    m_logStream << message << Qt::endl;
    m_stdStream << message << Qt::endl;
}
