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

#include "customeffectmodel.h"

#include <QSettings>
#include <QUuid>

using namespace Qt::Literals::StringLiterals;

CustomEffectModel::CustomEffectModel(QObject *parent)
    : QAbstractListModel{parent}
{}

int CustomEffectModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    return m_effects.size();
}

QVariant CustomEffectModel::data(const QModelIndex &index, int role) const
{
    if (role <= Qt::UserRole || index.row() >= m_effects.size())
    {
        return QVariant();
    }

    if (role == IDRole)
    {
        return m_effects.at(index.row()).id;
    }
    else if (role == NameRole)
    {
        return m_effects.at(index.row()).name;
    }
    else if (role == AnimatedRole)
    {
        return m_effects.at(index.row()).animation.enabled;
    }

    return QVariant();
}

QHash<int, QByteArray> CustomEffectModel::roleNames() const
{
    QHash <int, QByteArray> roles;

    roles[IDRole] = "id";
    roles[NameRole] = "name";
    roles[AnimatedRole] = "animated";

    return roles;
}

void CustomEffectModel::load(const QString &keyboardID)
{
    qInfo() << "Load custom effects for" << keyboardID;

    beginResetModel();

    m_keyboardID = keyboardID;
    m_effects.clear();

    QSettings settings;
    settings.beginGroup(QStringLiteral("custom_effects/%1").arg(m_keyboardID));

    const QStringList order = settings.value(u"order"_s).toStringList();

    for (auto&& id : order)
    {
        if (!settings.childGroups().contains(id)) continue;

        settings.beginGroup(id);

        CustomEffect effect{id, settings.value(u"name"_s).toString(), {}, {}, {}, {}};

        settings.beginGroup(u"keys"_s);
        for (auto&& bufferIndex : settings.childKeys())
        {
            effect.keys[bufferIndex.toInt()] = settings.value(bufferIndex).value<QColor>();
        }
        settings.endGroup();

        for (auto&& bufferIndex : settings.value(u"order"_s).toStringList())
        {
            effect.order << bufferIndex.toInt();
        }

        effect.sequence = LightAnimation::sequenceFromStrings(settings.value(u"sequence"_s).toStringList());
        effect.animation = AnimationSettings::read(settings);

        settings.endGroup();

        m_effects << effect;
    }

    settings.endGroup();

    endResetModel();
}

int CustomEffectModel::createEffect(const CustomEffect &newEffect)
{
    qInfo() << "Create custom effect" << newEffect.name;

    CustomEffect effect = newEffect;
    effect.id = QUuid::createUuid().toString();
    writeEffect(effect);

    auto row = m_effects.size();

    beginInsertRows(QModelIndex(), row, row);
    m_effects << effect;
    endInsertRows();

    writeOrder();

    return row;
}

void CustomEffectModel::setEffectName(const int &row, const QString &name)
{
    qInfo() << "Rename custom effect" << row << "to" << name;

    m_effects[row].name = name;
    writeEffect(m_effects.at(row));

    emit dataChanged(index(row), index(row));
}

void CustomEffectModel::setEffectContent(const int &row, const CustomEffect &content)
{
    qInfo() << "Update custom effect" << m_effects.at(row).name;

    m_effects[row].keys = content.keys;
    m_effects[row].order = content.order;
    m_effects[row].sequence = content.sequence;
    m_effects[row].animation = content.animation;
    writeEffect(m_effects.at(row));

    emit dataChanged(index(row), index(row));
}

void CustomEffectModel::deleteEffect(const int &row)
{
    const CustomEffect& effect = m_effects.at(row);

    qInfo() << "Delete custom effect" << effect.name << "(" << effect.id << ")";

    QSettings settings;
    settings.beginGroup(QStringLiteral("custom_effects/%1").arg(m_keyboardID));
    settings.remove(effect.id);
    settings.endGroup();

    beginRemoveRows(QModelIndex(), row, row);
    m_effects.removeAt(row);
    endRemoveRows();

    writeOrder();
}

const CustomEffect &CustomEffectModel::effect(const int &row) const
{
    return m_effects.at(row);
}

int CustomEffectModel::indexOf(const QString &id) const
{
    for (int i = 0; i < m_effects.size(); i++)
    {
        if (m_effects.at(i).id == id) return i;
    }

    return -1;
}

QString CustomEffectModel::effectName(const int &row) const
{
    if (row < 0 || row >= m_effects.size()) return QString();

    return m_effects.at(row).name;
}

void CustomEffectModel::writeEffect(const CustomEffect &effect)
{
    QSettings settings;
    settings.beginGroup(QStringLiteral("custom_effects/%1/%2").arg(m_keyboardID, effect.id));

    settings.setValue(u"name"_s, effect.name);

    settings.remove(u"keys"_s);
    settings.beginGroup(u"keys"_s);
    for (auto it = effect.keys.cbegin(); it != effect.keys.cend(); it++)
    {
        settings.setValue(QString::number(it.key()), it.value());
    }
    settings.endGroup();

    QStringList order;
    for (auto&& bufferIndex : effect.order)
    {
        order << QString::number(bufferIndex);
    }
    settings.setValue(u"order"_s, order);

    settings.setValue(u"sequence"_s, LightAnimation::sequenceToStrings(effect.sequence));
    effect.animation.write(settings);

    settings.endGroup();
}

void CustomEffectModel::writeOrder()
{
    QStringList order;

    for (auto&& effect : m_effects)
    {
        order << effect.id;
    }

    QSettings settings;
    settings.setValue(QStringLiteral("custom_effects/%1/order").arg(m_keyboardID), order);
}
