/* 
 * File:   LCD_HD44780_20x4.h
 * Author: Shahzaib
 *
 * Created on September 10, 2022, 6:07 PM
 */

#ifndef LCD_HD44780_20X4_H
#define	LCD_HD44780_20X4_H

// --Includes-------------------------------------------------------------------
#include <xc.h>
#include "main.h"
#include "I2C.h"

// --Macro Defination-----------------------------------------------------------
#define LCD_CLEAR_DISPLAY           0x01    // Mode : Clears display
#define LCD_RETURN_HOME             0x02    // Mode : Returns cursor to home posn.

// Entry Mode Set
#define LCD_ENTRY_MODE_SET          0x04    // Mode : Entry Mode Set, Sets the cursor move dir and specs whether or not to shift the display
#define LCD_INCREMENT               0x02        // Sub Mode of ENTRY_MODE_SET : Increment DDRAM (I/D), Entry Left
#define LCD_DECREMENT               0x00        // Sub Mode of ENTRY_MODE_SET : Decrement DDRAM (I/D), Entry Right
#define LCD_SHIFT_ON                0x01        // Sub Mode of ENTRY_MODE_SET : Shift On  (S), Shift Display when byte written. Display Shift
#define LCD_SHIFT_OFF               0x00        // Sub Mode of ENTRY_MODE_SET : Shift Off (S), Don't shift display when byte written. Cursor Move

// Display Function
#define LCD_DISPLAY_ON_OFF          0x08    // Mode : Display On/Off, Sets on/off of all display, Cursor on/off, Cursor Blink on/off
#define LCD_DISPLAY_ON              0x04        // Sub Mode of DISPLAY_ON_OFF : Puts display on  (D)
#define LCD_DISPLAY_OFF             0x00        // Sub Mode of DISPLAY_ON_OFF : Puts display off (D)
#define LCD_CURSOR_ON               0x02        // Sub Mode of DISPLAY_ON_OFF : Puts cursor on   (C)
#define LCD_CURSOR_OFF              0x00        // Sub Mode of DISPLAY_ON_OFF : Puts cursor off  (C)
#define LCD_BLINKING_ON             0x01        // Sub Mode of DISPLAY_ON_OFF : Blinking cursor  (B)
#define LCD_BLINKING_OFF            0x00        // Sub Mode of DISPLAY_ON_OFF : Solid cursor     (B)

// Display Control
#define LCD_MV_CUR_SHIFT_DISPLAY    0x10    // Mode : Move the cursor and shifts the display
#define LCD_DISPLAY_SHIFT           0x08        // Sub Mode of CURSOR_SHFT_DIS : Display shifts after char print   (SC)
#define LCD_CURSOR_SHIFT            0x00        // Sub Mode of CURSOR_SHFT_DIS : Cursor shifts after char print    (SC)
#define LCD_SHIFT_RIGHT             0x04        // Sub Mode of CURSOR_SHFT_DIS : Cursor or Display shifts to right (RL)
#define LCD_SHIFT_LEFT              0x00        // Sub Mode of CURSOR_SHFT_DIS : Cursor or Display shifts to left  (RL)

// Function Set
#define LCD_FUNCTION_SET            0x20    // Mode : Set the type of interface that the display will use
#define LCD_INTF8BITS               0x10        // Sub Mode of FUNCTION_SET : Select 8 bit interface         (DL)
#define LCD_INTF4BITS               0x00        // Sub Mode of FUNCTION_SET : Select 4 bit interface         (DL)
#define LCD_TWO_LINES               0x08        // Sub Mode of FUNCTION_SET : Selects two char line display  (N)
#define LCD_ONE_LINE                0x00        // Sub Mode of FUNCTION_SET : Selects one char line display  (N)
#define LCD_FONT_5_10               0x04        // Sub Mode of FUNCTION_SET : Selects 5 x 10 Dot Matrix Font (F)
#define LCD_FONT_5_7                0x00        // Sub Mode of FUNCTION_SET : Selects 5 x 7 Dot Matrix Font  (F)

#define LCD_CG_RAM_ADDRESS          0x40        // Mode : Enables the setting of the Char Gen (CG) Ram Address, to be or'ed with require address
#define LCD_CG_RAM_ADDRESS_MASK     0b00111111  // Used to mask off the lower 6 bits of valid CG Ram Addresses

#define LCD_DD_RAM_ADDRESS          0x80        // Mode : Enables the setting of the Display Data (DD) Ram Address, to be or'ed with require address
#define LCD_DD_RAM_ADDRESS_MASK     0b01111111    // Used to mask off the lower 6 bits of valid DD Ram Addresses

//#define USE_BUSY_FLAG               // Define this if you wish to use busy flag polling on slow LCD activities

// Change here for your I2C to 16 pin parallel interface // TODO Adapt
#define Bl 0b00001000  // Backlight enable bit (On = 1, Off =0)
#define En 0b00000100  // Enable bit (Enable on low edge)
#define Rw 0b00000010  // Read/Write bit (Read = 1, Write = 0)
#define Rs 0b00000001  // Register select bit (Data = 1, Control = 0)

