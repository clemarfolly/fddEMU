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
// ------------------------------------------------------------------------------

#ifndef FDDEMUCONST_H
#define FDDEMUCONST_H

#include <avr/pgmspace.h>

#if (!ENABLE_GUI) && (!ENABLE_SERIAL) //if no user interface
    #undef ENABLE_VFFS
    #define ENABLE_VFFS 1
#endif //(!ENABLE_GUI) && (!ENABLE_SERIAL)

#if ENABLE_WDT
	#include <avr/wdt.h>
#endif  //ENABLE_WDT
#if ENABLE_SERIAL
    #include "serial/simpleUART.h" //debug +itoa
    #include "serial/SerialUI.h"
#endif //ENABLE_SERIAL 
#if ENABLE_GUI
	#include "gui/GraphicUI.h"
	#include "gui/ADCButton.h"
#endif //ENABLE_GUI 
#if ENABLE_VFFS
    #include "vffs/VirtualFloppyFS.h"
#endif //ENABLE_VFFS

//Drive selection macros used in core and ui
#define DRIVE0 (1 << 0)     //1
#define DRIVE1 (1 << 1)     //2

const char s_RootDir[] = {'\0'};		//Don't make PROGMEM
const char s_bootfile[] = "BOOT.IMG";	//Don't make PROGMEM

const char str_fddEMU[]		PROGMEM = "fddEMU";
const char str_2021[]       PROGMEM = "(c) 2021";
const char str_acemi[]      PROGMEM = "Acemi";
const char str_elektron[]   PROGMEM = "Elektronikci";
const char str_usage[]		PROGMEM = "S: Select drive\nP: Previous\nN: Next\nL: Load\nE: Eject";
const char str_colon[]		PROGMEM = ": ";
const char str_drive[]		PROGMEM = "Drive ";
const char str_busy[]		PROGMEM = "BUSY\n";
const char str_idle[]		PROGMEM = " idle";
const char str_selected[]	PROGMEM = "Selected ";
const char str_nodisk[]		PROGMEM = "No disk";
const char str_none[]       PROGMEM = "none";
const char str_nofile[]		PROGMEM = "NO FILE";
const char str_eject[]		PROGMEM = "Ejected ";
const char str_cancel[]		PROGMEM = "Cancel\n";
const char str_loading[]	PROGMEM = "Loading ";
const char str_error[]      PROGMEM = "ERROR";
//Error strings
const char err_initSD[]		PROGMEM = "Initialize SD card";
const char err_fopen[] 		PROGMEM = "File open failed";
const char err_noncontig[]	PROGMEM = "Non-contiguous file";
const char err_diskread[]	PROGMEM = "Read SD failed";
const char err_diskwrite[]	PROGMEM = "Write SD failed";
const char err_notfound[]	PROGMEM = "Not found";
const char err_invfile[]	PROGMEM = "Unrecognized image";
const char err_geometry[] 	PROGMEM = "Incompat geometry";
const char err_geombig[]	PROGMEM = "Geometry > file";
const char err_secSize[]    PROGMEM = "Invalid sector size!";
const char err_readFDC[]    PROGMEM = "Read error";
const char err_test[]		PROGMEM = "Testing 1, 2, 3";

//Virtual floppy things
const char str_label[]      PROGMEM = "FDDEMU";
const char str_disks[]   	PROGMEM = "DISKS";
const char str_load0name[]  PROGMEM = "DRVA";
const char str_load1name[]  PROGMEM = "DRVB";
const char str_loadext[]    PROGMEM = "TXT";
const char str_dot[]        PROGMEM = ".";
const char str_2dot[]       PROGMEM = "..";

