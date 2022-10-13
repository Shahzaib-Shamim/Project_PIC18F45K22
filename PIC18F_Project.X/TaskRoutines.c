#include "TaskRoutines.h"
#include "GPIO.h"
//#include "EUSART.h"
#include "UART.h"
#include "ADS1115.h"
#include "DS3231.h"
#include "EEPROM.h"
#include "LCD_HD44780_20x4.h"

volatile TIMER1_t timer1 = {
    .timer1_1msFlag = false,
    .timer1_1msCounter = 0
};

ADS1115_Object_t myADC;
char txBuffer[50] = {0}; //uint8_t
uint8_t readBuff[32];

volatile ApplicationFlags_t sysFlags;

bool ads1115Flag = false;
bool ds3231Flag = false;

void initSysVariables(void)
{
    sysFlags.ledBlinkTask = false;
    sysFlags.uartTxTask = false;
    sysFlags.ads1115SampleTask = false;
    sysFlags.ds3231Task = false;
    sysFlags.eepromTask = false;
    sysFlags.lcdTask = false;

    // ADS1115 init (disabled for testing)
    // ADS1115 configuration
    /*myADC.address = ADS1115_ADDR;
    myADC.config.value = 0; // reset
    myADC.config.mux = MUX_SINGLE_0;
    myADC.config.pga = PGA_FSR_6_144;
    myADC.config.mode = MODE_SINGLE_SHOT_OR_PWR_DOWN;
    myADC.config.dataRate = DR_128_SPS;
    myADC.config.compMode = COMP_MODE_TRADITIONAL;
    myADC.config.compPol = COMP_POL_ACT_LOW;
    myADC.config.compLat = COMP_LAT_NONLATCHING;
    myADC.config.compQue = COMP_QUEUE_DISABLE_COMPARATOR;
    ADS1115_init(&myADC);*/

    //    if(myADC.isPresent == 1)
    //    {
    //        sprintf((char *) txBuffer, "ADS1115 Found\r\n");
    //        EUSART1_send_string((uint8_t *) txBuffer);
    //    }
    //    if(myADC.isPresent == 0)
    //    {
    //        sprintf((char *) txBuffer, "ADS1115 Not found\r\n");
    //        EUSART1_send_string((uint8_t *) txBuffer);
    //    }
}

char str[20];
uint32_t counterIndex = 0;

void runApplication(void)
{
    if(sysFlags.ledBlinkTask == true)
    {
        sysFlags.ledBlinkTask = false;
        ledBlink();
    }

    if(sysFlags.uartTxTask == true)
    {
        static uint8_t counter = 0;
        sysFlags.uartTxTask = false;

        //        sprintf((char *) txBuffer, "\nCounter Value = %d\r\n", counter);
        //        EUSART1_send_string((uint8_t *) txBuffer);
        //        counter++;
//        ads1115SampleData();
//        ds3232Timestamp();
        
        // added
//        sprintf(txBuffer, "\nCounter Value = %d\r\n", counter);
//        uart_puts(txBuffer);
//        counter++;
    }

    if(sysFlags.ads1115SampleTask == true)
    {
        sysFlags.ads1115SampleTask = false;
        //        ads1115SampleData();
    }

    if(sysFlags.ds3231Task == true)
    {
        sysFlags.ds3231Task = false;
        //        ds3232Timestamp();
    }

    if(sysFlags.eepromTask == true)
    {
        sysFlags.eepromTask = false;

        // added
        //        EEPROM_readMultipleBytes(EEPROM_ADDR, 0, 0, readBuff, 32);
        //        sprintf((char *) txBuffer, "%u %u %u %u\n\r",
        //                readBuff[0], readBuff[1], readBuff[2], readBuff[3]);
        //        EUSART1_send_string((uint8_t *) txBuffer);
    }

    if(sysFlags.lcdTask == true)
    {
        sysFlags.lcdTask = false;

        /*LCDsetCursor(0, 0);
        LCD_Write_Str("Counter = ");
        LCDsetCursor(0, 1);
        sprintf(str, "%lu", counterIndex);
        counterIndex++;
        LCD_Write_Str(str);*/
    }
}

void ledBlink(void)
{
    LED_Toggle();
}

void ads1115SampleData(void)
{
    float voltage[4];
    static uint8_t count = 0;

    if(myADC.isPresent == 1)
    {

        for(uint8_t i = 0; i < 4; i++)
        {
            voltage[i] = ADS1115_readSingleEnded(&myADC, i);
            //            __delay_us(10);
        }

        //        sprintf((char *) txBuffer, "--Start-------------------------------\r\n");
        //        EUSART1_send_string((uint8_t *) txBuffer);
        //
        //        sprintf(txBuffer, "\nADC# %d,  %0.2f  %0.2f  %0.2f  %0.2f\r\n\n",
        //                count, voltage[0], voltage[1], voltage[2], voltage[3]);
        //        EUSART1_send_string(txBuffer);
        //        count++;
//        EUSART1_send_string("ADS1115\n\r\0");
    }
}

void ds3232Timestamp(void)
{
    char *Day = "";
    char *MonthName = "";
    uint8_t ds3231_readBuff[7];
    DS3231_getTime(&ds3231_readBuff[0], &ds3231_readBuff[1], &ds3231_readBuff[2]);
    DS3231_getDate(&ds3231_readBuff[3], &ds3231_readBuff[4], &ds3231_readBuff[5], &ds3231_readBuff[6]);
    Day = setDayName(ds3231_readBuff[3]);
    MonthName = setMonthName(ds3231_readBuff[5]);
    
//    sprintf(txBuffer, "\n%d:%d:%d %s %d-%s-%d\r\n\n", ds3231_readBuff[2], ds3231_readBuff[1], ds3231_readBuff[0], Day, ds3231_readBuff[4], MonthName, ds3231_readBuff[6]);
//    EUSART1_send_string(txBuffer);
    //    EUSART1_send_string("RTC\n\r\0");
}