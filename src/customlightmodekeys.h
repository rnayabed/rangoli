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

#ifndef CUSTOMLIGHTMODEKEYS_H
#define CUSTOMLIGHTMODEKEYS_H

#include <QObject>
#include <QtGlobal>
#include <QString>
#include <QColor>
#include <QAbstractListModel>

struct CustomLightModeKey
{
    int keyIndex;
    QColor colour;
};

class CustomLightModeKeys : public QAbstractListModel
{
    Q_OBJECT

public:
    CustomLightModeKeys(QObject* parent = nullptr);
    ~CustomLightModeKeys();

    enum Roles {
        ColourRole = Qt::UserRole + 1
    };

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QHash<int, QByteArray> roleNames() const override;

    void add(const int& keyIndex, const QColor &colour);
    void addIfNotExists(const int& keyIndex, const QColor &colour);
    void toggle(const int& keyIndex, const QColor &colour);
    void toggle(const int& keyIndex);

    const QList<int>& keyIndexes();
    const QHash<int, QColor>& keys();
    void clear();

    Q_INVOKABLE QColor getColour(const int& index);

private:
    QHash<int, QColor> m_keys;
    QList<int> m_keyIndexes;
};

#endif // CUSTOMLIGHTMODEKEYS_H

