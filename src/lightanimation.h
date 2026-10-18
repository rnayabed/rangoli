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

#ifndef LIGHTANIMATION_H
#define LIGHTANIMATION_H

#include <QList>
#include <QSettings>
#include <QStringList>
#include <memory>

#include "key.h"

struct AnimationSettings
{
    enum Pattern {
        Sequence,
        WaveLeftRight,
        WaveTopBottom,
        Ripple,
        Breathing,
        Twinkle
    };

    enum Loop {
        Repeat,
        BackAndForth,
        Once
    };

    enum FadeCurve {
        Linear,
        Smooth,
        Comet
    };

    bool enabled{false};
    int pattern{Sequence};
    int stepMs{120};
    int fadeMs{600};
    int fadeInMs{0};
    int brightness{250};
    int loop{Repeat};
    int pauseMs{0};
    bool reverse{false};

    // Play a sequence's steps in a new random order every loop
    bool shuffle{false};

    // Keys of trail behind a moving light; 0 uses fadeMs instead
    int trailKeys{0};
    int fadeCurve{Linear};

    // A second, always repeating pattern across the whole keyboard that plays
    // under the main one; where both light a key the brighter wins
    struct Background
    {
        bool enabled{false};
        int pattern{Breathing};
        int stepMs{120};
        int fadeMs{1500};
        int fadeCurve{Smooth};
        int brightness{75};
        int pauseMs{0};
        bool reverse{false};
    };

    Background background;

    // The background layer as settings of its own
    AnimationSettings backgroundSettings() const;

    // Whether the pattern moves, so a trail in keys makes sense
    bool hasDirection() const;

    // Fade out time, worked out from the trail when one is set
    int effectiveFadeMs() const;

    // Read and write the "animation" subgroup of the current settings group
    void write(QSettings& settings) const;
    static AnimationSettings read(QSettings& settings);
};

// Renders host driven animations as a brightness level (0 - MaxLevel) per key
class LightAnimation
{
public:
    static constexpr int MaxLevel = 250;

    // selectedKeys are indexes into keys, in the order the user picked them.
    // sequence is the steps of a Sequence pattern, each a group of key indexes;
    // a key may appear in any number of steps, and an empty step is a pause.
    void prepare(const QList<Key>& keys, const QList<int>& selectedKeys,
                 const QList<QList<int>>& sequence, const AnimationSettings& settings);

    // Level for every key (indexed like keys) at the given time
    QList<int> render(qint64 ms) const;

    // True once a Loop::Once animation has played through. A background
    // layer keeps going after that, so it is never finished.
    bool finished(qint64 ms) const;

    // Sequences are saved as one string per step, keys joined by '+'.
    // A step with no keys is a pause and is saved as "-".
    static QStringList sequenceToStrings(const QList<QList<int>>& sequence);
    static QList<QList<int>> sequenceFromStrings(const QStringList& strings);

private:
    QList<Key> m_keys;
    AnimationSettings m_settings;

    // A key can take part more than once, e.g. a repeated sequence step
    QList<int> m_participants;
    QList<double> m_offsets;
    double m_span{0};

    // Sequence step of every participant, the steps that aren't pauses, and
    // a seed so each loop of a shuffled sequence gets its own order
    QList<int> m_steps;
    QList<int> m_keySteps;
    quint32 m_seed{0};

    // Position every sequence step plays at in the given loop
    QList<int> shuffledSteps(qint64 cycle) const;

    std::shared_ptr<LightAnimation> m_background;

    bool mainFinished(qint64 ms) const;

    qint64 cycleLength() const;
    int envelope(double age) const;
    double curve(double remaining) const;
    double breatheIn() const;
    double breatheOut() const;
};

#endif // LIGHTANIMATION_H
