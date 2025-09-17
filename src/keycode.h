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

#ifndef KEYCODE_H
#define KEYCODE_H

#include <QHash>
#include <QString>

using namespace Qt::Literals::StringLiterals;

class KeyCode {

public:
    enum Code {
        Key_1                       = 0x1e00,
        Key_2                       = 0x1f00,
        Key_3                       = 0x2000,
        Key_4                       = 0x2100,
        Key_5                       = 0x2200,
        Key_6                       = 0x2300,
        Key_7                       = 0x2400,
        Key_8                       = 0x2500,
        Key_9                       = 0x2600,
        Key_0                       = 0x2700,

        Key_A                       = 0x0400,
        Key_B                       = 0x0500,
        Key_C                       = 0x0600,
        Key_D                       = 0x0700,
        Key_E                       = 0x0800,
        Key_F                       = 0x0900,
        Key_G                       = 0x0a00,
        Key_H                       = 0x0b00,
        Key_I                       = 0x0c00,
        Key_J                       = 0x0d00,
        Key_K                       = 0x0e00,
        Key_L                       = 0x0f00,
        Key_M                       = 0x1000,
        Key_N                       = 0x1100,
        Key_O                       = 0x1200,
        Key_P                       = 0x1300,
        Key_Q                       = 0x1400,
        Key_R                       = 0x1500,
        Key_S                       = 0x1600,
        Key_T                       = 0x1700,
        Key_U                       = 0x1800,
        Key_V                       = 0x1900,
        Key_W                       = 0x1a00,
        Key_X                       = 0x1b00,
        Key_Y                       = 0x1c00,
        Key_Z                       = 0x1d00,

        Key_Hyphen                  = 0x2d00,
        Key_Equals                  = 0x2e00,
        Key_Left_Bracket            = 0x2f00,
        Key_Right_Bracket           = 0x3000,
        Key_Back_Slash              = 0x3100,
        Key_Semi_Colon              = 0x3300,
        Key_Quote                   = 0x3400,
        Key_Back_Quote              = 0x3500,
        Key_Comma                   = 0x3600,
        Key_Dot                     = 0x3700,
        Key_Slash                   = 0x3800,

        Key_Left_Control            = 0x010000,
        Key_Left_Shift              = 0x020000,
        Key_Left_Alt                = 0x040000,

        Key_Right_Control           = 0x100000,
        Key_Right_Shift             = 0x200000,
        Key_Right_Alt               = 0x400000,

        Key_Left_Super              = 0x080000,
        Key_Right_Super             = 0x800000,

        Key_Right                   = 0x4f00,
        Key_Left                    = 0x5000,
        Key_Down                    = 0x5100,
        Key_Up                      = 0x5200,

        Key_F1                      = 0x3a00,
        Key_F2                      = 0x3b00,
        Key_F3                      = 0x3c00,
        Key_F4                      = 0x3d00,
        Key_F5                      = 0x3e00,
        Key_F6                      = 0x3f00,
        Key_F7                      = 0x4000,
        Key_F8                      = 0x4100,
        Key_F9                      = 0x4200,
        Key_F10                     = 0x4300,
        Key_F11                     = 0x4400,
        Key_F12                     = 0x4500,

        Key_Escape                  = 0x2900,
        Key_Tab                     = 0x2b00,
        Key_Enter                   = 0x2800,
        Key_Menu                    = 0x6500,
        Key_Insert                  = 0x4900,
        Key_Pause                   = 0x4800,
        Key_Space                   = 0x2c00,
        Key_Home                    = 0x4a00,
        Key_End                     = 0x4d00,
        Key_Delete                  = 0x4c00,
        Key_Page_Down               = 0x4e00,
        Key_Page_Up                 = 0x4b00,
        Key_Caps_Lock               = 0x3900,
        Key_Backspace               = 0x2a00,
        Key_Print_Screen            = 0x4600,

