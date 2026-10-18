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

#include "lightanimation.h"

#include <QLineF>
#include <QPointF>
#include <QRandomGenerator>
#include <QtMath>
#include <limits>

using namespace Qt::Literals::StringLiterals;

// Approximate distance between neighbouring key centres on the layout images,
// used to turn positions into "steps" for the wave and ripple patterns
static constexpr double KeyPitch = 49.0;

void AnimationSettings::write(QSettings &settings) const
{
    settings.beginGroup(u"animation"_s);
    settings.setValue(u"enabled"_s, enabled);
    settings.setValue(u"pattern"_s, pattern);
    settings.setValue(u"step_ms"_s, stepMs);
    settings.setValue(u"fade_ms"_s, fadeMs);
    settings.setValue(u"fade_in_ms"_s, fadeInMs);
    settings.setValue(u"reverse"_s, reverse);
    settings.setValue(u"shuffle"_s, shuffle);
    settings.setValue(u"trail_keys"_s, trailKeys);
    settings.setValue(u"fade_curve"_s, fadeCurve);
    settings.setValue(u"brightness"_s, brightness);
    settings.setValue(u"loop"_s, loop);
    settings.setValue(u"pause_ms"_s, pauseMs);

    settings.beginGroup(u"background"_s);
    settings.setValue(u"enabled"_s, background.enabled);
    settings.setValue(u"pattern"_s, background.pattern);
    settings.setValue(u"step_ms"_s, background.stepMs);
    settings.setValue(u"fade_ms"_s, background.fadeMs);
    settings.setValue(u"fade_curve"_s, background.fadeCurve);
    settings.setValue(u"brightness"_s, background.brightness);
    settings.setValue(u"pause_ms"_s, background.pauseMs);
    settings.setValue(u"reverse"_s, background.reverse);
    settings.endGroup();

    settings.endGroup();
}

AnimationSettings AnimationSettings::read(QSettings &settings)
{
    AnimationSettings a;

    settings.beginGroup(u"animation"_s);
    a.enabled = settings.value(u"enabled"_s, a.enabled).toBool();
    a.pattern = settings.value(u"pattern"_s, a.pattern).toInt();
    a.stepMs = settings.value(u"step_ms"_s, a.stepMs).toInt();
    a.fadeMs = settings.value(u"fade_ms"_s, a.fadeMs).toInt();
    a.fadeInMs = settings.value(u"fade_in_ms"_s, a.fadeInMs).toInt();
    a.reverse = settings.value(u"reverse"_s, a.reverse).toBool();
    a.shuffle = settings.value(u"shuffle"_s, a.shuffle).toBool();
    a.trailKeys = settings.value(u"trail_keys"_s, a.trailKeys).toInt();
    a.fadeCurve = settings.value(u"fade_curve"_s, a.fadeCurve).toInt();
    a.brightness = settings.value(u"brightness"_s, a.brightness).toInt();
    a.loop = settings.value(u"loop"_s, a.loop).toInt();
    a.pauseMs = settings.value(u"pause_ms"_s, a.pauseMs).toInt();

    Background& b = a.background;
    settings.beginGroup(u"background"_s);
    b.enabled = settings.value(u"enabled"_s, b.enabled).toBool();
    b.pattern = settings.value(u"pattern"_s, b.pattern).toInt();
    b.stepMs = settings.value(u"step_ms"_s, b.stepMs).toInt();
    b.fadeMs = settings.value(u"fade_ms"_s, b.fadeMs).toInt();
    b.fadeCurve = settings.value(u"fade_curve"_s, b.fadeCurve).toInt();
    b.brightness = settings.value(u"brightness"_s, b.brightness).toInt();
    b.pauseMs = settings.value(u"pause_ms"_s, b.pauseMs).toInt();
    b.reverse = settings.value(u"reverse"_s, b.reverse).toBool();
    settings.endGroup();

    settings.endGroup();

    return a;
}

bool AnimationSettings::hasDirection() const
{
    return pattern == Sequence || pattern == WaveLeftRight
            || pattern == WaveTopBottom || pattern == Ripple;
}

AnimationSettings AnimationSettings::backgroundSettings() const
{
    AnimationSettings a;
    a.enabled = background.enabled;
    a.pattern = background.pattern;
    a.stepMs = background.stepMs;
    a.fadeMs = background.fadeMs;
    a.fadeCurve = background.fadeCurve;
    a.brightness = background.brightness;
    a.pauseMs = background.pauseMs;
    a.reverse = background.reverse;
    a.loop = Repeat;
    return a;
}

int AnimationSettings::effectiveFadeMs() const
{
    return (trailKeys > 0 && hasDirection()) ? trailKeys * stepMs : fadeMs;
}

