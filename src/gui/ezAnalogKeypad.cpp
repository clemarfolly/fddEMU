/*
 * Copyright (c) 2019, ArduinoGetStarted.com. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 *
 * - Neither the name of the ArduinoGetStarted.com nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ARDUINOGETSTARTED.COM "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ARDUINOGETSTARTED.COM BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "fddEMU.h"
#include <ezAnalogKeypad.h>
#if ENABLE_SERIAL
#include "../serial/simpleUART.h" //debug
#endif                            // ENABLE_SERIAL

// millis do not exists here, so this function was implemented.
bool ezAnalogKeypad::debounce()
{
    debounceCount--;

    if (debounceCount == 0)
    {
        debounceCount = debounceTime;
        return true;
    }

    return false;
}

void ezAnalogKeypad::initAdc()
{
#if defined(__AVR_ATmega328P__)
    PRR &= ~(1 << PRADC); // clear PRADC bit in Power Reduction Register
#elif defined(__AVR_ATmega32U4__)
    PRR0 &= ~(1 << PRADC); // clear PRADC bit in Power Reduction Register
    DDRF &= ~(1 << 5);     // set pin A2 (PF5-ADC5) as INPUT for ADC_BUTTON
#endif
    ADMUX = (1 << REFS0);                                              // select AVCC as reference
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // enable and prescale = 128 (16MHz/128 = 125kHz)
}

void ezAnalogKeypad::requestADC(uint8_t channel)
{
    ADMUX = ((channel & 0x0f) | (1 << REFS0)); // Select channel and voltage reference, if ADLAR is set result is left
                                               // justified ADCH register is sufficient
    ADCSRA |= (1 << ADSC);                     // Start the AD conversion
                                               // sample-and-hold takes 13.5 ADC clock cycles on first conversion
                                               // and 1.5 ADC clock cycles on subsequent converisons
    // ADC prescaler is set to clock/128, clock=16mhz so "1.5ADC cycle" = "192 cpu cycle" = 12uS
    // First conversion "13.5 ADC clock cycle" = 108us
    //_delay_us(12);
    // First conversion takes 25 ADC cycles and subsequent conversions takes 13 ADC clock cycles
}

uint16_t ezAnalogKeypad::analogRead(int pin)
{
    uint16_t result = 0;
    uint8_t low, high;
    low = ADCL;
    high = ADCH;
    result = (high << 8) | low;

    requestADC(keypadPin);

    return result;
}

ezAnalogKeypad::ezAnalogKeypad(int pin)
{
    keypadPin = pin;
    debounceTime = 500; // default is 500 passes

    keyNum = 0;
    lastKeyId = -1;

    for (int i = 0; i <= ezAnalogKeypad_MAX_KEY; i++)
    {
        keys[i] = 0;
        values[i] = 0;
    }

    initAdc();
    requestADC(keypadPin);
}

void ezAnalogKeypad::setDebounceTime(uint16_t time)
{
    debounceTime = time;
}

void ezAnalogKeypad::setNoPressValue(int analogValue)
{
    registerKey(6, analogValue); // 6 is key for non-press
}

void ezAnalogKeypad::registerKey(unsigned char key, int analogValue)
{
    if (keyNum <= ezAnalogKeypad_MAX_KEY)
    {
        keys[keyNum] = key;
        values[keyNum] = analogValue;
        keyNum++;

        // sorting the values array in ascending order
        for (int i = 0; i < (keyNum - 1); i++)
        {
            for (int j = i + 1; j < keyNum; j++)
            {
                if (values[i] > values[j])
                {
                    int swap_value = values[i];
                    values[i] = values[j];
                    values[j] = swap_value;

                    unsigned char swap_key = keys[i];
                    keys[i] = keys[j];
                    keys[j] = swap_key;
                }
            }
        }
    }
#if ENABLE_SERIAL
    else
    {
        Serial.print(F("Exceed the limit on number of keys"));
    }
#endif
}

unsigned char ezAnalogKeypad::getKey()
{
    uint16_t analogValue = analogRead(keypadPin);

    int keyId = -1;

    int lower_bound;
    int upper_bound;
    for (int i = 0; i < keyNum; i++)
    {
        if (i == 0)
            lower_bound = values[i] - 50; // 50 is tolerance
        else
            lower_bound = (values[i] + values[i - 1]) / 2;

        if (i == (keyNum - 1))
            upper_bound = values[i] + 50; // 50 is tolerance
        else
            upper_bound = (values[i] + values[i + 1]) / 2;

        if (analogValue >= lower_bound && analogValue < upper_bound)
        {
            keyId = i;
            break;
        }
    }

    if (keyId != -1 && keyId != lastKeyId)
    {
        if (debounce())
        {
            lastKeyId = keyId;
            return keys[keyId];
        }
    }

    return 0;
}
