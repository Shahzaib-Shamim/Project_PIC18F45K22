// feature/testing_serial


// --Includes-------------------------------------------------------------------
#include <xc.h>
#include <stdlib.h>
#include "main.h"
#include "DeviceConfiguration.h"
#include "SystemInitalization.h"
#include "GPIO.h"
#include "UART.h"
#include "Interrupt.h"
#include "ECCP.h"
#include "TMR.h"
#include "I2C.h"
#include "ADS1115.h"
#include "TaskRoutines.h"
#include "LCD_HD44780_20x4.h"
#include "DS3231.h"
#include "EEPROM.h"
#include "UART.h"

// --Global variable section----------------------------------------------------
char print_buffer[256] = {0}; //uint8_t

//void float2Bytes(uint8_t bytes_temp[4], float float_variable)
//{
//    memcpy(bytes_temp, (uint8_t*) (&float_variable), 4);
//}

volatile uint8_t rxInputBuff[20];
uint8_t data[4];

void main(void)
{
    SYSTEM_Initialization();

    // Initialize system variables
    initSysVariables();

    LED_SetDigitalOutput();
    LED_SetLow();
    LED_SetHigh();
    __delay_ms(500);
    LED_SetLow();


    // Timer with CCP config
    TMR1_StartTimer();
    ECCP1_SetTMR1CompareCount(1000); // 1000 usec or 1 ms

    __delay_ms(1000);
//    EUSART1_send_string((uint8_t*) "\nPIC18F Project\n\r\0");
    uart1_puts("\nPIC18F Project\n\r\0");

    // RTC time and date set
    //    DS3231_setTime(0, 13, 22);        // seconds, minutes, hours
    //    DS3231_setDate(7, 10, 9, 22);      // day of week, day of month, month, year

    /*LED_SetHigh();
    LCD_Init();
    __delay_ms(200);
    LED_SetLow();

    LCDclear();
    LCDsetCursor(0, 0);
    LCD_Write_Str("Some text1");
    LCDsetCursor(0, 1);
    LCD_Write_Str("Some text2");
    LCDsetCursor(0, 2);
    LCD_Write_Str("Some text3");
    LCDsetCursor(0, 3);
    LCD_Write_Str("Some text4");
    __delay_ms(1000);
    LCDclear();*/

    uint8_t readBuff[4];
    uint8_t writeBuff[4];
    float _floatWrite = 3.141f;
    float _floatRead;

    uint8_t dataBuff[4];

    // writing float data to eeprom (page 0, offset 0 -> from 0x0000h to 0x0003h)
    //    float2Bytes(writeBuff, _floatWrite);

    //    EEPROM_floatToByte(_floatWrite, writeBuff);
    //    EEPROM_write1ByteToPage(EEPROM_ADDR, 0, 0, writeBuff[0]);
    //    __delay_ms(5);
    //    EEPROM_write1ByteToPage(EEPROM_ADDR, 0, 1, writeBuff[1]);
    //    __delay_ms(5);
    //    EEPROM_write1ByteToPage(EEPROM_ADDR, 0, 2, writeBuff[2]);
    //    __delay_ms(5);
    //    EEPROM_write1ByteToPage(EEPROM_ADDR, 0, 3, writeBuff[3]);
    //    __delay_ms(5);
    //    EEPROM_writeMultipleBytes(EEPROM_ADDR, 0, 0, writeBuff, 4);
    //    __delay_ms(10);

    while (true)
    {
        runApplication();

        // reading float data from eeprom
        //        readBuff[0] = EEPROM_read1ByteFromPage(EEPROM_ADDR, 0, 0);
        //        readBuff[1] = EEPROM_read1ByteFromPage(EEPROM_ADDR, 0, 1);
        //        readBuff[2] = EEPROM_read1ByteFromPage(EEPROM_ADDR, 0, 2);
        //        readBuff[3] = EEPROM_read1ByteFromPage(EEPROM_ADDR, 0, 3);
        //        EEPROM_readMultipleBytes(EEPROM_ADDR, 0, 0, readBuff, 4);
        //        _floatRead = EEPROM_byteToFloat(readBuff);
        //
        //        sprintf((char *) print_buffer, "%.3f %u %u %u %u\n\r",
        //                _floatRead, readBuff[0], readBuff[1], readBuff[2], readBuff[3]);
        //        EUSART1_send_string((uint8_t *) print_buffer);

        //        if (calibrationRxFlag == true)
        //        {
        //            calibrationRxFlag = false;
        //
        //            DS3231_setTime(second, minute, hour);
        //            DS3231_setDate(day, date, month, year);
        //        }



        /*if (setTimeFlag == true)
        {
            calibrationRxFlag = false;

            setTimeFlag = false;
            DS3231_setTime(second, minute, hour);
        }
        if (setDateFlag == true)
        {
            setDateFlag = false;
            DS3231_setDate(day, date, month, year);
        }
        if (setTemp1Flag == true)
        {
            setTemp1Flag = false;
            EUSART1_send_string("Temp1\n\0");
        }
        if (setTemp2Flag == true)
        {
            setTemp2Flag = false;
            EUSART1_send_string("Temp2\n\0");
        }
        if (setPres1Flag == true)
        {
            setPres1Flag = false;
            EUSART1_send_string("Pres1\n\0");
        }
        if (setPres2Flag == true)
        {
            setPres2Flag = false;
            EUSART1_send_string("Pres2\n\0");
        }*/


        
        
        
    }// end while(true)

}// end main()

