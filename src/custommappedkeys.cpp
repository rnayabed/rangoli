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

#include "custommappedkeys.h"

CustomMappedKeys::CustomMappedKeys(QObject *parent)
    : QAbstractListModel{parent}, m_selectedKeyIndex{-1}
{

}

int CustomMappedKeys::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    return m_keys.contains(m_selectedKeyIndex) ? m_keys[m_selectedKeyIndex].size() : 0;
}

QVariant CustomMappedKeys::data(const QModelIndex &index, int role) const
{
    if (m_selectedKeyIndex == -1) return QVariant();

    if (role <= Qt::UserRole ||
            index.row() >= m_keys[m_selectedKeyIndex].size())
        return QVariant();

    if (role == NameRole)
    {
        return KeyCode::Names[m_keys[m_selectedKeyIndex].at(index.row())];
    }

    return QVariant();
}

QHash<int, QByteArray> CustomMappedKeys::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    return roles;
}

void CustomMappedKeys::addKeys(const int &keyIndex, const QList<KeyCode::Code> &keys)
{
    bool resetModel = m_selectedKeyIndex == keyIndex;

    if (resetModel) beginResetModel();
    m_keys[keyIndex] = keys;
    m_keyIndexes << keyIndex;
    if (resetModel) endResetModel();
}

const QList<int> &CustomMappedKeys::keyIndexes()
{
    return m_keyIndexes;
}

const QHash<int, QList<KeyCode::Code> > &CustomMappedKeys::keys()
{
    return m_keys;
}

int CustomMappedKeys::keysCount()
{
    return m_keyIndexes.size();
}

void CustomMappedKeys::setSelectedKeyIndex(const int &keyIndex)
{
    if (keyIndex == m_selectedKeyIndex) return;

    beginResetModel();
    m_selectedKeyIndex = keyIndex;
    endResetModel();
}

void CustomMappedKeys::add(const KeyCode::Code &code)
{
    beginResetModel();
    if (m_keys.contains(m_selectedKeyIndex))
    {
        if (!m_keys[m_selectedKeyIndex].contains(code))
        {
            if (!KeyCode::isCombination(code))
            {
                m_keys[m_selectedKeyIndex].removeIf([](const KeyCode::Code& i){
                    return !KeyCode::isCombination(i);
                });
            }

            m_keys[m_selectedKeyIndex] << code;
        }
    }
    else
    {
        m_keys[m_selectedKeyIndex] = {code};
        m_keyIndexes << m_selectedKeyIndex;
    }

    endResetModel();
}

void CustomMappedKeys::remove(const int& index)
{
    beginResetModel();

    m_keys[m_selectedKeyIndex].removeAt(index);

    if (m_keys[m_selectedKeyIndex].size() == 0) {
        m_keys.remove(m_selectedKeyIndex);
        m_keyIndexes.removeOne(m_selectedKeyIndex);
    }

    endResetModel();
}

void CustomMappedKeys::clear()
{
    beginResetModel();
    m_keys.remove(m_selectedKeyIndex);
    m_keyIndexes.removeOne(m_selectedKeyIndex);
    endResetModel();
}

void CustomMappedKeys::clearAll()
{
    beginResetModel();
    m_keys.clear();
    m_keyIndexes.clear();
    endResetModel();
}

