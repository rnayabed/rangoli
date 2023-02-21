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

#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <QtGlobal>
#include <QObject>
#include <QList>
#include <QPoint>

#include "key.h"

struct Keyboard
{
    Keyboard(const unsigned short& pid,
             const QString& path, const QString& name,
             const QList<Key>& keys, const bool& rgb, const bool& keyMapEnabled,
             const int& topLeftX, const int& topLeftY,
             const int& bottomRightX, const int& bottomRightY);
    ~Keyboard();

    QString imagePath;
    unsigned short pid;
    QString path;
    QString name;
    QList<Key> keys;

    bool rgb;
    bool keyMapEnabled;

    int topLeftX;
    int topLeftY;
    int bottomRightX;
    int bottomRightY;
};

#endif // KEYBOARD_H
