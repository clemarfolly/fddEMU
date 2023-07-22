// Code from:
//  https://labprojectsbd.com/2020/05/14/how-to-interface-16x2-lcd-with-pic12f675-its-easy/

#include "Lcd1602_595.h"

unsigned char data_value;

void SIPO()
{
    LCD_LCK_LOW();
    xmit_spi(data_value);
    LCD_LCK_HIGH();
}

void LCD_init()
{
    LCD_LCK_Direction();
    init_spi();

    unsigned char t = 0x0A;
    data_value = 0x08;
    SIPO();
    while (t > 0x00)
    {
        Delay_ms(dly);
        t--;
    };

    data_value = 0x30;
    SIPO();

    data_value |= 0x08;
    SIPO();
    Delay_ms(dly);
    data_value &= 0xF7;
    SIPO();
    Delay_ms(dly);

    data_value = 0x30;
    SIPO();

    data_value |= 0x08;
    SIPO();
    Delay_ms(dly);
    data_value &= 0xF7;
    SIPO();
    Delay_ms(dly);

    data_value = 0x30;
    SIPO();

    data_value |= 0x08;
    SIPO();
    Delay_ms(dly);
    data_value &= 0xF7;
    SIPO();
    Delay_ms(dly);

    data_value = 0x20;
    SIPO();

    data_value |= 0x08;
    SIPO();
    Delay_ms(dly);
    data_value &= 0xF7;
    SIPO();
    Delay_ms(dly);

    LCD_command(_4_pin_interface | _2_row_display | _5x7_dots);
    LCD_command(display_on | cursor_off | blink_off);
    LCD_command(clear_display);
    LCD_command(cursor_direction_inc | display_no_shift);
}

void LCD_command(unsigned char value)
{
    data_value &= 0xFB;
    SIPO();
    LCD_4bit_send(value);
}

void LCD_send_data(unsigned char value)
{
    data_value |= 0x04;
    SIPO();
    LCD_4bit_send(value);
}

void LCD_4bit_send(unsigned char lcd_data)
{
    unsigned char temp = 0x00;

    temp = (lcd_data & 0xF0);
    data_value &= 0x0F;
    data_value |= temp;
    SIPO();

    data_value |= 0x08;
    SIPO();
    Delay_ms(dly);
    data_value &= 0xF7;
    SIPO();
    Delay_ms(dly);

    temp = (lcd_data & 0x0F);
    temp <<= 0x04;
    data_value &= 0x0F;
    data_value |= temp;
    SIPO();

    data_value |= 0x08;
    SIPO();
    Delay_ms(dly);
    data_value &= 0xF7;
    SIPO();
    Delay_ms(dly);
}

void LCD_putstr(const char *lcd_string)
{
    uint8_t pos = 0;
    while (lcd_string[pos] != 0)
    {
        LCD_send_data(lcd_string[pos]);
        pos++;
    }
}

void LCD_putstr_P(const char *lcd_string)
{
    for (size_t i = 0; i < strlen_P(lcd_string); i++)
    {
        LCD_send_data(pgm_read_byte_near(lcd_string + i));
    }
}

void LCD_putchar(char char_data)
{
    LCD_send_data(char_data);
}

void LCD_clear_home()
{
    LCD_command(clear_display);
    LCD_command(goto_home);
}

void LCD_goto(unsigned char y_pos, unsigned char x_pos)
{
    if ((y_pos - 1) == 0)
    {
        LCD_command(0x80 | (x_pos - 1));
    }
    else
    {
        LCD_command(0x80 | 0x40 | (x_pos - 1));
    }
}
