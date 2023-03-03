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

#ifndef MESSAGEHANDLER_H
#define MESSAGEHANDLER_H

#include <QtGlobal>
#include <QFile>
#include <QTextStream>

#include <QDir>

class MessageHandler
{
public:
    MessageHandler() = delete;

    static bool init();
    static void close();
    static void handler(QtMsgType messageType,
                        const QMessageLogContext &context,
                        const QString &message);

private:
    inline static QFile m_logFile{QStringLiteral("%1/rangoli.log")
                .arg(QDir::homePath())};

    inline static QTextStream m_logStream{&m_logFile};
};


#endif // MESSAGEHANDLER_H
