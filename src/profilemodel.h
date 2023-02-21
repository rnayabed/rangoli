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

#ifndef PROFILEMODEL_H
#define PROFILEMODEL_H

#include <QAbstractListModel>

#include "profile.h"

class ProfileModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int defaultProfileIndex READ defaultProfileIndex WRITE setDefaultProfileIndex NOTIFY defaultProfileIndexChanged)

public:
    enum ProfileModelRoles {
        IDRole = Qt::UserRole + 1,
        NameRole
    };

    explicit ProfileModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    Q_INVOKABLE const QString profileName(const int& index);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    int createNewProfile(const QString& name);

    // FIXME: Change all qsizetype to int
    void setProfileName(const qsizetype& row, const QString& name);
    void deleteProfile(const qsizetype& row);

    QHash<int, QByteArray> roleNames() const override;

    int& defaultProfileIndex();

    void registerProfiles();

signals:
    void defaultProfileIndexChanged();

public slots:
    void setDefaultProfileIndex(const int& row);

private:
    QList<Profile> m_profiles;
    int m_defaultProfileIndex;

};

#endif // PROFILEMODEL_H