        Key_Num_1                   = 0x5900,
        Key_Num_2                   = 0x5a00,
        Key_Num_3                   = 0x5b00,
        Key_Num_4                   = 0x5c00,
        Key_Num_5                   = 0x5d00,
        Key_Num_6                   = 0x5e00,
        Key_Num_7                   = 0x5f00,
        Key_Num_8                   = 0x6000,
        Key_Num_9                   = 0x6100,
        Key_Num_0                   = 0x6200,
        Key_Num_Add                 = 0x5700,
        Key_Num_Substract           = 0x5600,
        Key_Num_Multiply            = 0x5500,
        Key_Num_Divide              = 0x5400,
        Key_Num_Decimal_Point       = 0x6300,
        Key_Num_Lock                = 0x5300,
        Key_Num_Enter               = 0x5800,
        Key_102ND                   = 0x6400,

        Key_Fn                      = 0xb000,

        Shortcut_Explorer           = 0x01000194,
        Shortcut_Cut                = 0x011b00,
        Shortcut_Copy               = 0x010600,
        Shortcut_Paste              = 0x011900,
        Shortcut_Save               = 0x011600,
        Shortcut_Show_Desktop       = 0x080700,
        Shortcut_Lock               = 0x080f00,
        Shortcut_Switch_Window      = 0x042b00,
        Shortcut_Close_Window       = 0x043d00,

        Multimedia_Play_Pause       = 0x010000cd,
        Multimedia_Stop             = 0x010000b7,
        Multimedia_Previous         = 0x010000b6,
        Multimedia_Next             = 0x010000b5,
        Multimedia_Volume_Up        = 0x010000e9,
        Multimedia_Volume_Down      = 0x010000ea,
        Multimedia_Mute             = 0x010000e2,

        Key_Invalid                 = 0,
        Key_Scroll_Lock             = -1,
        Key_Left_Shift_Split        = -2,
        Key_Calculator              = -3
    };

