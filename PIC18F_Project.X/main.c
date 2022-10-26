// --Includes-------------------------------------------------------------------
#include <xc.h>
#include "main.h"
#include "DeviceConfiguration.h"
#include "SystemInitalization.h"
#include "GPIO.h"
#include "UART.h"
//#include "Interrupt.h"
#include "ECCP.h"
#include "TMR.h"
//#include "I2C.h"
//#include "ADS1115.h"
#include "TaskRoutines.h"
#include "LCD_HD44780_20x4.h"
#include "EEPROM.h"
//#include "DS3231.h"
//#include "EEPROM.h"
//#include "EXTI.h"
//#include "TMR0.h"
//#include "Capture.h"

// --Global variable section----------------------------------------------------
char uartBuff[10] = {0};
uint8_t intData[4];

void main(void)
{
    SYSTEM_Initialization();

    // Initialize system variables
    initSysVariables();
    
    // Start boot sequence
    startInitialization();

    __delay_ms(2000);
    uart1_puts("\nPIC18F Project\n\r\0");

    LED_SetDigitalOutput();
    LED_SetHigh();
    LCD_Init();
    __delay_ms(200);
    LED_SetLow();

    // <editor-fold defaultstate="collapsed" desc="LCD Startup">
    LCDclear();
    LCDsetCursor(0, 0);
    LCD_Write_Str("--------------------");
    LCDsetCursor(6, 1);
    LCD_Write_Str("Project");
    LCDsetCursor(2, 2);
    LCD_Write_Str("Firmware V");
    intData[0] = EEPROM_read1ByteFromPage(EEPROM_ADDR, 0, 0);
    itoa(intData[0] / 100, uartBuff, 10);
    LCD_Write_Str(uartBuff);
    LCD_Write_Str(".");
    itoa(intData[0] % 100, uartBuff, 10);
    LCD_Write_Str(uartBuff);
    LCDsetCursor(0, 3);
    LCD_Write_Str("--------------------");
    __delay_ms(1000);
    lcdStates = LCD_IDLE;
    // </editor-fold>
    
    // Timer with CCP config
    TMR1_StartTimer();
    ECCP1_SetTMR1CompareCount(1000); // 1000 usec or 1 ms
    
    while (true)
    {
        runApplication();
        
    }// end while(true)

}// end main()

