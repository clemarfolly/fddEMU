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

#include "LCDHD44780.h"
#include "ADCButton.h"
#include "DiskFile.h"
#include "FloppyDrive.h"
#include "Lcd1602_595.h"
#include "fddEMU.h"

void LCDHD44780::sleepOn()
{
}

void LCDHD44780::sleepOff()
{
}

void LCDHD44780::drawMenu(void)
{
    if ((lastScreen != LCD_DRAWMENU_SCREEN) || (menuScreenCount == 0))
    {
        lastScreen = LCD_DRAWMENU_SCREEN;
        menuScreenCount = MENU_SCREEN_COUNT;

    Serial.print(F("LCDHD44780::drawMenu\n"));
    LCD_clear_home();

    for (uint8_t i = 0; i < menu_max; i++)
    {
        LCD_goto(i + 1, 1);
        LCD_putchar((i == menu_sel) ? '>' : ' ');
        LCD_putstr(menuFileNames[i]);
    }
    }
    else
    {
        menuScreenCount++;
    }
}

void LCDHD44780::statusScreen()
{
    if ((lastScreen != LCD_STATUS_SCREEN) || (statusScreenCount == 0))
    {
        lastScreen = LCD_STATUS_SCREEN;
        statusScreenCount = STATUS_SCREEN_COUNT;

        Serial.print(F("LCDHD44780::statusScreen()\n"));

        LCD_clear_home();

#if ENABLE_DRIVE_B == 0

        // Disk0 info
        LCD_goto(1, 1);
        LCD_putstr(&drive[0].fName[0]);
        LCD_goto(2, 1);
        LCD_putstr(drive[0].diskInfoStr());

#else

        // Disk0 info
        LCD_goto(1, 1);
        LCD_putstr("A:");
        LCD_putstr(&drive[0].fName[0]);

        // Draw drive1
        LCD_goto(2, 1);
        LCD_putstr("B:");
        LCD_putstr(&drive[1].fName[0]);

#endif
    }
    else
    {
        statusScreenCount--;
    }
}

void LCDHD44780::loadingScreen()
{
    LCD_clear_home();
    Serial.print(F("LCDHD44780::loadingScreen()\n"));
    LCD_goto(1, 1);
    LCD_putstr(&menuFileNames[menu_sel][0]);
    LCD_goto(2, 1);
    LCD_putstr_P(str_loading);
}

void LCDHD44780::busyScreen()
{
    Serial.print(F("LCDHD44780::busyScreen()\n"));
    LCD_goto(1, 1);
    LCD_putstr(&drive[getSelectedDrive() - 1].fName[0]);
    LCD_goto(2, 1);
    LCD_putstr_P(str_busy);
}

void LCDHD44780::noticeScreen()
{
    Serial.print(F("LCDHD44780::noticeScreen()\n"));
    LCD_goto(1, 1);
    LCD_putstr_P(notice_header);
    LCD_goto(2, 1);
    LCD_putstr(notice_message);
}

void LCDHD44780::splashScreen()
{
    if (lastScreen != LCD_SPLASH_SCREEN)
    {
        lastScreen = LCD_SPLASH_SCREEN;

        Serial.print(F("LCDHD44780::splashScreen()\n"));
        LCD_goto(1, 1);
        LCD_putstr_P(str_fddEMU);
        LCD_putstr(" ");
        LCD_putstr_P(str_2021);
        LCD_goto(2, 1);
        LCD_putstr_P(str_acemi);
        LCD_putstr(" ");
        LCD_putstr_P(str_elektron);
    }
}

void LCDHD44780::init()
{
    LCD_init();
}

LCDHD44780::LCDHD44780()
{
    lastScreen = LCD_NONE_SCREEN;
}

LCDHD44780::~LCDHD44780()
{
}