    static const inline QHash<KeyCode::Code, QString> Names{
        {Key_1, u"1"_s},
        {Key_2, u"2"_s},
        {Key_3, u"3"_s},
        {Key_4, u"4"_s},
        {Key_5, u"5"_s},
        {Key_6, u"6"_s},
        {Key_7, u"7"_s},
        {Key_8, u"8"_s},
        {Key_9, u"9"_s},
        {Key_0, u"0"_s},
        {Key_A, u"A"_s},
        {Key_B, u"B"_s},
        {Key_C, u"C"_s},
        {Key_D, u"D"_s},
        {Key_E, u"E"_s},
        {Key_F, u"F"_s},
        {Key_G, u"G"_s},
        {Key_H, u"H"_s},
        {Key_I, u"I"_s},
        {Key_J, u"J"_s},
        {Key_K, u"K"_s},
        {Key_L, u"L"_s},
        {Key_M, u"M"_s},
        {Key_N, u"N"_s},
        {Key_O, u"O"_s},
        {Key_P, u"P"_s},
        {Key_Q, u"Q"_s},
        {Key_R, u"R"_s},
        {Key_S, u"S"_s},
        {Key_T, u"T"_s},
        {Key_U, u"U"_s},
        {Key_V, u"V"_s},
        {Key_W, u"W"_s},
        {Key_X, u"X"_s},
        {Key_Y, u"Y"_s},
        {Key_Z, u"Z"_s},
        {Key_Hyphen, u"-"_s},
        {Key_Equals, u"="_s},
        {Key_Left_Bracket, u"["_s},
        {Key_Right_Bracket, u"]"_s},
        {Key_Back_Slash, u"\\"_s},
        {Key_Semi_Colon, u";"_s},
        {Key_Quote, u"'"_s},
        {Key_Back_Quote, u"`"_s},
        {Key_Comma, u","_s},
        {Key_Dot, u"."_s},
        {Key_Slash, u"/"_s},
        {Key_Left_Control, u"Left Control"_s},
        {Key_Left_Shift, u"Left Shift"_s},
        {Key_Left_Alt, u"Left Alt"_s},
        {Key_Right_Control, u"Right Control"_s},
        {Key_Right_Shift, u"Right Shift"_s},
        {Key_Right_Alt, u"Right Alt"_s},
#ifdef Q_OS_LINUX
        {Key_Left_Super, u"Left Super"_s},
        {Key_Right_Super, u"Right Super"_s},
#else
        {Key_Left_Super, u"Left Win"_s},
        {Key_Right_Super, u"Right Win"_s},
#endif
        {Key_Right, u"Right"_s},
        {Key_Left, u"Left"_s},
        {Key_Down, u"Down"_s},
        {Key_Up, u"Up"_s},
        {Key_F1, u"F1"_s},
        {Key_F2, u"F2"_s},
        {Key_F3, u"F3"_s},
        {Key_F4, u"F4"_s},
        {Key_F5, u"F5"_s},
        {Key_F6, u"F6"_s},
        {Key_F7, u"F7"_s},
        {Key_F8, u"F8"_s},
        {Key_F9, u"F9"_s},
        {Key_F10, u"F10"_s},
        {Key_F11, u"F11"_s},
        {Key_F12, u"F12"_s},
        {Key_Escape, u"Esc"_s},
        {Key_Tab, u"Tab"_s},
        {Key_Enter, u"Enter"_s},
        {Key_Menu, u"Menu"_s},
        {Key_Insert, u"Insert"_s},
        {Key_Pause, u"Pause"_s},
        {Key_Space, u"Space"_s},
        {Key_Home, u"Home"_s},
        {Key_End, u"End"_s},
        {Key_Delete, u"Delete"_s},
        {Key_Page_Down, u"Page Down"_s},
        {Key_Page_Up, u"Page Up"_s},
        {Key_Caps_Lock, u"Caps Lock"_s},
        {Key_Backspace, u"Backspace"_s},
        {Key_Print_Screen, u"Print Screen"_s},
        {Key_Num_1, u"Numpad 1"_s},
        {Key_Num_2, u"Numpad 2"_s},
        {Key_Num_3, u"Numpad 3"_s},
        {Key_Num_4, u"Numpad 4"_s},
        {Key_Num_5, u"Numpad 5"_s},
        {Key_Num_6, u"Numpad 6"_s},
        {Key_Num_7, u"Numpad 7"_s},
        {Key_Num_8, u"Numpad 8"_s},
        {Key_Num_9, u"Numpad 9"_s},
        {Key_Num_0, u"Numpad 0"_s},
        {Key_Num_Add, u"Numpad +"_s},
        {Key_Num_Substract, u"Numpad -"_s},
        {Key_Num_Multiply, u"Numpad *"_s},
        {Key_Num_Divide, u"Numpad /"_s},
        {Key_Num_Decimal_Point, u"Numpad ."_s},
        {Key_Num_Lock, u"Num Lock"_s},
        {Key_Num_Enter, u"Numpad Enter"_s},
        {Key_Fn, u"Fn"_s},
        {Shortcut_Explorer, u"Open Explorer"_s},
        {Shortcut_Cut, u"Cut"_s},
        {Shortcut_Copy, u"Copy"_s},
        {Shortcut_Paste, u"Paste"_s},
        {Shortcut_Save, u"Save"_s},
        {Shortcut_Show_Desktop, u"Show Desktop"_s},
        {Shortcut_Lock, u"Lock System"_s},
        {Shortcut_Switch_Window, u"Switch Window"_s},
        {Shortcut_Close_Window, u"Close Window"_s},
        {Multimedia_Play_Pause, u"Play / Pause"_s},
        {Multimedia_Stop, u"Stop"_s},
        {Multimedia_Previous, u"Previous"_s},
        {Multimedia_Next, u"Next"_s},
        {Multimedia_Volume_Up, u"Volume Up"_s},
        {Multimedia_Volume_Down, u"Volume Down"_s},
        {Multimedia_Mute, u"Multimedia Mute"_s},
        {Key_Scroll_Lock, u"Scroll Lock"_s},
        {Key_Left_Shift_Split, u">< (Left Shift Split)"_s},
        {Key_Calculator, u"Calculator"_s}
    };

