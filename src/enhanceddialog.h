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

#ifndef ENHANCEDDIALOG_H
#define ENHANCEDDIALOG_H

#include <QObject>
#include <QQmlEngine>
#include <QEventLoop>

class EnhancedDialog
{
    Q_GADGET
    Q_PROPERTY(QString title READ title CONSTANT)
    Q_PROPERTY(QString text READ text CONSTANT)
    Q_PROPERTY(int buttonsType READ buttonsType CONSTANT)

public:
    EnhancedDialog() = default;
    explicit EnhancedDialog(const QString& title, const QString& text);

    enum ButtonsType { // FIXME: Make non caps
        OK = 0,
        YES_NO = 1
    };

    explicit EnhancedDialog(const QString& title, const QString& text, const EnhancedDialog::ButtonsType& buttonsType);

    QString& title();
    QString& text();
    int& buttonsType();
    ~EnhancedDialog() = default;

    void setOnClosed(std::function<void()> onClosed);
    void setOnAccepted(std::function<void()> onAccepted);
    void setOnRejected(std::function<void()> onRejected);
    void setButtonsType(const EnhancedDialog::ButtonsType& buttonsType);

public slots:
    void onClosed();
    void onAccepted();
    void onRejected();

private:
    QString m_title;
    QString m_text;
    int m_buttonsType;
    std::function<void()> m_onClosed;
    std::function<void()> m_onAccepted;
    std::function<void()> m_onRejected;
};

#endif // ENHANCEDDIALOG_H
