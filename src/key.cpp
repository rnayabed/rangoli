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

#include "key.h"

#include <QDebug>

Key::Key()
{}

Key::Key(const int &bufferIndex, const KeyCode::Code& keyCode,
         const int& topX, const int& topY,
         const int& bottomX, const int& bottomY) :
    bufferIndex{bufferIndex}, keyCode{keyCode},
    topX{topX}, topY{topY},
    bottomX{bottomX}, bottomY{bottomY}
{}

Key::~Key()
{}
