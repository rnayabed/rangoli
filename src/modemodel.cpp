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

#include "modemodel.h"


ModeModel::ModeModel(QObject *parent)
    : QAbstractListModel(parent), m_rgb{true}
{
    m_rgbModes << Mode(tr("Custom"), static_cast<int>(RGBModes::Custom))
            << Mode(tr("Ambilight"), static_cast<int>(RGBModes::Ambilight))
            << Mode(tr("Breathing"), static_cast<int>(RGBModes::Breathing))
            << Mode(tr("Diagonal Transformation"), static_cast<int>(RGBModes::DiagonalTransformation))
            << Mode(tr("Flash Away"), static_cast<int>(RGBModes::FlashAway))
            << Mode(tr("Layer Upon Layer"), static_cast<int>(RGBModes::LayerUponLayer))
            << Mode(tr("Marquee Effect"), static_cast<int>(RGBModes::MarqueeEffect))
            << Mode(tr("Neon"), static_cast<int>(RGBModes::Neon))
            << Mode(tr("Neon Stream"), static_cast<int>(RGBModes::NeonStream))
            << Mode(tr("Rainbow Roulette"), static_cast<int>(RGBModes::RainbowRoulette))
            << Mode(tr("Retro Snake"), static_cast<int>(RGBModes::RetroSnake))
            << Mode(tr("Rich And Honored"), static_cast<int>(RGBModes::RichAndHonored))
            << Mode(tr("Ripples Shining"), static_cast<int>(RGBModes::RipplesShining))
            << Mode(tr("Rotating Storm"), static_cast<int>(RGBModes::RotatingStorm))
            << Mode(tr("Rotating Windmill"), static_cast<int>(RGBModes::RotatingWindmill))
            << Mode(tr("Serpentine Horse Race"), static_cast<int>(RGBModes::SerpentineHorseRace))
            << Mode(tr("Shadow Disappear"), static_cast<int>(RGBModes::ShadowDisappear))
            << Mode(tr("Sine Wave"), static_cast<int>(RGBModes::SineWave))
            << Mode(tr("Stars Twinkle"), static_cast<int>(RGBModes::StarsTwinkle))
            << Mode(tr("Steady"), static_cast<int>(RGBModes::Steady))
            << Mode(tr("Streamer"), static_cast<int>(RGBModes::Streamer));


    m_modes << Mode(tr("Custom"), static_cast<int>(Modes::Custom))
            << Mode(tr("Ambilight"), static_cast<int>(Modes::Ambilight))
            << Mode(tr("Breathing"), static_cast<int>(Modes::Breathing))
            << Mode(tr("Brilliant Point"), static_cast<int>(Modes::BrilliantPoint))
            << Mode(tr("Diagonal Transformation"), static_cast<int>(Modes::DiagonalTransformation))
            << Mode(tr("Drippling Ripples"), static_cast<int>(Modes::DripplingRipples))
            << Mode(tr("Flash Away"), static_cast<int>(Modes::FlashAway))
            << Mode(tr("Marquee Effect"), static_cast<int>(Modes::MarqueeEffect))
            << Mode(tr("Neon Stream"), static_cast<int>(Modes::NeonStream))
            << Mode(tr("Press And Destroy"), static_cast<int>(Modes::PressAndDestroy))
            << Mode(tr("Retro Snake"), static_cast<int>(Modes::RetroSnake))
            << Mode(tr("Rich And Honored"), static_cast<int>(Modes::RichAndHonored))
            << Mode(tr("Ripples Shining"), static_cast<int>(Modes::RipplesShining))
            << Mode(tr("Rotating Storm"), static_cast<int>(Modes::RotatingStorm))
            << Mode(tr("Serpentine Horse Race"), static_cast<int>(Modes::SerpentineHorseRace))
            << Mode(tr("Shadow Disappear"), static_cast<int>(Modes::ShadowDisappear))
            << Mode(tr("Sine Wave"), static_cast<int>(Modes::SineWave))
            << Mode(tr("Stars Twinkle"), static_cast<int>(Modes::StarsTwinkle))
            << Mode(tr("Steady"), static_cast<int>(Modes::Steady))
            << Mode(tr("Streamer"), static_cast<int>(Modes::Streamer));
}

int ModeModel::neonStreamIndex()
{
    return 8;
}

int ModeModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    return m_rgb ? m_rgbModes.size() : m_modes.size();
}

const int& ModeModel::getModeBit(const int &index) const
{
    return m_rgb ? m_rgbModes.at(index).modeBit : m_modes.at(index).modeBit;
}

bool ModeModel::isCustomMode(const int &index) const
{
    if (m_rgb)
    {
        return m_rgbModes.at(index).modeBit == static_cast<int>(RGBModes::Custom);
    }
    else
    {
        return m_modes.at(index).modeBit == static_cast<int>(Modes::Custom);
    }
}

QVariant ModeModel::data(const QModelIndex &index, int role) const
{
    if (role <= Qt::UserRole || index.row() >= (m_rgb ? m_rgbModes.size() : m_modes.size()))
    {
        return QVariant();
    }

    if (role == NameRole)
    {
        return m_rgb ? m_rgbModes.at(index.row()).name : m_modes.at(index.row()).name;
    }
    else if (role == ModeBitRole)
    {
        return m_rgb ? m_rgbModes.at(index.row()).modeBit : m_modes.at(index.row()).modeBit;
    }

    return QVariant();
}

QHash<int, QByteArray> ModeModel::roleNames() const
{
    QHash <int, QByteArray> roles;

    roles[NameRole] = "name";
    roles[ModeBitRole] = "modeBit";

    return roles;
}

void ModeModel::setRGB(const bool &rgb)
{
    if (m_rgb == rgb) return;

    beginResetModel();
    m_rgb = rgb;
    endResetModel();
}
