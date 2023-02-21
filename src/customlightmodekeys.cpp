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

#include "customlightmodekeys.h"
#include <QDebug>

CustomLightModeKeys::CustomLightModeKeys(QObject *parent)
    : QAbstractListModel{parent}
{

}

CustomLightModeKeys::~CustomLightModeKeys()
{

}

int CustomLightModeKeys::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return m_keys.size();
}

QVariant CustomLightModeKeys::data(const QModelIndex &index, int role) const
{
    if (role <= Qt::UserRole || index.row() >= m_keys.size())
    {
        return QVariant();
    }

    if (role == ColourRole)
    {
        return m_keys[m_keyIndexes.at(index.row())];
    }

    return QVariant();
}

QHash<int, QByteArray> CustomLightModeKeys::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[ColourRole] = "colour";
    return roles;
}

void CustomLightModeKeys::add(const int &keyIndex, const QColor &colour)
{
    beginResetModel();
    m_keys[keyIndex] = colour;
    m_keyIndexes << keyIndex;
}

void CustomLightModeKeys::addIfNotExists(const int &keyIndex, const QColor &colour)
{
    if (!m_keys.contains(keyIndex))
    {
        add(keyIndex, colour);
    }
}

void CustomLightModeKeys::toggle(const int& keyIndex, const QColor &colour)
{
    if (m_keys.contains(keyIndex))
    {
        beginResetModel();
        m_keys.remove(keyIndex);
        m_keyIndexes.removeOne(keyIndex);
        endResetModel();
    }
    else
    {
        add(keyIndex, colour);
    }
}

void CustomLightModeKeys::toggle(const int& keyIndex)
{
    toggle(keyIndex, QColor(Qt::white));
}

const QList<int> &CustomLightModeKeys::keyIndexes()
{
    return m_keyIndexes;
}

const QHash<int, QColor> &CustomLightModeKeys::keys()
{
    return m_keys;
}

void CustomLightModeKeys::clear()
{
    beginResetModel();
    m_keys.clear();
    m_keyIndexes.clear();
    endResetModel();
}

QColor CustomLightModeKeys::getColour(const int &index)
{
    return data(this->index(index),
                CustomLightModeKeys::ColourRole).value<QColor>();
}
