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

#include "keyboard.h"

#include <QDebug>

Keyboard::Keyboard(const unsigned short &pid,
                   const QString &path, const QString &name,
                    const QList<Key>& keys, const bool& rgb, const bool& keyMapEnabled,
                   const int& topLeftX, const int& topLeftY,
                   const int& bottomRightX, const int& bottomRightY)
    : imagePath{QStringLiteral("file:keyboards/images/%1.png").arg(QString::number(pid, 16))},
      pid{pid},
      path{path}, name{name},
      keys{keys}, rgb{rgb}, keyMapEnabled{keyMapEnabled},
      topLeftX{topLeftX}, topLeftY{topLeftY},
      bottomRightX{bottomRightX}, bottomRightY{bottomRightY}
{}

Keyboard::~Keyboard()
{}
