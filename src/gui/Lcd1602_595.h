#ifndef LCD1602_595_H
#define LCD1602_595_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "petitfs/pffArduino.h"
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#define Delay_ms(a) _delay_ms(a)

#define clear_display 0x01
#define goto_home 0x02

#define cursor_direction_inc (0x04 | 0x02)
#define cursor_direction_dec (0x04 | 0x00)
#define display_shift (0x04 | 0x01)
#define display_no_shift (0x04 | 0x00)

#define display_on (0x08 | 0x04)
#define display_off (0x08 | 0x02)
#define cursor_on (0x08 | 0x02)
#define cursor_off (0x08 | 0x00)
#define blink_on (0x08 | 0x01)
#define blink_off (0x08 | 0x00)

#define _8_pin_interface (0x20 | 0x10)
#define _4_pin_interface (0x20 | 0x00)
#define _2_row_display (0x20 | 0x08)
#define _1_row_display (0x20 | 0x00)
#define _5x10_dots (0x20 | 0x40)
#define _5x7_dots (0x20 | 0x00)

#define dly 1

#define LCD_LCK_LOW() PORTC &= ~_BV(4)
#define LCD_LCK_HIGH() PORTC |= _BV(4)
#define LCD_LCK_Direction() DDRC |= _BV(4)

    void SIPO();
    void LCD_init();
    void LCD_command(unsigned char value);
    void LCD_send_data(unsigned char value);
    void LCD_4bit_send(unsigned char lcd_data);
    void LCD_putstr(const char *lcd_string);
    void LCD_putstr_P(const char *lcd_string);
    void LCD_putchar(char char_data);
    void LCD_clear_home();
    void LCD_goto(unsigned char x_pos, unsigned char y_pos);

#ifdef __cplusplus
}
#endif

#endif