    static const inline QHash<QString, KeyCode::Code> CodeStrings {
        {u"Key_1"_s, Key_1},
        {u"Key_2"_s, Key_2},
        {u"Key_3"_s, Key_3},
        {u"Key_4"_s, Key_4},
        {u"Key_5"_s, Key_5},
        {u"Key_6"_s, Key_6},
        {u"Key_7"_s, Key_7},
        {u"Key_8"_s, Key_8},
        {u"Key_9"_s, Key_9},
        {u"Key_0"_s, Key_0},
        {u"Key_A"_s, Key_A},
        {u"Key_B"_s, Key_B},
        {u"Key_C"_s, Key_C},
        {u"Key_D"_s, Key_D},
        {u"Key_E"_s, Key_E},
        {u"Key_F"_s, Key_F},
        {u"Key_G"_s, Key_G},
        {u"Key_H"_s, Key_H},
        {u"Key_I"_s, Key_I},
        {u"Key_J"_s, Key_J},
        {u"Key_K"_s, Key_K},
        {u"Key_L"_s, Key_L},
        {u"Key_M"_s, Key_M},
        {u"Key_N"_s, Key_N},
        {u"Key_O"_s, Key_O},
        {u"Key_P"_s, Key_P},
        {u"Key_Q"_s, Key_Q},
        {u"Key_R"_s, Key_R},
        {u"Key_S"_s, Key_S},
        {u"Key_T"_s, Key_T},
        {u"Key_U"_s, Key_U},
        {u"Key_V"_s, Key_V},
        {u"Key_W"_s, Key_W},
        {u"Key_X"_s, Key_X},
        {u"Key_Y"_s, Key_Y},
        {u"Key_Z"_s, Key_Z},
        {u"Key_Hyphen"_s, Key_Hyphen},
        {u"Key_Equals"_s, Key_Equals},
        {u"Key_Left_Bracket"_s, Key_Left_Bracket},
        {u"Key_Right_Bracket"_s, Key_Right_Bracket},
        {u"Key_Back_Slash"_s, Key_Back_Slash},
        {u"Key_Semi_Colon"_s, Key_Semi_Colon},
        {u"Key_Quote"_s, Key_Quote},
        {u"Key_Back_Quote"_s, Key_Back_Quote},
        {u"Key_Comma"_s, Key_Comma},
        {u"Key_Dot"_s, Key_Dot},
        {u"Key_Slash"_s, Key_Slash},
        {u"Key_Left_Control"_s, Key_Left_Control},
        {u"Key_Left_Shift"_s, Key_Left_Shift},
        {u"Key_Left_Alt"_s, Key_Left_Alt},
        {u"Key_Right_Control"_s, Key_Right_Control},
        {u"Key_Right_Shift"_s, Key_Right_Shift},
        {u"Key_Right_Alt"_s, Key_Right_Alt},
        {u"Key_Left_Super"_s, Key_Left_Super},
        {u"Key_Right_Super"_s, Key_Right_Super},
        {u"Key_Right"_s, Key_Right},
        {u"Key_Left"_s, Key_Left},
        {u"Key_Down"_s, Key_Down},
        {u"Key_Up"_s, Key_Up},
        {u"Key_F1"_s, Key_F1},
        {u"Key_F2"_s, Key_F2},
        {u"Key_F3"_s, Key_F3},
        {u"Key_F4"_s, Key_F4},
        {u"Key_F5"_s, Key_F5},
        {u"Key_F6"_s, Key_F6},
        {u"Key_F7"_s, Key_F7},
        {u"Key_F8"_s, Key_F8},
        {u"Key_F9"_s, Key_F9},
        {u"Key_F10"_s, Key_F10},
        {u"Key_F11"_s, Key_F11},
        {u"Key_F12"_s, Key_F12},
        {u"Key_Escape"_s, Key_Escape},
        {u"Key_Tab"_s, Key_Tab},
        {u"Key_Enter"_s, Key_Enter},
        {u"Key_Menu"_s, Key_Menu},
        {u"Key_Insert"_s, Key_Insert},
        {u"Key_Pause"_s, Key_Pause},
        {u"Key_Space"_s, Key_Space},
        {u"Key_Home"_s, Key_Home},
        {u"Key_End"_s, Key_End},
        {u"Key_Delete"_s, Key_Delete},
        {u"Key_Page_Down"_s, Key_Page_Down},
        {u"Key_Page_Up"_s, Key_Page_Up},
        {u"Key_Caps_Lock"_s, Key_Caps_Lock},
        {u"Key_Backspace"_s, Key_Backspace},
        {u"Key_Print_Screen"_s, Key_Print_Screen},
        {u"Key_Num_1"_s, Key_Num_1},
        {u"Key_Num_2"_s, Key_Num_2},
        {u"Key_Num_3"_s, Key_Num_3},
        {u"Key_Num_4"_s, Key_Num_4},
        {u"Key_Num_5"_s, Key_Num_5},
        {u"Key_Num_6"_s, Key_Num_6},
        {u"Key_Num_7"_s, Key_Num_7},
        {u"Key_Num_8"_s, Key_Num_8},
        {u"Key_Num_9"_s, Key_Num_9},
        {u"Key_Num_0"_s, Key_Num_0},
        {u"Key_Num_Add"_s, Key_Num_Add},
        {u"Key_Num_Substract"_s, Key_Num_Substract},
        {u"Key_Num_Multiply"_s, Key_Num_Multiply},
        {u"Key_Num_Divide"_s, Key_Num_Divide},
        {u"Key_Num_Decimal_Point"_s, Key_Num_Decimal_Point},
        {u"Key_Num_Lock"_s, Key_Num_Lock},
        {u"Key_Num_Enter"_s, Key_Num_Enter},
        {u"Key_Fn"_s, Key_Fn},
        {u"Shortcut_Explorer"_s, Shortcut_Explorer},
        {u"Shortcut_Cut"_s, Shortcut_Cut},
        {u"Shortcut_Copy"_s, Shortcut_Copy},
        {u"Shortcut_Paste"_s, Shortcut_Paste},
        {u"Shortcut_Save"_s, Shortcut_Save},
        {u"Shortcut_Show_Desktop"_s, Shortcut_Show_Desktop},
        {u"Shortcut_Lock"_s, Shortcut_Lock},
        {u"Shortcut_Switch_Window"_s, Shortcut_Switch_Window},
        {u"Shortcut_Close_Window"_s, Shortcut_Close_Window},
        {u"Multimedia_Play_Pause"_s, Multimedia_Play_Pause},
        {u"Multimedia_Stop"_s, Multimedia_Stop},
        {u"Multimedia_Previous"_s, Multimedia_Previous},
        {u"Multimedia_Next"_s, Multimedia_Next},
        {u"Multimedia_Volume_Up"_s, Multimedia_Volume_Up},
        {u"Multimedia_Volume_Down"_s, Multimedia_Volume_Down},
        {u"Multimedia_Mute"_s, Multimedia_Mute},
        {u"Key_Scroll_Lock"_s, Key_Scroll_Lock},
        {u"Key_Left_Shift_Split"_s, Key_Left_Shift_Split},
        {u"Key_Calculator"_s, Key_Calculator}
    };

