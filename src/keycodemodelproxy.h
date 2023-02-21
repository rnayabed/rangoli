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

#ifndef KEYCODEMODELPROXY_H
#define KEYCODEMODELPROXY_H

#include <QSortFilterProxyModel>

class KeyCodeModelProxy : public QSortFilterProxyModel
{
    Q_OBJECT

    enum Filter {
        All,
        Keyboard_Keys,
        Combination_Keys,
        Multimedia,
        Shortcut
    };

public:
    explicit KeyCodeModelProxy(QObject *parent = nullptr);

    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

public slots:
    void setCurrentFilter(const KeyCodeModelProxy::Filter& filter);

private:
    Filter m_currentFilter;

};

#endif // KEYCODEMODELPROXY_H
