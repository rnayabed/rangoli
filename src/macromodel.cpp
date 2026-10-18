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

#include "macromodel.h"

#include <QCryptographicHash>
#include <QSettings>

#include "keycode.h"

using namespace Qt::Literals::StringLiterals;

// Every slot is 64 events of 3 bytes, after a 2 byte header
static constexpr int SlotBytes = MacroModel::MaxEvents * 3;
static constexpr int AreaHeaderBytes = 2;

QString MacroEvent::text() const
{
    const int keyCode = modifier ? (code << 16) : (code << 8);
    const QString name = KeyCode::Names.value(static_cast<KeyCode::Code>(keyCode),
                                              u"0x%1"_s.arg(code, 2, 16, QChar(u'0')));

    return release ? QObject::tr("Release %1").arg(name) : QObject::tr("Press %1").arg(name);
}

MacroModel::MacroModel(QObject *parent)
    : QAbstractListModel{parent}
{
    m_macros.resize(SlotCount);
}

int MacroModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    return SlotCount;
}

QVariant MacroModel::data(const QModelIndex &index, int role) const
{
    if (role <= Qt::UserRole || index.row() >= m_macros.size())
    {
        return QVariant();
    }

    const Macro& macro = m_macros.at(index.row());

    if (role == NameRole)
    {
        return macroName(index.row());
    }
    else if (role == EventCountRole)
    {
        return macro.events.size();
    }
    else if (role == EmptyRole)
    {
        return macro.events.isEmpty();
    }

    return QVariant();
}

QHash<int, QByteArray> MacroModel::roleNames() const
{
    QHash<int, QByteArray> roles;

    roles[NameRole] = "name";
    roles[EventCountRole] = "eventCount";
    roles[EmptyRole] = "empty";

    return roles;
}

void MacroModel::load(const QString &keyboardID)
{
    beginResetModel();

    m_keyboardID = keyboardID;
    m_macros = QList<Macro>(SlotCount);

    QSettings settings;
    settings.beginGroup(u"macros"_s);
    settings.beginGroup(keyboardID);

    for (int slot = 0; slot < SlotCount; slot++)
    {
        settings.beginGroup(QString::number(slot));

        Macro& macro = m_macros[slot];
        macro.name = settings.value(u"name"_s).toString();

        // Each event is "release,modifier,code,delay"
        for (auto&& string : settings.value(u"events"_s).toStringList())
        {
            const QStringList parts = string.split(u',');
            if (parts.size() != 4) continue;

            MacroEvent event;
            event.release = parts.at(0) == u"1"_s;
            event.modifier = parts.at(1) == u"1"_s;
            event.code = quint8(parts.at(2).toInt());
            event.delayMs = parts.at(3).toInt();
            macro.events << event;
        }

        settings.endGroup();
    }

    settings.endGroup();
    settings.endGroup();

    endResetModel();
}

const Macro &MacroModel::macro(const int &slot) const
{
    return m_macros.at(slot);
}

void MacroModel::setMacro(const int &slot, const Macro &macro)
{
    if (slot < 0 || slot >= SlotCount) return;

    m_macros[slot] = macro;
    m_macros[slot].events = macro.events.mid(0, MaxEvents);

    writeMacro(slot);

    emit dataChanged(index(slot), index(slot));
}

QString MacroModel::macroName(const int &slot) const
{
    if (slot < 0 || slot >= SlotCount) return {};

    const QString& name = m_macros.at(slot).name;
    return name.isEmpty() ? tr("Macro %1").arg(slot + 1) : name;
}

bool MacroModel::isEmpty(const int &slot) const
{
    return slot < 0 || slot >= SlotCount || m_macros.at(slot).events.isEmpty();
}

QByteArray MacroModel::area() const
{
    QByteArray area(AreaBytes, '\0');

    // Always 04 00 in captures from the official software
    area[0] = 0x04;

    for (int slot = 0; slot < SlotCount; slot++)
    {
        const QList<MacroEvent>& events = m_macros.at(slot).events;
        int pos = AreaHeaderBytes + slot * SlotBytes;

        for (int i = 0; i < events.size(); i++)
        {
            const MacroEvent& event = events.at(i);

            // The last event doesn't wait for anything. Everything else waits at
            // least one step, so keys pressed together still reach the computer
            // in the order they were pressed (Ctrl before Tab)
            const int units = (i == events.size() - 1) ? 0
                    : qBound(1, qRound(event.delayMs / MsPerDelayUnit), MaxDelayUnits);

            area[pos++] = char((event.release ? 0x80 : 0x00) | (units & 0x7f));
            area[pos++] = char((event.modifier ? 0xf0 : 0x00) | ((units >> 7) & 0x0f));
            area[pos++] = char(event.code);
        }
    }

    return area;
}

bool MacroModel::areaChanged() const
{
    QSettings settings;

    // Nothing written yet counts as an empty area, so keyboards without
    // macros never get the area sent
    QByteArray empty(AreaBytes, '\0');
    empty[0] = 0x04;

    const QByteArray written = settings.value(u"macros/%1/written"_s.arg(m_keyboardID),
            QCryptographicHash::hash(empty, QCryptographicHash::Sha1).toHex()).toByteArray();

    return QCryptographicHash::hash(area(), QCryptographicHash::Sha1).toHex() != written;
}

void MacroModel::markAreaWritten()
{
    QSettings settings;
    settings.setValue(u"macros/%1/written"_s.arg(m_keyboardID),
                      QCryptographicHash::hash(area(), QCryptographicHash::Sha1).toHex());
}

int MacroModel::keyMapCode(const int &slot)
{
    // Played once per press; the RK96 software offers no other mode
    return 0x02000000 | (slot << 16) | 0x0100 | 0x01;
}

bool MacroModel::isMacroCode(const int &code)
{
    return ((code >> 24) & 0xff) == 0x02;
}

int MacroModel::slotForCode(const int &code)
{
    return (code >> 16) & 0xff;
}

void MacroModel::writeMacro(const int &slot)
{
    const Macro& macro = m_macros.at(slot);

    QStringList events;

    for (auto&& event : macro.events)
    {
        events << u"%1,%2,%3,%4"_s.arg(event.release ? 1 : 0)
                  .arg(event.modifier ? 1 : 0)
                  .arg(event.code)
                  .arg(event.delayMs);
    }

    QSettings settings;
    settings.beginGroup(u"macros"_s);
    settings.beginGroup(m_keyboardID);
    settings.beginGroup(QString::number(slot));
    settings.setValue(u"name"_s, macro.name);
    settings.setValue(u"events"_s, events);
    settings.endGroup();
    settings.endGroup();
    settings.endGroup();
}
