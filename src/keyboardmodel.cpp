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

void KeyboardModel::append(const quint16& pid, const Keyboard& keyboard)
{
    beginInsertRows(QModelIndex(), m_keyboards.size(), m_keyboards.size());
    m_keyboardPIDs << pid;
    m_keyboards << keyboard;
    endInsertRows();
}

void KeyboardModel::remove(const quint16& pid)
{
    auto index = m_keyboardPIDs.indexOf(pid);
    beginRemoveRows(QModelIndex(), index, index);
    m_keyboardPIDs.removeAt(index);
    m_keyboards.removeAt(index);
    endRemoveRows();
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

const QList<quint16> &KeyboardModel::keyboardPIDs()
{
    return m_keyboardPIDs;
}

const QList<Keyboard> &KeyboardModel::keyboards()
{
    return m_keyboards;
}
