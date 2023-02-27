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

#include "keycodemodelproxy.h"

#include "keycodemodel.h"

KeyCodeModelProxy::KeyCodeModelProxy(QObject *parent)
    : QSortFilterProxyModel{parent}, m_currentFilter{Filter::All}
{
    setFilterCaseSensitivity(Qt::CaseInsensitive);
    setFilterKeyColumn(0);
    setFilterRole(KeyCodeModel::KeyRoles::CodeRole);
}

bool KeyCodeModelProxy::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    int keyCode = sourceModel()->data(sourceModel()->index(source_row, 0, source_parent), KeyCodeModel::CodeRole).toInt();

    if (KeyCode::isUnknownKey(keyCode))
    {
        return false;
    }

#if defined(Q_OS_LINUX)
    if (keyCode == KeyCode::Code::Shortcut_Explorer)
    {
        return false;
    }
#elif defined(Q_OS_MACOS)
    if (KeyCode::isShortcutKey(keyCode))
    {
        return false;
    }
#endif

    const QString searchStr = sourceModel()
            ->data(sourceModel()->index(source_row, 0, source_parent), KeyCodeModel::NameRole).toString();

    if (!searchStr.contains(filterRegularExpression()))
    {
        return false;
    }

    if (m_currentFilter == Filter::All)
    {
        return true;
    }
    if(m_currentFilter == Filter::Keyboard_Keys)
    {
        return !KeyCode::isMultimediaKey(keyCode)
                && !KeyCode::isShortcutKey(keyCode);
    }
    else if(m_currentFilter == Filter::Combination_Keys)
    {
        return KeyCode::isCombination(keyCode);
    }
    else if(m_currentFilter == Filter::Multimedia)
    {
        return KeyCode::isMultimediaKey(keyCode);
    }
    else if(m_currentFilter == Filter::Shortcut)
    {
        return KeyCode::isShortcutKey(keyCode);
    }

    return false;
}

void KeyCodeModelProxy::setCurrentFilter(const KeyCodeModelProxy::Filter &filter)
{
    if (m_currentFilter == filter) return;

    beginResetModel();
    m_currentFilter = filter;
    endResetModel();
}
