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

#ifndef CUSTOMMAPPEDKEYS_H
#define CUSTOMMAPPEDKEYS_H

#include <QAbstractListModel>
#include <QPointer>
#include <QSet>

#include "keycode.h"
#include "macromodel.h"

class CustomMappedKeys : public QAbstractListModel
{
    Q_OBJECT

public: 
    enum Roles {
        NameRole = Qt::UserRole + 1
    };

    explicit CustomMappedKeys(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QHash<int, QByteArray> roleNames() const override;

    void addKeys(const int& keyIndex, const QList<KeyCode::Code>& keys);

    const QList<int>& keyIndexes();
    const QHash<int, QList<KeyCode::Code>>& keys();

    Q_INVOKABLE int keysCount();

    // Used to name keys that play a macro
    void setMacros(MacroModel* macros);

    // The macro slot the selected key plays, or -1
    Q_INVOKABLE int selectedMacroSlot() const;

public slots:
    void setSelectedKeyIndex(const int& keyIndex);

    void add(const KeyCode::Code& code);

    // A key that plays a macro does nothing else
    void setMacro(const int& slot);
    void remove(const int& index);
    void clear();
    void clearAll();

private:
    QHash<int, QList<KeyCode::Code>> m_keys;
    QList<int> m_keyIndexes;
    int m_selectedKeyIndex;
    QPointer<MacroModel> m_macros;
};

#endif // CUSTOMMAPPEDKEYS_H
