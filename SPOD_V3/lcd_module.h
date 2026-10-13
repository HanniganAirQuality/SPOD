/*******************************************************************************
 * @file    lcd_module.h
 * @brief   Module to organize functions for ST7735 LCD display
 *
 * @cite    Adapted LCDDisplay.h / LCDDisplay.cpp (SPOD)
 *
 * @author  Alexander Hansen
 * @editor  Percy Smith, percy.smith@colorado.edu
 * @date    October 7, 2026
 *
 * @log     Restructured into lab module format (LCD_Module class)
 *          Removed PCD8544 support (ST7735 only)
 ******************************************************************************/
#ifndef _LCD_MODULE_H
#define _LCD_MODULE_H

#include <Arduino.h>
#include <stdint.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>

#include "SPOD_node.h"

/*! ST7735 driver forced to the verified slow SPI clock (library default is 32 MHz) */
class SlowST7735 : public Adafruit_ST7735
{
  public:
    SlowST7735(SPIClass *spi, int8_t cs, int8_t dc, int8_t rst)
      : Adafruit_ST7735(spi, cs, dc, rst) {}

    void begin(uint32_t frequency = 0) override
    {
      (void)frequency;
      Adafruit_ST77xx::begin(SPOD_ST7735_SPI_FREQUENCY);
    }
}; //class SlowST7735


#endif //_LCD_MODULE_H
