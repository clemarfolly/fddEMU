// -----------------------------------------------------------------------------
// fddEMU "Floppy Disk Drive Emulator"
// Copyright (C) 2021 Acemi Elektronikci
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
// -----------------------------------------------------------------------------
#include "DiskFile.h"
#include "FloppyDrive.h"
#include "fddEMU.h"
#include "petitfs/pff.h"
#include "serial/SerialUI.h"
#include <avr/io.h>

char s_dir[255] = "/\0"; 

#if ENABLE_GUI
#if ENABLE_LCD1602
LCDHD44780 disp;
#else
GraphicUI disp;
#endif
#endif

int main(void)
{
#if ENABLE_GUI
    disp.init();
#endif // ENABLE_GUI

#if ENABLE_WDT
    wdt_enable(WDTO_8S);
#endif // ENABLE_WDT
#if ENABLE_SERIAL || DEBUG
    Serial.init(115200);
    ser.intro();
#endif // ENABLE_SERIAL || DEBUG
    if (sdfile.sdInitialized)
        drive[0].load((char *)s_bootfile, false); // if there is "BOOT.IMG" on SD load it
#if ENABLE_VFFS
    else
        drive[0].loadVirtualDisk();
#endif // ENABLE_VFFS
    for (;;)
    {
#if ENABLE_WDT
        wdt_reset();
#endif // ENABLE_WDT
        if (ds.isDrvChanged())
        {
            uint8_t selectedDrive = ds.getDriveSel();
#if ENABLE_GUI
            disp.showDriveBusy(selectedDrive);
#endif // ENABLE_GUI
#if ENABLE_SERIAL || DEBUG
            Serial.print_P(str_busy); // very short message, take too long and get drive read errors
#endif                                // #if ENABLE_SERIAL || DEBUG
            drive[selectedDrive - 1].run();
#if ENABLE_VFFS
            vffs.loadImage(); // check if a drive is flagged for vffs load and load if it is
#endif                        // ENABLE_VFFS
#if ENABLE_GUI
            disp.showDriveIdle();
#endif // ENABLE_GUI
#if ENABLE_SERIAL || DEBUG
            Serial.print_P(str_drive);
            Serial.print_P(str_idle);
            Serial.write('\n');
#endif // ENABLE_SERIAL || DEBUG
        }
#if ENABLE_SERIAL
        if (rxReady)
            ser.readRx();
#endif // ENABLE_SERIAL
#if ENABLE_GUI
        if (adcReady)
            disp.buttonAction(abtn.read());
        disp.update();
#endif // ENABLE_GUI
    }
}