    static bool isCombination(const int& keyCode)
    {
        return keyCode == Key_Left_Control ||
                keyCode == Key_Left_Shift ||
                keyCode == Key_Left_Alt ||
                keyCode == Key_Left_Super;
    }

    static bool isMultimediaKey(const int& keyCode)
    {
        return keyCode == Multimedia_Play_Pause ||
                keyCode == Multimedia_Stop ||
                keyCode == Multimedia_Previous ||
                keyCode == Multimedia_Next ||
                keyCode == Multimedia_Volume_Up ||
                keyCode == Multimedia_Volume_Down ||
                keyCode == Multimedia_Mute;
    }

    static bool isUnknownKey(const int& keyCode)
    {
        return keyCode == Key_Scroll_Lock ||
                keyCode == Key_Left_Shift_Split ||
                keyCode == Key_Calculator;
    }

    static bool isShortcutKey(const int& keyCode)
    {
        return keyCode == Shortcut_Explorer ||
                keyCode == Shortcut_Cut ||
                keyCode == Shortcut_Copy ||
                keyCode == Shortcut_Paste ||
                keyCode == Shortcut_Save ||
                keyCode == Shortcut_Show_Desktop ||
                keyCode == Shortcut_Lock ||
                keyCode == Shortcut_Switch_Window ||
                keyCode == Shortcut_Close_Window;
    }

};

#endif // KEYCODE_H
