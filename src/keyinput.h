/*
 * Copyright (C) 2026 Rangoli contributors
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

#ifndef KEYINPUT_H
#define KEYINPUT_H

#include <QChar>

#include "keycode.h"

// Maps typed text and physical key presses to keyboard key codes
namespace KeyInput
{
    // The key a character is typed with, ignoring Shift (e.g. '!' -> 1)
    KeyCode::Code codeForCharacter(const QChar& character);

    // Whether Shift has to be held to type a character on a US layout
    bool needsShift(const QChar& character);

    // The key behind a native scan code from a Qt key event
    KeyCode::Code codeForNativeScanCode(const quint32& nativeScanCode);
}

#endif // KEYINPUT_H
