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

#ifndef STARTATBOOT_H
#define STARTATBOOT_H

#include <QProcess>

class StartAtBoot
{
public:

#if defined(Q_OS_LINUX) or defined(Q_OS_WIN) or defined(Q_OS_MACOS)
    static inline constexpr bool Supported = true;
#else
    static inline constexpr bool Supported = false;
#endif

    static bool createStarter();
    static bool deleteStarter();

private slots:
    static void linuxSystemDDaemonReloadFinished(int exitCode, QProcess::ExitStatus exitStatus);
    static void macOSLaunchCtlLoadFinished(int exitCode, QProcess::ExitStatus exitStatus);
};

#endif // STARTATBOOT_H
