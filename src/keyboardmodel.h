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

#ifndef KEYBOARDMODEL_H
#define KEYBOARDMODEL_H

#include <QAbstractItemModel>

#include "keyboard.h"

class KeyboardModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum KeyboardRoles {
        NameRole = Qt::UserRole + 1,
        ImagePathRole
    };

    explicit KeyboardModel(QObject* parent = nullptr);

    void append(const quint16& pid, const Keyboard& keyboard);
    void remove(const quint16& pid);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QHash<int, QByteArray> roleNames() const override;

    const QList<quint16>& keyboardPIDs();
    const QList<Keyboard>& keyboards();

private:
    QList<quint16> m_keyboardPIDs;
    QList<Keyboard> m_keyboards;
};

#endif // KEYBOARDMODEL_H
