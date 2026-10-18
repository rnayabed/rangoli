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

#include "keyinput.h"

#include <QHash>

namespace KeyInput
{

KeyCode::Code codeForCharacter(const QChar &character)
{
    const QChar c = character.toUpper();

    if (c.isLetterOrNumber() && c.unicode() < 128)
    {
        return KeyCode::CodeStrings.value(QStringLiteral("Key_%1").arg(c), KeyCode::Key_Invalid);
    }

    static const QHash<QChar, KeyCode::Code> punctuation {
        {u' ', KeyCode::Key_Space}, {u'\n', KeyCode::Key_Enter}, {u'\t', KeyCode::Key_Tab},
        {u'`', KeyCode::Key_Back_Quote}, {u'~', KeyCode::Key_Back_Quote},
        {u'!', KeyCode::Key_1}, {u'@', KeyCode::Key_2}, {u'#', KeyCode::Key_3},
        {u'$', KeyCode::Key_4}, {u'%', KeyCode::Key_5}, {u'^', KeyCode::Key_6},
        {u'&', KeyCode::Key_7}, {u'*', KeyCode::Key_8}, {u'(', KeyCode::Key_9},
        {u')', KeyCode::Key_0},
        {u'-', KeyCode::Key_Hyphen}, {u'_', KeyCode::Key_Hyphen},
        {u'=', KeyCode::Key_Equals}, {u'+', KeyCode::Key_Equals},
        {u'[', KeyCode::Key_Left_Bracket}, {u'{', KeyCode::Key_Left_Bracket},
        {u']', KeyCode::Key_Right_Bracket}, {u'}', KeyCode::Key_Right_Bracket},
        {u'\\', KeyCode::Key_Back_Slash}, {u'|', KeyCode::Key_Back_Slash},
        {u';', KeyCode::Key_Semi_Colon}, {u':', KeyCode::Key_Semi_Colon},
        {u'\'', KeyCode::Key_Quote}, {u'"', KeyCode::Key_Quote},
        {u',', KeyCode::Key_Comma}, {u'<', KeyCode::Key_Comma},
        {u'.', KeyCode::Key_Dot}, {u'>', KeyCode::Key_Dot},
        {u'/', KeyCode::Key_Slash}, {u'?', KeyCode::Key_Slash},
    };

    return punctuation.value(c, KeyCode::Key_Invalid);
}

bool needsShift(const QChar &character)
{
    if (character.isLetter()) return character.isUpper();

    return QStringLiteral("~!@#$%^&*()_+{}|:\"<>?").contains(character);
}

KeyCode::Code codeForNativeScanCode(const quint32 &nativeScanCode)
{
#if defined(Q_OS_LINUX)
    // X11 key codes are Linux evdev codes plus 8 (see linux/input-event-codes.h)
    static const QHash<quint32, KeyCode::Code> evdev {
        {1, KeyCode::Key_Escape},
        {2, KeyCode::Key_1}, {3, KeyCode::Key_2}, {4, KeyCode::Key_3}, {5, KeyCode::Key_4},
        {6, KeyCode::Key_5}, {7, KeyCode::Key_6}, {8, KeyCode::Key_7}, {9, KeyCode::Key_8},
        {10, KeyCode::Key_9}, {11, KeyCode::Key_0},
        {12, KeyCode::Key_Hyphen}, {13, KeyCode::Key_Equals}, {14, KeyCode::Key_Backspace},
        {15, KeyCode::Key_Tab},
        {16, KeyCode::Key_Q}, {17, KeyCode::Key_W}, {18, KeyCode::Key_E}, {19, KeyCode::Key_R},
        {20, KeyCode::Key_T}, {21, KeyCode::Key_Y}, {22, KeyCode::Key_U}, {23, KeyCode::Key_I},
        {24, KeyCode::Key_O}, {25, KeyCode::Key_P},
        {26, KeyCode::Key_Left_Bracket}, {27, KeyCode::Key_Right_Bracket}, {28, KeyCode::Key_Enter},
        {29, KeyCode::Key_Left_Control},
        {30, KeyCode::Key_A}, {31, KeyCode::Key_S}, {32, KeyCode::Key_D}, {33, KeyCode::Key_F},
        {34, KeyCode::Key_G}, {35, KeyCode::Key_H}, {36, KeyCode::Key_J}, {37, KeyCode::Key_K},
        {38, KeyCode::Key_L},
        {39, KeyCode::Key_Semi_Colon}, {40, KeyCode::Key_Quote}, {41, KeyCode::Key_Back_Quote},
        {42, KeyCode::Key_Left_Shift}, {43, KeyCode::Key_Back_Slash},
        {44, KeyCode::Key_Z}, {45, KeyCode::Key_X}, {46, KeyCode::Key_C}, {47, KeyCode::Key_V},
        {48, KeyCode::Key_B}, {49, KeyCode::Key_N}, {50, KeyCode::Key_M},
        {51, KeyCode::Key_Comma}, {52, KeyCode::Key_Dot}, {53, KeyCode::Key_Slash},
        {54, KeyCode::Key_Right_Shift}, {55, KeyCode::Key_Num_Multiply}, {56, KeyCode::Key_Left_Alt},
        {57, KeyCode::Key_Space}, {58, KeyCode::Key_Caps_Lock},
        {59, KeyCode::Key_F1}, {60, KeyCode::Key_F2}, {61, KeyCode::Key_F3}, {62, KeyCode::Key_F4},
        {63, KeyCode::Key_F5}, {64, KeyCode::Key_F6}, {65, KeyCode::Key_F7}, {66, KeyCode::Key_F8},
        {67, KeyCode::Key_F9}, {68, KeyCode::Key_F10},
        {69, KeyCode::Key_Num_Lock}, {70, KeyCode::Key_Scroll_Lock},
        {71, KeyCode::Key_Num_7}, {72, KeyCode::Key_Num_8}, {73, KeyCode::Key_Num_9},
        {74, KeyCode::Key_Num_Substract},
        {75, KeyCode::Key_Num_4}, {76, KeyCode::Key_Num_5}, {77, KeyCode::Key_Num_6},
        {78, KeyCode::Key_Num_Add},
        {79, KeyCode::Key_Num_1}, {80, KeyCode::Key_Num_2}, {81, KeyCode::Key_Num_3},
        {82, KeyCode::Key_Num_0}, {83, KeyCode::Key_Num_Decimal_Point},
        {87, KeyCode::Key_F11}, {88, KeyCode::Key_F12},
        {96, KeyCode::Key_Num_Enter}, {97, KeyCode::Key_Right_Control}, {98, KeyCode::Key_Num_Divide},
        {99, KeyCode::Key_Print_Screen}, {100, KeyCode::Key_Right_Alt},
        {102, KeyCode::Key_Home}, {103, KeyCode::Key_Up}, {104, KeyCode::Key_Page_Up},
        {105, KeyCode::Key_Left}, {106, KeyCode::Key_Right}, {107, KeyCode::Key_End},
        {108, KeyCode::Key_Down}, {109, KeyCode::Key_Page_Down},
        {110, KeyCode::Key_Insert}, {111, KeyCode::Key_Delete},
        {119, KeyCode::Key_Pause},
        {125, KeyCode::Key_Left_Super}, {126, KeyCode::Key_Right_Super}, {127, KeyCode::Key_Menu},
    };

    if (nativeScanCode < 8) return KeyCode::Key_Invalid;

    return evdev.value(nativeScanCode - 8, KeyCode::Key_Invalid);
#else
    Q_UNUSED(nativeScanCode)
    return KeyCode::Key_Invalid;
#endif
}

}
