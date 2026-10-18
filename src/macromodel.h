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

#ifndef MACROMODEL_H
#define MACROMODEL_H

#include <QAbstractListModel>
#include <QByteArray>
#include <QList>
#include <QString>

// One key going down or up, then a wait before the next event
struct MacroEvent
{
    bool release{false};

    // A modifier event's code is a HID modifier bit (0x01 Left Ctrl, 0x02 Left Shift, ...),
    // anything else is a HID keyboard usage
    bool modifier{false};
    quint8 code{0};

    int delayMs{0};

    QString text() const;
};

struct Macro
{
    QString name;
    QList<MacroEvent> events;
};

// The macros stored on the keyboard itself. The keyboard has a fixed number of
// slots, and a key plays a macro through its key map entry, which names the slot.
class MacroModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum MacroModelRoles {
        NameRole = Qt::UserRole + 1,
        EventCountRole,
        EmptyRole
    };

    static constexpr int SlotCount = 10;
    static constexpr int MaxEvents = 64;

    // The keyboard waits in steps of roughly 6.94 ms (measured), up to 11 bits of them
    static constexpr double MsPerDelayUnit = 6.94;
    static constexpr int MaxDelayUnits = 0x7ff;
    static constexpr int MaxDelayMs = int(MaxDelayUnits * MsPerDelayUnit);

    // The whole macro area is sent as this many reports of 62 bytes
    static constexpr int AreaPackets = 31;
    static constexpr int AreaBytes = AreaPackets * 62;

    explicit MacroModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QHash<int, QByteArray> roleNames() const override;

    void load(const QString& keyboardID);

    const Macro& macro(const int& slot) const;
    void setMacro(const int& slot, const Macro& macro);

    Q_INVOKABLE QString macroName(const int& slot) const;
    Q_INVOKABLE bool isEmpty(const int& slot) const;

    // The macro area as the keyboard stores it
    QByteArray area() const;

    // Whether the area differs from what was last sent to this keyboard model
    bool areaChanged() const;
    void markAreaWritten();

    // Key map entries that play a macro: 02 <slot> <mode> <times>
    static int keyMapCode(const int& slot);
    static bool isMacroCode(const int& code);
    static int slotForCode(const int& code);

private:
    QList<Macro> m_macros;
    QString m_keyboardID;

    void writeMacro(const int& slot);
};

#endif // MACROMODEL_H
