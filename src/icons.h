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

#ifndef ICONS_H
#define ICONS_H

#include <QObject>
#include <QString>
#include <QStringLiteral>
#include <QQmlEngine>

class Icons : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

public:
    enum Icon {
        Add,
        ArrowBack,
        BugReport,
        Delete,
        Donate,
        Done,
        Edit,
        Git,
        Help,
        Home,
        Keyboard,
        Light,
        Rangoli,
        Refresh,
        Reset,
        Save,
        Settings,
        Website
    };

    Q_ENUM(Icon)

    Q_INVOKABLE static QString get(const int& icon);
};

#endif // ICONS_H