void LightAnimation::prepare(const QList<Key> &keys, const QList<int> &selectedKeys,
                             const QList<QList<int>> &sequence, const AnimationSettings &settings)
{
    m_keys = keys;
    m_settings = settings;
    m_participants.clear();
    m_offsets.clear();
    m_steps.clear();
    m_keySteps.clear();
    m_span = 0;
    m_seed = QRandomGenerator::global()->generate();
    m_background.reset();

    if (m_keys.isEmpty()) return;

    // A sequence makes no sense without its own steps, so the background
    // never is one; it always covers the whole keyboard
    if (m_settings.background.enabled && m_settings.background.pattern != AnimationSettings::Sequence)
    {
        m_background = std::make_shared<LightAnimation>();
        m_background->prepare(m_keys, {}, {}, m_settings.backgroundSettings());
    }

    if (m_settings.pattern == AnimationSettings::Sequence)
    {
        for (int n = 0; n < sequence.size(); n++)
        {
            if (!sequence.at(n).isEmpty()) m_keySteps << n;

            for (auto&& i : sequence.at(n))
            {
                if (i < 0 || i >= m_keys.size()) continue;

                m_participants << i;
                m_steps << n;
                m_offsets << double(n) * m_settings.stepMs;
            }
        }

        if (!m_offsets.isEmpty())
        {
            m_span = double(sequence.size() - 1) * m_settings.stepMs;
        }

        if (m_settings.reverse)
        {
            for (auto& offset : m_offsets) offset = m_span - offset;
        }

        return;
    }

    // Everything else uses the whole keyboard unless the user picked at least two keys
    if (selectedKeys.size() <= 1)
    {
        for (int i = 0; i < m_keys.size(); i++) m_participants << i;
    }
    else
    {
        m_participants = selectedKeys;
    }

    auto centre = [this](int i) {
        const Key& k = m_keys.at(i);
        return QPointF((k.topX + k.bottomX) / 2.0, (k.topY + k.bottomY) / 2.0);
    };

    double minX = std::numeric_limits<double>::max();
    double minY = std::numeric_limits<double>::max();
    double maxX = std::numeric_limits<double>::lowest();
    double maxY = std::numeric_limits<double>::lowest();

    for (auto&& i : m_participants)
    {
        const QPointF c = centre(i);
        minX = qMin(minX, c.x());
        minY = qMin(minY, c.y());
        maxX = qMax(maxX, c.x());
        maxY = qMax(maxY, c.y());
    }

    // Ripples start from the first picked key, or the middle of the keyboard
    const QPointF origin = selectedKeys.isEmpty()
            ? QPointF((minX + maxX) / 2, (minY + maxY) / 2)
            : centre(selectedKeys.first());

    for (int n = 0; n < m_participants.size(); n++)
    {
        const QPointF c = centre(m_participants.at(n));
        double offset = 0;

        switch (m_settings.pattern)
        {
        case AnimationSettings::WaveLeftRight:
            offset = (c.x() - minX) / KeyPitch * m_settings.stepMs;
            break;
        case AnimationSettings::WaveTopBottom:
            offset = (c.y() - minY) / KeyPitch * m_settings.stepMs;
            break;
        case AnimationSettings::Ripple:
            offset = QLineF(origin, c).length() / KeyPitch * m_settings.stepMs;
            break;
        default:
            break;
        }

        m_offsets << offset;
        m_span = qMax(m_span, offset);
    }

    // Right to left, bottom to top, or a ripple coming inwards
    if (m_settings.reverse)
    {
        for (auto& offset : m_offsets) offset = m_span - offset;
    }
}

QList<int> LightAnimation::render(qint64 ms) const
{
    QList<int> levels = m_background ? m_background->render(ms) : QList<int>(m_keys.size(), 0);

    if (m_participants.isEmpty() || mainFinished(ms)) return levels;

    const qint64 cycle = cycleLength();
    const qint64 n = ms / cycle;
    const double t = ms % cycle;

    if (m_settings.pattern == AnimationSettings::Breathing)
    {
        const double in = breatheIn();
        const double out = breatheOut();

        double level = 0;

        if (t < in)
        {
            level = m_settings.brightness * (0.5 - 0.5 * qCos(M_PI * t / in));
        }
        else if (t < in + out)
        {
            level = m_settings.brightness * (0.5 + 0.5 * qCos(M_PI * (t - in) / out));
        }

        for (auto&& i : m_participants) levels[i] = qMax(levels.at(i), qRound(level));
    }
    else if (m_settings.pattern == AnimationSettings::Twinkle)
    {
        // Every key twinkles once per cycle at a random time; seeding with the
        // cycle number keeps a cycle identical for every frame rendered from it
        QRandomGenerator rng(quint32(n) + 1);
        const quint32 window = qMax<quint32>(1, m_settings.stepMs * m_participants.size());

        for (auto&& i : m_participants) levels[i] = qMax(levels.at(i), envelope(t - rng.bounded(window)));
    }
    else
    {
        const bool reverse = m_settings.loop == AnimationSettings::BackAndForth && (n % 2 == 1);
        const bool shuffle = m_settings.shuffle && m_settings.pattern == AnimationSettings::Sequence;

        // Where every step plays in this loop
        const QList<int> order = shuffle ? shuffledSteps(n) : QList<int>{};

        for (int k = 0; k < m_participants.size(); k++)
        {
            double offset = shuffle ? double(order.at(m_steps.at(k))) * m_settings.stepMs : m_offsets.at(k);
            if (reverse) offset = m_span - offset;
            const int i = m_participants.at(k);

            // Overlapping flashes of the same key show as the brighter one
            levels[i] = qMax(levels.at(i), envelope(t - offset));
        }
    }

    return levels;
}

