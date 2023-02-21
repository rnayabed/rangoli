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

#ifndef KEYCODEMODEL_H
#define KEYCODEMODEL_H

#include <QAbstractListModel>
#include <QSet>

#include "keycode.h"

class KeyCodeModel : public QAbstractListModel
{
    Q_OBJECT

public:
    //FIXME: refactor

    enum KeyRoles {
        NameRole = Qt::UserRole + 1,
        CodeRole
    };

    explicit KeyCodeModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QHash<int, QByteArray> roleNames() const override;

private:
    // FIXME: refactor?

    QList<KeyCode::Code> m_codes;

};

#endif // KEYCODEMODEL_H
