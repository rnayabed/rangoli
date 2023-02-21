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

#include <QString>
#include "icons.h"

using namespace Qt::Literals::StringLiterals;

QString Icons::get(const int &icon)
{
    switch (icon)
    {
    case Icon::Add:
        return u"qrc:/icons/add.svg"_s;
    case Icon::ArrowBack:
        return u"qrc:/icons/arrow_back.svg"_s;
    case Icon::BugReport:
        return u"qrc:/icons/bug_report.svg"_s;
    case Icon::Delete:
        return u"qrc:/icons/delete.svg"_s;
    case Icon::Donate:
        return u"qrc:/icons/donate.svg"_s;
    case Icon::Done:
        return u"qrc:/icons/done.svg"_s;
    case Icon::Edit:
        return u"qrc:/icons/edit.svg"_s;
    case Icon::Git:
        return u"qrc:/icons/git.svg"_s;
    case Icon::Help:
        return u"qrc:/icons/help.svg"_s;
    case Icon::Home:
        return u"qrc:/icons/home.svg"_s;
    case Icon::Keyboard:
        return u"qrc:/icons/keyboard.svg"_s;
    case Icon::Light:
        return u"qrc:/icons/light.svg"_s;
    case Icon::Rangoli:
        return u"qrc:/icons/rangoli.svg"_s;
    case Icon::Refresh:
        return u"qrc:/icons/refresh.svg"_s;
    case Icon::Reset:
        return u"qrc:/icons/reset.svg"_s;
    case Icon::Save:
        return u"qrc:/icons/save.svg"_s;
    case Icon::Settings:
        return u"qrc:/icons/settings.svg"_s;
    case Icon::Website:
        return u"qrc:/icons/website.svg"_s;
    }

    return QString();
}
