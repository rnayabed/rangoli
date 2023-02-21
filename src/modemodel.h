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

#ifndef MODESMODEL_H
#define MODESMODEL_H

#include <QAbstractItemModel>

#include "mode.h"

class ModeModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum ModeRoles {
        NameRole = Qt::UserRole + 1,
        ModeBitRole
    };

    enum class RGBModes : int {
        NeonStream = 1,
        RipplesShining,
        RotatingWindmill,
        SineWave,
        RainbowRoulette,
        StarsTwinkle,
        LayerUponLayer,
        RichAndHonored,
        MarqueeEffect,
        RotatingStorm,
        SerpentineHorseRace,
        RetroSnake,
        DiagonalTransformation,
        Custom,
        Ambilight,
        Streamer,
        Steady,
        Breathing,
        Neon,
        ShadowDisappear,
        FlashAway
    };

    enum class Modes : int {
        Steady = 1,
        Custom,
        Breathing,
        PressAndDestroy,
        NeonStream,
        Streamer,
        Ambilight,
        DripplingRipples,
        BrilliantPoint,
        FlashAway,
        ShadowDisappear,
        RipplesShining,
        RichAndHonored,
        MarqueeEffect,
        RotatingStorm,
        SerpentineHorseRace,
        StarsTwinkle,
        RetroSnake,
        DiagonalTransformation,
        SineWave,
    };

    explicit ModeModel(QObject* parent = nullptr);

    int neonStreamIndex();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    const int& getModeBit(const int& index) const;

    bool isCustomMode(const int& index) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QHash<int, QByteArray> roleNames() const override;

    void setRGB(const bool& rgb);

private:
    QList<Mode> m_modes;
    QList<Mode> m_rgbModes;

    bool m_rgb;

};

#endif // MODESMODEL_H
