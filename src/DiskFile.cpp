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

#include "DiskFile.h"
#include "UINotice.h" //msg.show
#include "fddEMU.h"
#include <string.h>

DiskFile sdfile; // we will use as extern

bool DiskFile::initSD()
{
    // initialize the SD card
    res = pf_mount(&fs);
    if (res == FR_OK)
        sdInitialized = true;
    else
        return false;
    nFiles = 0;
    scanFiles((char *)s_dir); // get number of files on SD root Dir
    return true;
}

DiskFile::DiskFile()
{
    sdInitialized = false;
    initSD();
}

void DiskFile::printFileName()
{
#if ENABLE_SERIAL || DEBUG
    if (fno.fattrib & AM_DIR)
        Serial.write('[');
    Serial.print(fno.fname);
    if (fno.fattrib & AM_DIR)
        Serial.write(']');
#endif // ENABLE_SERIAL || DEBUG
}

int16_t DiskFile::scanFiles(char *path)
{
    nFiles = 0;
    if (!openDir(path))
        return 0;
    while (getNextFile())
    {
#if ENABLE_WDT
        wdt_reset();
#endif // ENABLE_WDT
        nFiles++;
#if DEBUG
        printFileName();
        Serial.write('\n');
#endif // DEBUG
    }
    return nFiles;
}

bool DiskFile::openDir(char *path)
{
    if ((res = pf_opendir(&dir, path)) == FR_OK)
        return true;
    else if (res & (FR_DISK_ERR | FR_NOT_READY))
    {
        if (initSD())                                    // reinitialize SD
            if ((res = pf_opendir(&dir, path)) == FR_OK) // retry
                return true;
    }
    return false;
}

bool DiskFile::getNextEntry()
{
    res = pf_readdir(&dir, &fno);
    if (res != FR_OK)
        return false;
    if (fno.fname[0] == 0)
        return false;
    return true;
}

uint8_t DiskFile::getNextFile()
{
    if (!getNextEntry())
        return 0;

    if (fno.fattrib & (AM_DIR))
        return FTYPE_DIR;

    return FTYPE_FILE;
}

bool DiskFile::getFileInfo(char *path, char *filename)
{
    if (!openDir(path))
        return false;
    while (getNextEntry())
    {
#if ENABLE_WDT
        wdt_reset();
#endif // ENABLE_WDT
        if (strcmp(fno.fname, filename) == 0)
            return true;
    }
    return false;
}

uint32_t DiskFile::getStartSector()
{
    if (IsRootDir()) // root directory
    {
        res = pf_open(fno.fname);
    }
    else
    {
        char fullpath[255];
        uint8_t i = 0;
        uint8_t j = 0;

        while (s_dir[j] != '\0')
        {
            fullpath[i] = s_dir[j];
            i++;
            j++;
        }

        if (fullpath[i - 1] != '/')
        {
            fullpath[i] = '/';
            i++;
        }

        j = 0;
        while (fno.fname[j] != '\0')
        {
            fullpath[i] = fno.fname[j];
            i++;
            j++;
        }

        if (i < 245)
            fullpath[i] = '\0';
        else
            fullpath[254] = '\0';

        res = pf_open(fullpath);

#if DEBUG
        Serial.print_P("Path: ");
        Serial.print(fullpath);
        Serial.write('\n');
#endif // DEBUG
    }

    if (res != FR_OK)
    {
        msg.error(err_fopen);
        return 0;
    }
    if (!isContiguousFile()) // we will use direct sector access so we need a contiguous file
    {
        msg.error(err_noncontig);
        return 0;
    }
    return get_start_sector();
}