//Images
#define floppy_width 46
#define floppy_height 54
const unsigned char floppy_bits[] PROGMEM = {
 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x01, 0x00, 0x00, 0x00, 0x80, 0x01,
 0x01, 0xC0, 0xFF, 0xFF, 0x01, 0x03, 0x01, 0xC0, 0xFF, 0x83, 0x01, 0x06,
 0x01, 0xC0, 0xFF, 0x83, 0x01, 0x0C, 0x01, 0xC0, 0xFF, 0x83, 0x01, 0x18,
 0x01, 0xC0, 0xFF, 0x83, 0x01, 0x30, 0x01, 0xC0, 0xFF, 0x83, 0x01, 0x20,
 0x01, 0xC0, 0xFF, 0x83, 0x01, 0x20, 0x01, 0xC0, 0xFF, 0x83, 0x01, 0x20,
 0x01, 0xC0, 0xFF, 0x83, 0x01, 0x20, 0x01, 0xC0, 0xFF, 0x83, 0x01, 0x20,
 0x01, 0xC0, 0xFF, 0xFF, 0x01, 0x20, 0x01, 0x00, 0x00, 0x00, 0x00, 0x20,
 0x01, 0x00, 0x00, 0x00, 0x00, 0x20, 0x01, 0x00, 0x00, 0x00, 0x00, 0x20,
 0x01, 0x00, 0x00, 0x00, 0x00, 0x20, 0x01, 0x00, 0x00, 0x00, 0x00, 0x20,
 0xC1, 0xFF, 0xFF, 0xFF, 0x7F, 0x20, 0xC1, 0xFF, 0xFF, 0xFF, 0x7F, 0x20,
 0xC1, 0xFF, 0xFF, 0xFF, 0x7F, 0x20, 0xC1, 0xFF, 0xFF, 0xFF, 0x7F, 0x20,
 0xC1, 0xFF, 0xFF, 0xFF, 0x7F, 0x20, 0xC1, 0xFF, 0xFF, 0xFF, 0x7F, 0x20,
 0xC1, 0xFF, 0xFF, 0xFF, 0x7F, 0x20, 0xC1, 0xFF, 0xFF, 0xFF, 0x7F, 0x20,
 0xC1, 0xFF, 0xFF, 0xFF, 0x7F, 0x20, 0xC1, 0x80, 0xF1, 0xC8, 0x73, 0x20,
 0xC1, 0x80, 0x71, 0xC8, 0x73, 0x20, 0xC1, 0xFC, 0x61, 0xC8, 0x73, 0x20,
 0xC1, 0xFC, 0x61, 0xC8, 0x73, 0x20, 0xC1, 0xFC, 0x69, 0xC9, 0x73, 0x20,
 0xC1, 0x80, 0x69, 0xC9, 0x73, 0x20, 0xC1, 0x80, 0x69, 0xC9, 0x73, 0x20,
 0xC1, 0xFC, 0x09, 0xC9, 0x73, 0x20, 0xC1, 0xFC, 0x09, 0xC9, 0x73, 0x20,
 0xC1, 0xFC, 0x89, 0x89, 0x71, 0x20, 0xC1, 0x00, 0x99, 0x19, 0x78, 0x20,
 0xC1, 0x00, 0x99, 0x39, 0x7C, 0x20, 0xC1, 0xFF, 0xFF, 0xFF, 0x7F, 0x20,
 0xC1, 0xFF, 0xFF, 0xFF, 0x7F, 0x20, 0xC1, 0xFF, 0xFF, 0xFF, 0x7F, 0x20,
 0xC1, 0xFF, 0xFF, 0xFF, 0x7F, 0x20, 0xC1, 0xFF, 0xFF, 0xFF, 0x7F, 0x20,
 0xC1, 0xFF, 0xFF, 0xFF, 0x7F, 0x20, 0xC1, 0xFF, 0xFF, 0xFF, 0x7F, 0x20,
 0xC1, 0xFF, 0xFF, 0xFF, 0x7F, 0x20, 0xC1, 0xFF, 0xFF, 0xFF, 0x7F, 0x20,
 0xC1, 0xFF, 0xFF, 0xFF, 0x7F, 0x20, 0xC1, 0xFF, 0xFF, 0xFF, 0x7F, 0x20,
 0x0D, 0x00, 0x00, 0x00, 0x00, 0x2C, 0x0D, 0x00, 0x00, 0x00, 0x00, 0x2C,
 0x01, 0x00, 0x00, 0x00, 0x00, 0x20, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x3F };

