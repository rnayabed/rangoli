/*
 * Copyright (C) 2026 Rangoli contributors
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

#ifndef CUSTOMEFFECTMODEL_H
#define CUSTOMEFFECTMODEL_H

#include <QAbstractListModel>
#include <QColor>
#include <QHash>

#include "lightanimation.h"

struct CustomEffect
{
    QString id;
    QString name;

    // LED buffer index -> colour
    QHash<int, QColor> keys;

    // LED buffer indexes in the order the user picked them
    QList<int> order;

    // Sequence pattern steps, as LED buffer indexes
    QList<QList<int>> sequence;

    AnimationSettings animation;
};

// User created custom light mode effects, saved per keyboard model
class CustomEffectModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum CustomEffectModelRoles {
        IDRole = Qt::UserRole + 1,
        NameRole,
        AnimatedRole
    };

    explicit CustomEffectModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QHash<int, QByteArray> roleNames() const override;

    void load(const QString& keyboardID);

    // effect's id is ignored, a new one is made
    int createEffect(const CustomEffect& effect);
    void setEffectName(const int& row, const QString& name);
    // Replaces everything except the id and name
    void setEffectContent(const int& row, const CustomEffect& content);
    void deleteEffect(const int& row);

    const CustomEffect& effect(const int& row) const;
    int indexOf(const QString& id) const;

    Q_INVOKABLE QString effectName(const int& row) const;

private:
    QList<CustomEffect> m_effects;
    QString m_keyboardID;

    void writeEffect(const CustomEffect& effect);
    void writeOrder();
};

#endif // CUSTOMEFFECTMODEL_H
