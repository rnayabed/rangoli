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

#include "customcolours.h"

#include <QColor>

CustomColours::CustomColours(QObject *parent)
    : QAbstractListModel{parent}
{}

int CustomColours::defaultRowCount()
{
    return 8;
}

int CustomColours::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    return m_colours.size();
}

QVariant CustomColours::data(const QModelIndex &index, int role) const
{
    if (role <= Qt::UserRole || index.row() >= m_colours.size())
    {
        return QVariant();
    }

    if (role == ColourRole)
    {
        return m_colours.at(index.row());
    }

    return QVariant();
}

bool CustomColours::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!hasIndex(index.row(), index.column(), index.parent()))
        return false;

    if (role == ColourRole)
        m_colours.replace(index.row(), value.value<QColor>());
    else
        return false;

    emit dataChanged(index, index);

    return true;
}

QHash<int, QByteArray> CustomColours::roleNames() const
{
    QHash <int, QByteArray> roles;

    roles[ColourRole] = "colour";

    return roles;
}

void CustomColours::setColours(const QList<QColor> colours)
{
    beginResetModel();
    m_colours = colours;
    endResetModel();
}

const QList<QColor> &CustomColours::colours()
{
    return m_colours;
}