#define floppym_width 23
#define floppym_height 27
const unsigned char floppya_bits[] PROGMEM = {
 0xFF, 0xFF, 0x1F, 0x01, 0x00, 0x30, 0x81, 0xFF, 0x60, 0x81, 0x9F, 0x40,
 0x81, 0x9F, 0x40, 0x81, 0x9F, 0x40, 0x81, 0x9F, 0x40, 0x81, 0xFF, 0x40,
 0x01, 0x00, 0x40, 0xF1, 0xFF, 0x47, 0xF1, 0xFF, 0x47, 0xF1, 0xFB, 0x47,
 0xF1, 0xF9, 0x47, 0xF1, 0xF1, 0x47, 0xF1, 0x74, 0x46, 0xF1, 0x76, 0x46,
 0x71, 0xE6, 0x47, 0x71, 0xE6, 0x47, 0x31, 0xE0, 0x47, 0x31, 0xEF, 0x47,
 0x91, 0x4F, 0x46, 0x91, 0x4F, 0x46, 0xF1, 0xFF, 0x47, 0xF1, 0xFF, 0x47,
 0xF5, 0xFF, 0x57, 0x01, 0x00, 0x40, 0xFF, 0xFF, 0x7F };

const unsigned char floppyb_bits[] PROGMEM = {
 0xFF, 0xFF, 0x1F, 0x01, 0x00, 0x30, 0x81, 0xFF, 0x60, 0x81, 0x9F, 0x40,
 0x81, 0x9F, 0x40, 0x81, 0x9F, 0x40, 0x81, 0x9F, 0x40, 0x81, 0xFF, 0x40,
 0x01, 0x00, 0x40, 0xF1, 0xFF, 0x47, 0xF1, 0xFF, 0x47, 0xF1, 0xFF, 0x47,
 0x71, 0xF0, 0x47, 0x71, 0xE7, 0x47, 0x71, 0x6F, 0x46, 0x71, 0x67, 0x46,
 0x71, 0xF0, 0x47, 0x71, 0xE7, 0x47, 0x71, 0xEF, 0x47, 0x71, 0xEF, 0x47,
 0x71, 0x6F, 0x46, 0x71, 0x70, 0x46, 0xF1, 0xFF, 0x47, 0xF1, 0xFF, 0x47,
 0xF5, 0xFF, 0x57, 0x01, 0x00, 0x40, 0xFF, 0xFF, 0x7F };

#define caution_width 23
#define caution_height 27
const unsigned char caution_bits[] PROGMEM = {
 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x1C, 0x00, 0x00, 0x1C, 0x00,
 0x00, 0x36, 0x00, 0x00, 0x36, 0x00, 0x00, 0x63, 0x00, 0x00, 0x63, 0x00,
 0x00, 0x4B, 0x00, 0x80, 0xC9, 0x00, 0x80, 0xDD, 0x00, 0xC0, 0x9C, 0x01,
 0xC0, 0x9C, 0x01, 0x40, 0x1C, 0x01, 0x60, 0x08, 0x03, 0x60, 0x08, 0x03,
 0x30, 0x08, 0x06, 0x30, 0x08, 0x06, 0x18, 0x00, 0x0C, 0x18, 0x00, 0x0C,
 0x18, 0x18, 0x0C, 0x0C, 0x18, 0x18, 0x0C, 0x00, 0x18, 0xFE, 0xFF, 0x3F,
 0xFE, 0xFF, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

#endif //FDDEMUCONST_H
