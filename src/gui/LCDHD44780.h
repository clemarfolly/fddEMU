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

#ifndef LCDHD44780_H
#define LCDHD44780_H

#include "Display.h"
#include "DisplayConf.h"
#include "FloppyDrive.h" //BIT_DRIVE0 & BIT_DRIVE1

#define LCD_NONE_SCREEN 0
#define LCD_STATUS_SCREEN 1
#define LCD_LOADING_SCREEN 2
#define LCD_BUSY_SCREEN 3
#define LCD_NOTICE_SCREEN 4
#define LCD_SPLASH_SCREEN 5
#define LCD_DRAWMENU_SCREEN 6

#define STATUS_SCREEN_COUNT 150
#define MENU_SCREEN_COUNT 150

class LCDHD44780 : public Display
{
  protected:
    void sleepOn() override;
    void sleepOff() override;
    void splashScreen() override;
    void loadingScreen() override;
    void busyScreen() override;
    void noticeScreen() override;
    void statusScreen() override;
    void drawMenu(void) override;

  private:
    uint8_t lastScreen;
    uint8_t statusScreenCount;
    uint8_t menuScreenCount;

  public:
    LCDHD44780();
    ~LCDHD44780();
    void init() override;
    void directWrite(const char *) override;
};

// #if ENABLE_GUI && ENABLE_LCD1602
// extern class LCDHD44780 disp;
// #endif

#endif // LCDHD44780
