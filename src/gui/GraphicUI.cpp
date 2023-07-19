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

#include "GraphicUI.h"
#include "ADCButton.h"
#include "DiskFile.h"
#include "FloppyDrive.h"
#include "fddEMU.h"
#if ENABLE_GUI
class GraphicUI disp; // will use as extern
#endif                // ENABLE_GUI

/// https://github.com/olikraus/u8glib/blob/master/sys/arm/examples/menu/menu.c
void GraphicUI::drawMenu(void)
{
    uint8_t i, h;
    u8g_uint_t w, d;

    // u8g_SetFont(&u8g, u8g_font_6x10);
    u8g_SetFontRefHeightText(&u8g);
    // u8g_SetFontPosTop(&u8g);
    h = u8g_GetFontAscent(&u8g) - u8g_GetFontDescent(&u8g);
    w = u8g_GetWidth(&u8g);
    for (i = 0; i < menu_max; i++)
    {
        d = (w - getStrWidth(menuFileNames[i])) / 2;
        u8g_SetDefaultForegroundColor(&u8g);
        if (i == menu_sel)
        {
            u8g_DrawBox(&u8g, 0, i * h + 1, w, h);
            u8g_SetDefaultBackgroundColor(&u8g);
        }
        u8g_DrawStr(&u8g, d, i * h, menuFileNames[i]);
    }
    u8g_SetDefaultForegroundColor(&u8g); // set color back to foreground color
}

void GraphicUI::statusScreen()
{
#define X_OFS 1
#define Y_OFS_A 3
#define Y_OFS_B 33

    u8g_uint_t w = u8g_GetWidth(&u8g);
    // Draw drive0
    u8g_SetDefaultForegroundColor(&u8g); // set color back to foreground color
    if (isDriveA())
    {
        drawXBMP(X_OFS, Y_OFS_A, floppym_width, floppym_height, floppya_bits);
        u8g_DrawFrame(&u8g, X_OFS - 1, Y_OFS_A - 1, floppym_width + 2, floppym_height + 2);
        u8g_DrawBox(&u8g, X_OFS + floppym_width + 2, Y_OFS_A - 1, w - (X_OFS + floppym_width + 2), floppym_height + 2);
        u8g_SetDefaultBackgroundColor(&u8g); // set color black
    }
    else
        drawXBMP(X_OFS, Y_OFS_A, floppym_width, floppym_height, floppya_bits);
    // Disk0 info
    u8g_uint_t d = (w - floppym_width - getStrWidth(drive[0].fName)) / 2;
    drawStr(floppym_width + d, Y_OFS_A + 1, drive[0].fName);
    char *infostr = drive[0].diskInfoStr();
    d = (w - floppym_width - getStrWidth(infostr)) / 2;
    drawStr(floppym_width + d, Y_OFS_A + 14, infostr);
#if ENABLE_DRIVE_B
    // Draw drive1
    u8g_SetDefaultForegroundColor(&u8g); // set color back to foreground color
    if (isDriveB())
    {
        drawXBMP(X_OFS, Y_OFS_B, floppym_width, floppym_height, floppyb_bits);
        u8g_DrawFrame(&u8g, X_OFS - 1, Y_OFS_B - 1, floppym_width + 2, floppym_height + 2);
        u8g_DrawBox(&u8g, X_OFS + floppym_width + 2, Y_OFS_B - 1, w - (X_OFS + floppym_width + 2), floppym_height + 2);
        u8g_SetDefaultBackgroundColor(&u8g); // set color black
    }
    else
        drawXBMP(X_OFS, Y_OFS_B, floppym_width, floppym_height, floppyb_bits);
    // Disk1 info
    d = (w - floppym_width - getStrWidth(drive[1].fName)) / 2;
    drawStr(floppym_width + d, Y_OFS_B + 1, drive[1].fName);
    infostr = drive[1].diskInfoStr();
    d = (w - floppym_width - getStrWidth(infostr)) / 2;
    drawStr(floppym_width + d, Y_OFS_B + 14, infostr);
    u8g_SetDefaultForegroundColor(&u8g); // set color back to foreground color
#endif                                   // ENABLE_DRIVE_B
}

