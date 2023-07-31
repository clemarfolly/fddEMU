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

#include "ADCButton.h"
#include "ezAnalogKeypad.h"
#include "fddEMU.h"
#include <avr/io.h>
#include <util/delay.h>

#if ENABLE_SERIAL
#include "../serial/simpleUART.h" //debug
#endif                            // ENABLE_SERIAL
#if ENABLE_GUI
class ADCButton abtn;
#endif // ENABLE_GUI

ezAnalogKeypad buttonArray(BUTTON_CHANNEL);

ADCButton::ADCButton()
{
    init();
}

void ADCButton::init()
{
    buttonArray.setNoPressValue(1023); // analog value when no button is pressed
    buttonArray.registerKey(1, 50);    // analog value when the button 1 is pressed
    buttonArray.registerKey(2, 200);   // analog value when the button 2 is pressed
    buttonArray.registerKey(3, 400);   // analog value when the button 3 is pressed
    buttonArray.registerKey(4, 600);   // analog value when the button 4 is pressed
    buttonArray.registerKey(5, 800);   // analog value when the button 5 is pressed
}

int8_t ADCButton::read()
{
    if (!initialized)
    {
        init();
        initialized = 1;
    }

    uint8_t newval = 7;

    if (adcBusy)
    {
        return 0;
    }

    newval = buttonArray.getKey();

    return newval;
}
