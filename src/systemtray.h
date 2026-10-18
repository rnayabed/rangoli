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

#ifndef SYSTEMTRAY_H
#define SYSTEMTRAY_H

#include <QObject>
#include <QPointer>
#include <QSystemTrayIcon>
#include <QMenu>

class MainWindowController;
class KeyboardConfiguratorController;

// System tray icon with a widgets menu. Qt.labs.platform on Qt 6.4 doesn't
// show menu items that are added or hidden after the menu is created.
class SystemTray : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool visible READ visible WRITE setVisible NOTIFY visibleChanged)

public:
    SystemTray(MainWindowController* mainWindowController,
               KeyboardConfiguratorController* keyboardConfiguratorController,
               QObject* parent = nullptr);

    bool visible() const;

signals:
    void visibleChanged();

    // Left click or "Open Rangoli"
    void showWindowRequested();

public slots:
    void setVisible(const bool& visible);

private:
    QPointer<MainWindowController> m_mainWindowController;
    QPointer<KeyboardConfiguratorController> m_keyboardConfiguratorController;

    QSystemTrayIcon m_icon;
    QMenu m_menu;

    // Built every time the menu opens, so it always shows the current state
    void rebuildMenu();
};

#endif // SYSTEMTRAY_H
