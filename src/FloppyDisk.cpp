// -----------------------------------------------------------------------------
// This file is part of fddEMU "Floppy Disk Drive Emulator"
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

#include "FloppyDisk.h"
#include "DiskFile.h"
#include "UINotice.h" //msg.error
#include "fddEMU.h"
#include "petitfs/diskio.h"
#include <string.h> //memcpy

bool FloppyDisk::load(char *filename, bool showError)
{
    uint16_t totalSectors;
    uint8_t wbuf[18]; // working buffer;

    if (isReady())
        eject(); // if a disk is loaded, eject
    // open requested file
    if (!sdfile.getFileInfo((char *)s_dir, filename))
    {
        if (showError)
        {
            msg.error(err_notfound);
        }
        return false;
    }
    startSector = sdfile.getStartSector();
    if (sdfile.getReadOnly())
        flags.readonly = 1;

    if (disk_readp(wbuf, startSector, 54, 18)) // FileSystemType@54
    {
        msg.error(err_diskread);
        return false;
    }
#if DEBUG
    else
    {
        Serial.print(F("sd read okj.\n"));
    }
    Serial.print(F("FS: "));
    for (int i = 0; i < 5; i++)
        Serial.write(wbuf[i]);
    Serial.write('\n');
#endif                                                                                // DEBUG
    if ((wbuf[0] != 'F') || (wbuf[1] != 'A') || (wbuf[2] != 'T') || (wbuf[3] != '1')) // Not FAT. Raw image ?
    {
        totalSectors = sdfile.getFileSize() >> 9; // convert filesize to 512 byte sectors (filesize / 512)
        switch (totalSectors)                     // check filesize in sectors
        {                                         // Standart floppy: C*H*S*512
        case (uint16_t)(80 * 2 * 36):             // 3.5" HD   2880KB
            numTrack = 80;
            numSec = 36;
            bitLength = BIT_LENGTH_HD;
            flags.seclen = 2; // 512b sectors
            break;
        case (uint16_t)(80 * 2 * 18): // 3.5" HD   1440KB
            numTrack = 80;
            numSec = 18;
            bitLength = BIT_LENGTH_HD;
            flags.seclen = 2; // 512b sectors
            break;
        case (uint16_t)(80 * 2 * 9): // 3.5" DD   720KB
            numTrack = 80;
            numSec = 9;
            bitLength = BIT_LENGTH_DD;
            flags.seclen = 2; // 512b sectors
            break;
        case (uint16_t)(80 * 2 * 15): // 5.25" HD  1.2MB
            numTrack = 80;
            numSec = 15;
            bitLength = BIT_LENGTH_HD;
            flags.seclen = 2; // 512b sectors
            break;
        case (uint16_t)(40 * 2 * 9): // 5.25" DD  360KB
            numTrack = 40;
            numSec = 9;
            bitLength = BIT_LENGTH_DD;
            flags.seclen = 2; // 512b sectors
            break;
        case (uint16_t)(80 * 2 * 16 / 2): // 80 track 2 sided TR-DOS image
            numTrack = 80;
            numSec = 16;
            bitLength = BIT_LENGTH_DD;
            flags.seclen = 1; // 256b sectors
            break;
        case (uint16_t)(40 * 2 * 16 / 2): // 80 track 2 sided TR-DOS image
            numTrack = 40;
            numSec = 16;
            bitLength = BIT_LENGTH_DD;
            flags.seclen = 1; // 256b sectors
            break;
        default: // not a standart raw floppy image
            msg.error(err_invfile);
            return false;
        } // switch
    }
    else // Valid FAT boot record
    {
        disk_readp(wbuf, startSector, 11, 18); // WbytesPerSector@11 WnumHeads@26 WsectorsPerTrack@24
#if DEBUG
        Serial.print(F("BPS: "));
        Serial.print(*(int16_t *)wbuf);
        Serial.print(F(" Heads: "));
        Serial.print(*(int16_t *)(wbuf + 15));
        Serial.print(F(" SPT: "));
        Serial.print(*(int16_t *)(wbuf + 13));
        Serial.print(F(" Total Sec: "));
        Serial.print(*(uint16_t *)(wbuf + 8));
        Serial.write('\n');
#endif
        if ((*(int16_t *)wbuf != 512) || (*(int16_t *)(wbuf + 15) > 2) || (*(int16_t *)(wbuf + 13) > 255))
        {
            msg.error(err_geometry);
            return false;
        }
        flags.seclen = 2;                                  // 512b sectors - we have already checked above
        totalSectors = (uint16_t) * (int16_t *)(wbuf + 8); // WtotalSectors@19
        if (totalSectors > (sdfile.getFileSize() >> 9))
        {
            msg.error(err_geombig);
            return false;
        }
        numSec = (uint8_t) * (int16_t *)(wbuf + 13); // WsectorsPerTrack@24
        numTrack = (uint8_t)(totalSectors / (numSec * 2));
        bitLength = (numSec >= 15) ? BIT_LENGTH_HD : BIT_LENGTH_DD; // Decide according to numSec, >=15 HD else DD
    }
    // After all the checks load image file
    memcpy(fName, filename, 13);
    flags.ready = 1;
    return true;
}

void FloppyDisk::eject(void)
{
    startSector = 0;
    memset(fName, 0, 13); // clear disk file name
    numTrack = 80;        // default for 3.5" HD Floppy
    numSec = 18;          // default for 3.5" HD Floppy
    // clear flags & set DISK CHANGED
    flags.changed = 1;
    flags.ready = 0;
    flags.readonly = 0;
    flags.vdisk = 0;
    flags.seclen = 2; // default 512b
}

FloppyDisk::FloppyDisk(void)
{
    eject();
}

void FloppyDisk::loadVirtualDisk()
{
#if ENABLE_VFFS
    if (isReady())
        eject();                    // if a disk is loaded, eject
    memcpy_P(fName, str_label, 13); // set disk name to FDDEMU
    flags.vdisk = 1;
    flags.seclen = 2; // 512b sectors
    bitLength = BIT_LENGTH_HD;
#endif // ENABLE_VFFS
}
