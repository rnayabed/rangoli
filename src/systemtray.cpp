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

#include "systemtray.h"

#include <QActionGroup>
#include <QApplication>

#include "mainwindowcontroller.h"
#include "keyboardconfiguratorcontroller.h"

SystemTray::SystemTray(MainWindowController *mainWindowController,
                       KeyboardConfiguratorController *keyboardConfiguratorController,
                       QObject *parent)
    : QObject{parent},
      m_mainWindowController{mainWindowController},
      m_keyboardConfiguratorController{keyboardConfiguratorController}
{
    m_icon.setIcon(QApplication::windowIcon());
    m_icon.setToolTip(tr("Rangoli"));
    m_icon.setContextMenu(&m_menu);

    connect(&m_menu, &QMenu::aboutToShow, this, &SystemTray::rebuildMenu);

    connect(&m_icon, &QSystemTrayIcon::activated, this, [this](QSystemTrayIcon::ActivationReason reason){
        if (reason == QSystemTrayIcon::Trigger)
        {
            emit showWindowRequested();
        }
    });

    // Some desktops only show a context menu that has items in it
    rebuildMenu();
}

bool SystemTray::visible() const
{
    return m_icon.isVisible();
}

void SystemTray::setVisible(const bool &visible)
{
    if (m_icon.isVisible() == visible) return;

    m_icon.setVisible(visible);
    emit visibleChanged();
}

void SystemTray::rebuildMenu()
{
    m_menu.clear();

    // Profiles apply to the first keyboard, like opening one from the home screen
    const bool hasKeyboard = m_mainWindowController->connectedKeyboards() &&
            m_mainWindowController->connectedKeyboards()->rowCount() > 0;

    ProfileModel* profiles = m_keyboardConfiguratorController->profiles();

    if (profiles)
    {
        auto group = new QActionGroup(&m_menu);

        for (int i = 0; i < profiles->rowCount(); i++)
        {
            QAction* action = m_menu.addAction(profiles->profileName(i));
            action->setCheckable(true);
            action->setChecked(i == m_keyboardConfiguratorController->selectedProfileIndex());
            action->setEnabled(hasKeyboard);
            action->setActionGroup(group);

            connect(action, &QAction::triggered, this, [this, i](){
                m_mainWindowController->loadKeyboard(0);
                m_keyboardConfiguratorController->setSelectedProfileIndex(i);
            });
        }

        m_menu.addSeparator();
    }

    if (m_keyboardConfiguratorController->playing())
    {
        connect(m_menu.addAction(tr("Stop animation")), &QAction::triggered,
                m_keyboardConfiguratorController, qOverload<>(&KeyboardConfiguratorController::stopPlayback));
        m_menu.addSeparator();
    }
    else if (m_keyboardConfiguratorController->canResumePlayback())
    {
        connect(m_menu.addAction(tr("Play animation")), &QAction::triggered,
                m_keyboardConfiguratorController, &KeyboardConfiguratorController::resumePlayback);
        m_menu.addSeparator();
    }

    connect(m_menu.addAction(tr("Open Rangoli")), &QAction::triggered,
            this, &SystemTray::showWindowRequested);

    connect(m_menu.addAction(tr("Quit")), &QAction::triggered, this, [this](){
        m_mainWindowController->setCloseToSystemTrayIcon(false);
        m_mainWindowController->setInterruptClose(false);
        m_mainWindowController->quit();
    });
}
