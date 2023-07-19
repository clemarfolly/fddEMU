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

#ifndef GRAPHIC_UI_H
#define GRAPHIC_UI_H

#include "DisplayConf.h"
#include "Display.h"
#include "FloppyDrive.h" //BIT_DRIVE0 & BIT_DRIVE1
#include "u8g.h"

class U8G : public Display
{ // Encapsulate some u8g functions
  protected:
    u8g_t u8g;

  public:
    void drawStrP(int x, int y, const char *str)
    {
        u8g_DrawStrP(&u8g, x, y, (const u8g_pgm_uint8_t *)str);
    }
    void drawStr(int x, int y, char *str)
    {
        u8g_DrawStr(&u8g, x, y, (const char *)str);
    }
    void drawXBMP(u8g_uint_t x, u8g_uint_t y, u8g_uint_t w, u8g_uint_t h, const u8g_pgm_uint8_t *bitmap)
    {
        u8g_DrawXBMP(&u8g, x, y, w, h, bitmap);
    }
    u8g_uint_t getStrWidthP(const char *str)
    {
        return u8g_GetStrWidthP(&u8g, (const u8g_pgm_uint8_t *)str);
    }
    u8g_uint_t getStrWidth(char *str)
    {
        return u8g_GetStrWidth(&u8g, (const char *)str);
    }
    void sleepOn() override
    {
        u8g_SleepOn(&u8g);
    }
    void sleepOff() override
    {
        u8g_SleepOff(&u8g);
    }
};

class GraphicUI : public U8G
{
  private:
    void drawPage();
    void init() override;
    void splashScreen() override;
    void loadingScreen() override;
    void busyScreen() override;
    void noticeScreen() override;
    void statusScreen() override;
    void drawMenu(void) override;

  public:
    GraphicUI();
    ~GraphicUI();
};

extern class GraphicUI disp;

#endif // GRAPHIC_UI_H