QStringList LightAnimation::sequenceToStrings(const QList<QList<int>> &sequence)
{
    QStringList strings;

    for (auto&& step : sequence)
    {
        if (step.isEmpty())
        {
            strings << u"-"_s;
            continue;
        }

        QStringList keys;
        for (auto&& key : step) keys << QString::number(key);
        strings << keys.join(u'+');
    }

    return strings;
}

QList<QList<int>> LightAnimation::sequenceFromStrings(const QStringList &strings)
{
    QList<QList<int>> sequence;

    for (auto&& string : strings)
    {
        if (string == u"-"_s)
        {
            sequence << QList<int>{};
            continue;
        }

        QList<int> step;
        for (auto&& key : string.split(u'+', Qt::SkipEmptyParts)) step << key.toInt();
        if (!step.isEmpty()) sequence << step;
    }

    return sequence;
}

QList<int> LightAnimation::shuffledSteps(qint64 cycle) const
{
    // Steps keep their place by default; pauses never move, so the rhythm of
    // the sequence stays the same and only which keys light up changes
    QList<int> order(m_steps.isEmpty() ? 0 : m_steps.last() + 1);
    for (int n = 0; n < order.size(); n++) order[n] = n;

    QList<int> places = m_keySteps;
    QRandomGenerator rng(m_seed + quint32(cycle));

    for (qsizetype n = places.size() - 1; n > 0; n--)
    {
        places.swapItemsAt(n, rng.bounded(int(n) + 1));
    }

    for (int j = 0; j < m_keySteps.size(); j++) order[m_keySteps.at(j)] = places.at(j);

    return order;
}

bool LightAnimation::finished(qint64 ms) const
{
    return !m_background && mainFinished(ms);
}

bool LightAnimation::mainFinished(qint64 ms) const
{
    return m_settings.loop == AnimationSettings::Once && ms >= cycleLength();
}

qint64 LightAnimation::cycleLength() const
{
    qint64 length;

    switch (m_settings.pattern)
    {
    case AnimationSettings::Breathing:
        length = qint64(breatheIn() + breatheOut());
        break;
    case AnimationSettings::Twinkle:
        length = qint64(m_settings.stepMs) * m_participants.size() + m_settings.fadeInMs
                + m_settings.effectiveFadeMs();
        break;
    default:
        // Without a fade out each key stays lit for one step instead
        length = qint64(m_span) + m_settings.fadeInMs + qMax(m_settings.effectiveFadeMs(), m_settings.stepMs);
        break;
    }

    return qMax<qint64>(1, length + m_settings.pauseMs);
}

double LightAnimation::breatheIn() const
{
    // Without a separate fade in, breathe in as long as out
    return m_settings.fadeInMs > 0 ? m_settings.fadeInMs : breatheOut();
}

double LightAnimation::breatheOut() const
{
    return qMax(m_settings.fadeMs, 100);
}

int LightAnimation::envelope(double age) const
{
    if (age < 0) return 0;

    if (m_settings.fadeInMs > 0)
    {
        if (age < m_settings.fadeInMs)
        {
            // The fade in mirrors the fade out
            return qRound(m_settings.brightness * curve(age / m_settings.fadeInMs));
        }

        age -= m_settings.fadeInMs;
    }

    const int fadeMs = m_settings.effectiveFadeMs();

    if (fadeMs <= 0)
    {
        return age < m_settings.stepMs ? m_settings.brightness : 0;
    }

    if (age >= fadeMs) return 0;

    return qRound(m_settings.brightness * curve(1.0 - age / fadeMs));
}

double LightAnimation::curve(double remaining) const
{
    switch (m_settings.fadeCurve)
    {
    case AnimationSettings::Smooth:
        return 0.5 - 0.5 * qCos(M_PI * remaining);
    case AnimationSettings::Comet:
        // Bright head that drops quickly into a long faint tail
        return remaining * remaining * remaining;
    default:
        return remaining;
    }
}
