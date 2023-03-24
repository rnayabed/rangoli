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

#include "profilemodel.h"

#include <QSettings>

using namespace Qt::Literals::StringLiterals;

ProfileModel::ProfileModel(QObject *parent)
    : QAbstractListModel{parent}, m_defaultProfileIndex{0}
{}

int ProfileModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    return m_profiles.size();
}

const QString ProfileModel::profileName(const int &index)
{
    return data(this->index(index), ProfileModel::NameRole).toString();
}

QVariant ProfileModel::data(const QModelIndex &index, int role) const
{
    if (role <= Qt::UserRole || index.row() >= m_profiles.size())
    {
        return QVariant();
    }

    if (role == IDRole)
    {
        return m_profiles.at(index.row()).id;
    }
    else if (role == NameRole)
    {
        return m_profiles.at(index.row()).name;
    }

    return QVariant();
}

bool ProfileModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!hasIndex(index.row(), index.column(), index.parent()))
        return false;

    if (role == IDRole)
    {
        m_profiles[index.row()].id = value.toString();
        emit dataChanged(index, index);
        return true;
    }
    else if (role == NameRole)
    {
        m_profiles[index.row()].name = value.toString();
        emit dataChanged(index, index);
        return true;
    }

    return false;
}

int ProfileModel::createNewProfile(const QString& name)
{
    qInfo() << "Create profile" << name;
    QSettings settings;

    settings.beginGroup(u"profiles"_s);

    Profile p{name};
    settings.beginGroup(p.id);
    settings.setValue(u"name"_s, p.name);
    settings.endGroup();

    auto index = m_profiles.size();

    beginInsertRows(QModelIndex(), index, index);
    m_profiles << p;
    endInsertRows();

    return index;
}

void ProfileModel::setProfileName(const qsizetype &row, const QString &name)
{
    qInfo() << "Change profile" << row << "name to" << name;

    setData(index(row, 0), name, ProfileModel::NameRole);

    QSettings settings;
    settings.beginGroup(u"profiles"_s);

    const Profile& p = m_profiles.at(row);
    settings.beginGroup(p.id);
    settings.setValue(u"name"_s, p.name);
    settings.endGroup();
}

void ProfileModel::deleteProfile(const qsizetype &row)
{
    const Profile& profile = m_profiles.at(row);

    qInfo() << "Delete profile" << profile.name << "(" << profile.id <<")";
    QSettings settings;

    settings.beginGroup(u"profiles"_s);
    settings.remove(profile.id);
    settings.endGroup();

    beginRemoveRows(QModelIndex(), row, row);
    m_profiles.removeAt(row);
    endRemoveRows();

    if (m_defaultProfileIndex >= row)
    {
        m_defaultProfileIndex--;
        emit defaultProfileIndexChanged();
    }
}

QHash<int, QByteArray> ProfileModel::roleNames() const
{
    QHash <int, QByteArray> roles;

    roles[IDRole] = "id";
    roles[NameRole] = "name";

    return roles;
}

int& ProfileModel::defaultProfileIndex()
{
    return m_defaultProfileIndex;
}

void ProfileModel::registerProfiles()
{
    qInfo() << "Register profiles";

    QSettings settings;

    m_defaultProfileIndex = 0;
    m_profiles.clear();
    QString defaultID = settings.value(u"default_profile"_s).toString();

    settings.beginGroup(u"profiles"_s);

    QStringList profileIDs = settings.childGroups();

    for (qsizetype i = 0; i < profileIDs.size(); i++)
    {
        const QString& ID = profileIDs.at(i);
        settings.beginGroup(ID);

        beginInsertRows(QModelIndex(), m_profiles.size(), m_profiles.size());
        m_profiles << Profile{ID, settings.value(u"name"_s).toString()};
        endInsertRows();

        settings.endGroup();

        if (!defaultID.isNull() && defaultID == ID)
        {
            m_defaultProfileIndex = i;
        }
    }

    if (m_profiles.size() == 0)
    {
        qInfo() << "No profiles found. Create default profile";
        createNewProfile(tr("Default Profile"));
    }

    settings.endGroup();
}

void ProfileModel::setDefaultProfileIndex(const int &row)
{
    if (m_defaultProfileIndex == row) return;

    QSettings settings;
    settings.setValue(u"default_profile"_s, m_profiles.at(row).id);

    m_defaultProfileIndex = row;
    emit defaultProfileIndexChanged();
}