void GraphicUI::loadingScreen()
{
    u8g_uint_t w = u8g_GetWidth(&u8g);
    u8g_uint_t d = (w - getStrWidthP(str_loading)) / 2;

    if (isDriveA())
        drawXBMP((w - floppym_width) / 2, 0, floppym_width, floppym_height, floppya_bits);
    else if (isDriveB())
        drawXBMP((w - floppym_width) / 2, 0, floppym_width, floppym_height, floppyb_bits);

    drawStrP(d, floppym_height + 5, str_loading);
    d = (w - getStrWidth(menuFileNames[menu_sel])) / 2;
    drawStr(d, floppym_height + 20, menuFileNames[menu_sel]);
}

void GraphicUI::busyScreen()
{
    u8g_uint_t w = u8g_GetWidth(&u8g);
    u8g_uint_t d = (w - getStrWidthP(str_busy)) / 2;

    if (isDriveA())
        drawXBMP((w - floppym_width) / 2, 0, floppym_width, floppym_height, floppya_bits);
    else if (isDriveB())
        drawXBMP((w - floppym_width) / 2, 0, floppym_width, floppym_height, floppyb_bits);

    drawStrP(d, floppym_height + 5, str_busy);
    d = (w - getStrWidth(drive[getSelectedDrive() - 1].fName)) / 2;
    drawStr(d, floppym_height + 20, drive[getSelectedDrive() - 1].fName);
}

void GraphicUI::noticeScreen()
{
    u8g_uint_t w = u8g_GetWidth(&u8g);
    u8g_uint_t d = (w - getStrWidthP(notice_header)) / 2;

    drawXBMP((w - caution_width) / 2, 0, caution_width, caution_height, caution_bits);

    drawStrP(d, caution_height + 5, notice_header);
    d = (w - getStrWidthP(notice_message)) / 2;
    drawStrP(d, caution_height + 20, notice_message);
}

void GraphicUI::splashScreen()
{
    u8g_uint_t w = u8g_GetWidth(&u8g);
    u8g_uint_t d = (w - floppy_width - getStrWidthP(str_2021)) / 2;

    drawXBMP(1, 5, floppy_width, floppy_height, floppy_bits);
    drawStrP(floppy_width + d, 10, str_2021);
    d = (w - floppy_width - getStrWidthP(str_acemi)) / 2;
    drawStrP(floppy_width + d, 25, str_acemi);
    d = (w - floppy_width - getStrWidthP(str_elektron)) / 2;
    drawStrP(floppy_width + d, 40, str_elektron);
}

void GraphicUI::init() 
{
    u8g_SetPinInput(PN(2, 5));
    u8g_SetPinLevel(PN(2, 5), 1);
    u8g_SetPinOutput(PN(2, 5));
    u8g_SetPinInput(PN(2, 4));
    u8g_SetPinLevel(PN(2, 4), 1);
    u8g_SetPinOutput(PN(2, 4));
#if OLED_128X32
    u8g_InitI2C(&u8g, &u8g_dev_ssd1306_128x32_2x_i2c, U8G_I2C_OPT_NONE);
#else  // default
    u8g_InitI2C(&u8g, &u8g_dev_ssd1306_128x64_2x_i2c, U8G_I2C_OPT_NONE);
#endif // OLED_128X32
#if FLIP_SCREEN
    u8g_SetRot180(&u8g);
#endif                                // FLIP_SCREEN
    u8g_SetFont(&u8g, u8g_font_6x10); // select font
    u8g_SetFontPosTop(&u8g);          // set font position
}

void GraphicUI::drawPage()
{
    u8g_FirstPage(&u8g);
    do
    {
        Display::drawPage();
    } while (u8g_NextPage(&u8g));
}

GraphicUI::GraphicUI()
{
}

GraphicUI::~GraphicUI()
{
}

