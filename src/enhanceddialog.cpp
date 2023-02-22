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

#include "enhanceddialog.h"


EnhancedDialog::EnhancedDialog(const QString &title, const QString &text)
    : EnhancedDialog(title, text, ButtonsType::OK)
{

}

EnhancedDialog::EnhancedDialog(const QString &title, const QString &text, const ButtonsType &buttonsType)
    : m_title(title), m_text(text), m_buttonsType(buttonsType)
{

}

QString &EnhancedDialog::title()
{
    return m_title;
}

QString &EnhancedDialog::text()
{
    return m_text;
}

int &EnhancedDialog::buttonsType()
{
    return m_buttonsType;
}

void EnhancedDialog::setOnClosed(std::function<void ()> onClosed)
{
    m_onClosed = onClosed;
}

void EnhancedDialog::setOnAccepted(std::function<void ()> onAccepted)
{
    m_onAccepted = onAccepted;
}

void EnhancedDialog::setOnRejected(std::function<void ()> onRejected)
{
    m_onRejected = onRejected;
}

void EnhancedDialog::setButtonsType(const ButtonsType &buttonsType)
{
    m_buttonsType = buttonsType;
}

void EnhancedDialog::onClosed()
{
    if(m_onClosed)
        m_onClosed();
}

void EnhancedDialog::onAccepted()
{
    if(m_onAccepted)
        m_onAccepted();
}

void EnhancedDialog::onRejected()
{
    if(m_onRejected)
        m_onRejected();
}

