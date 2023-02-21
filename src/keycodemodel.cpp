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

#include "keycodemodel.h"

using namespace Qt::Literals::StringLiterals;

KeyCodeModel::KeyCodeModel(QObject *parent)
    : QAbstractListModel{parent},
      m_codes{KeyCode::Names.keys()}
{}

int KeyCodeModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    return m_codes.size();
}

QVariant KeyCodeModel::data(const QModelIndex &index, int role) const
{
    if (role <= Qt::UserRole || index.row() >= m_codes.size())
    {
        return QVariant();
    }

    if (role == NameRole)
    {
        return KeyCode::Names[m_codes.at(index.row())];
    }
    else if (role == CodeRole)
    {
        return m_codes.at(index.row());
    }

    return QVariant();
}

QHash<int, QByteArray> KeyCodeModel::roleNames() const
{
    QHash<int, QByteArray> roles;

    roles[NameRole] = "name";
    roles[CodeRole] = "code";

    return roles;
}