// Change here for your I2C to 16 pin parallel interface // TODO Adapt
#define LCD_INIT      ((0b00000000 | En) & ~Rs) & (~Rw) // Used to set all the O/Ps on the PCF8574 to initialise the LCD
#define LCD_8BIT_INIT 0b00110000 // Used to initialise the interface at the LCD
#define LCD_4BIT_INIT 0b00100000 // Used to initialise the interface at the LCD

#define LCD_PCF8574_ADDR         (0x27)  // Modify this if the default address is altered 
#define LCD_PCF8574_WEAK_PU      0b11110000 // Used to turn on PCF8574 Bits 7-4 on. To allow for read of LCD.

#define LCD_BUSY_FLAG_MASK       0b10000000 // Used to mask off the status of the busy flag
#define LCD_ADDRESS_COUNTER_MASK 0b01111111 // Used to mask off the value of the Address Counter
#define LCD_MAX_COLS             20
#define LCD_MAX_ROWS             4

//
// Code was written with the following assumptions as to PCF8574 -> Parallel 4bit convertor interconnections
// controlling a 20 by 4 LCD display. Assumes A0...A2 on PCF8574 are all pulled high. Giving address of 0x4E or 0b01001110 (0x27)
// (the last bit [b0] is I2C R/nW bit)
//
// Pin out for LCD display (16 pins)
// ---------------------------------
// 1  - Gnd
// 2  - Vcc
// 3  - VContrast
// 4  - RS - P0 - Pin 4 PCF8574
// 5  - RW - P1 - Pin 5 PCF8574
// 6  - En - P2 - Pin 6 PCF8574
// 7  - D0 - Don't Care
// 8  - D1 - Don't Care
// 9  - D2 - Don't Care
// 10 - D3 - Don't Care
// 11 - D4 - P4 - Pin 9  PCF8574
// 12 - D6 - P5 - Pin 10 PCF8574
// 13 - D6 - P6 - Pin 11 PCF8574
// 14 - D7 - P7 - Pin 12 PCF8574
// 15 - Anode   LED
// 16 - Cathode LED
//
// PCF8574 register and pin mapping
// Bit 0 - RS  - P0 - Pin 4  PCF8574
// Bit 1 - RW  - P1 - Pin 5  PCF8574
// Bit 2 - En  - P2 - Pin 6  PCF8574
// Bit 3 - Led - P3 - Pin 7  PCF8574 (Active High, Led turned on)
// Bit 4 - D4  - P4 - Pin 9  PCF8574
// Bit 5 - D5  - P5 - Pin 10 PCF8574
// Bit 6 - D6  - P6 - Pin 11 PCF8574
// Bit 7 - D7  - P7 - Pin 12 PCF8574
//



// The display is configured as follows:
//
// 1. Display clear
// 2. Function set:
//    DL = 1; 4-bit interface data
//    N = 0; 2-line display
//    F = 0; 5x7 dot character font
// 3. Display on/off control:
//    D = 1; Display on
//    C = 0; Cursor off
//    B = 0; Blinking off
// 4. Entry mode set:
//    I/D = 1; Increment by 1
//    S = 0; No shift
//

#define LCD_LINE1	0x00		// Constant used to point to start of LCD Line 1
#define LCD_LINE2	0x40		// Constant used to point to start of LCD Line 2
#define LCD_LINE3	0x14		// Constant used to point to start of LCD Line 3
#define LCD_LINE4	0x54		// Constant used to point to start of LCD Line 4

// DDRAM address:
// Display position
// 0  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18 19
// 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F 10 11 12 13
// 40 41 42 43 44 45 46 47 48 49 4A 4B 4C 4D 4E 4F 50 51 52 53
// 14 15 16 17 18 19 1A 1B 1C 1D 1E 1F 20 21 22 23 24 25 26 27
// 54 55 56 57 58 59 5A 5B 5C 5D 5E 5F 60 61 62 63 64 65 66 67

void LCD_Init(void);
void LCD_Write_Char(char message);
void LCD_Write_Str(const char *message);
void LCDclear(void);
void LCDhome(void);
void LCDdisplayOff(void);
void LCDdisplayOn(void);
void LCDblinkOff(void);
void LCDblinkOn(void);
void LCDcursorOff(void);
void LCDcursorOn(void);
void LCDscrollDisplayLeft(void);
void LCDscrollDisplayRight(void);
void LCDleftToRight(void);
void LCDrightToLeft(void);
void LCDnoBacklight(void);
void LCDbacklight(void);
void LCDautoscroll(void);
void LCDnoAutoscroll(void);
void LCDcreateChar(unsigned char location, unsigned char charmap[]);
void LCDsetCursor(unsigned char col, unsigned char row);

inline void LCDcommandWrite(unsigned char value);
inline unsigned char LCDcommandRead(void);
inline void LCDdataWrite(unsigned char value);
inline unsigned char LCDdataRead(void);
unsigned char LCDbusy(void);
unsigned char LCDaddressCounter(void);
unsigned char LCDreadDDRam(unsigned char address);
unsigned char LCDreadCGRam(unsigned char address);

// --LCD I2C functions----------------------------------------------------------
//void I2C_Write_Byte_Single_Reg(unsigned char device, unsigned char info);
//unsigned char I2C_Read_Byte_Single_Reg(unsigned char device);
void LCD_i2cWriteSingle(unsigned char device, unsigned char info);
unsigned char LCD_i2cRead(unsigned char device);

#endif	/* LCD_HD44780_20X4_H */

