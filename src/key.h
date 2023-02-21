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

#ifndef KEY_H
#define KEY_H

#include <QObject>
#include <QtGlobal>
#include <QString>
#include <QPoint>

#include "keycode.h"

class Key
{
    Q_GADGET
    Q_PROPERTY(int bufferIndex MEMBER bufferIndex)

    Q_PROPERTY(int topX MEMBER topX)
    Q_PROPERTY(int topY MEMBER topY)
    Q_PROPERTY(int bottomX MEMBER bottomX)
    Q_PROPERTY(int bottomY MEMBER bottomY)

public:

    Key();

    Key(const int& bufferIndex, const KeyCode::Code& keyCode,
        const int& topX, const int& topY,
        const int& bottomX, const int& bottomY);
    ~Key();

    int bufferIndex;
    KeyCode::Code keyCode;
    int topX;
    int topY;
    int bottomX;
    int bottomY;

};


#endif // KEY_H
