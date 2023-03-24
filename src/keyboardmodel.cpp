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

#include "keyboardmodel.h"


KeyboardModel::KeyboardModel(QObject *parent)
    : QAbstractListModel(parent)
{}

void KeyboardModel::append(const Keyboard& keyboard)
{
    qInfo() << "Add keyboard" << keyboard.id;

    beginInsertRows(QModelIndex(), m_keyboards.size(), m_keyboards.size());
    m_keyboardUSBIDs << keyboard.id;
    m_keyboards << keyboard;
    endInsertRows();
}

void KeyboardModel::remove(const KeyboardUSBID& id)
{
    qInfo() << "Remove keyboard" << id;

    for (int i = 0; i < m_keyboardUSBIDs.size(); i++)
    {
        if (m_keyboardUSBIDs.at(i) == id)
        {
            beginRemoveRows(QModelIndex(), i, i);
            m_keyboardUSBIDs.removeAt(i);
            m_keyboards.removeAt(i);
            endRemoveRows();
            return;
        }
    }

    qCritical() << "Keyboard not found";
}

int KeyboardModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    return m_keyboards.size();
}

QVariant KeyboardModel::data(const QModelIndex &index, int role) const
{
    if (role <= Qt::UserRole || index.row() >= m_keyboards.size())
    {
        return QVariant();
    }

    if (role == NameRole)
    {
        return m_keyboards.at(index.row()).name;
    }
    else if (role == ImagePathRole)
    {
        return m_keyboards.at(index.row()).imagePath;
    }

    return QVariant();
}

QHash<int, QByteArray> KeyboardModel::roleNames() const
{
    QHash <int, QByteArray> roles;
    roles[NameRole] = "name";
    roles[ImagePathRole] = "imgPath";
    return roles;
}

const QList<KeyboardUSBID> &KeyboardModel::keyboardUSBIDs()
{
    return m_keyboardUSBIDs;
}

const QList<Keyboard> &KeyboardModel::keyboards()
{
    return m_keyboards;
}
