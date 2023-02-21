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

#ifndef CUSTOMCOLOURS_H
#define CUSTOMCOLOURS_H

#include <QAbstractListModel>

class CustomColours : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit CustomColours(QObject *parent = nullptr);

    enum Roles {
        ColourRole = Qt::UserRole +1
    };

    int defaultRowCount();
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    QHash<int, QByteArray> roleNames() const override;

    void setColours(const QList<QColor> colours);

    const QList<QColor>& colours();

private:
    QList<QColor> m_colours;

};

#endif // CUSTOMCOLOURS_H
