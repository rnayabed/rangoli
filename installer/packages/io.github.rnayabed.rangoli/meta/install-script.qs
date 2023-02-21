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

function Component()
{}

Component.prototype.createOperations = function()
{
    component.createOperations();

    if (systemInfo.kernelType === "linux") {
        component.addOperation("CreateDesktopEntry",
            "Rangoli.desktop",
            "Name=Rangoli\n" +
            "Type=Application\n" +
            "Terminal=false\n" +
            "Exec=@TargetDir@/bin/rangoli\n"+
            "Icon=@TargetDir@/bin/rangoli.svg\n" +
            "Categories=Settings;Utility\n" +
            "Keywords=Royal Kludge;RK;Keyboard\n" +
            "Comment=Start Rangoli\n" +
            "SingleMainWindow=true\n"
            );
        component.addOperation("CreateDesktopEntry",
            "Rangoli Maintenance Tool.desktop",
            "Name=Rangoli Maintenance Tool\n" +
            "Type=Application\n" +
            "Terminal=false\n" +
            "Exec=@TargetDir@/maintenancetool\n" +
            "Icon=@TargetDir@/bin/rangoli.svg\n" +
            "Categories=Settings;Utility\n" +
            "Keywords=Royal Kludge;RK;Keyboard\n" +
            "Comment=Start Rangoli Maintenance Tool\n" +
            "SingleMainWindow=true\n"
            );
    }
    else if (systemInfo.kernelType === "winnt")
    {
        component.addOperation("CreateShortcut",
            "@TargetDir@/bin/rangoli.exe",
            "@StartMenuDir@/Rangoli.lnk",
            "workingDirectory=@TargetDir@/bin",
            "iconPath=@TargetDir@/bin/rangoli.exe", "iconId=0",
            "description=Start Rangoli"
            );

        component.addOperation("CreateShortcut",
            "@TargetDir@/bin/rangoli.exe",
            "@DesktopDir@/Rangoli.lnk",
            "workingDirectory=@TargetDir@/bin",
            "iconPath=@TargetDir@/bin/rangoli.exe", "iconId=0",
            "description=Start Rangoli");
    }
    else if (systemInfo.kernelType === "darwin")
    {
        installer.execute("osascript", "-e", 'tell application "Finder" to make alias file to POSIX file "@TargetDir@/bin/rangoli.app" at desktop');
    }
}
