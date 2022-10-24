// --Includes-------------------------------------------------------------------
#include <xc.h>
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
#include "EXTI.h"
#include "TMR0.h"
#include "Capture.h"

// --Global variable section----------------------------------------------------
char uartBuff[20] = {0};
uint32_t time_ccp = 0;

void main(void)
{
    SYSTEM_Initialization();

    // Initialize system variables
    initSysVariables();
    
    // Start boot sequence
    startInitialization();

    __delay_ms(1000);
    uart1_puts("\nPIC18F Project\n\r\0");

    LED_SetDigitalOutput();
    LED_SetHigh();
    LCD_Init();
    __delay_ms(200);
    LED_SetLow();

    LCDclear();
    LCDsetCursor(0, 0);
    LCD_Write_Str("--------------------");
    LCDsetCursor(6, 1);
    LCD_Write_Str("Project");
    LCDsetCursor(2, 2);
    LCD_Write_Str("Firmware V0.10");
    LCDsetCursor(0, 3);
    LCD_Write_Str("--------------------");
    __delay_ms(1000);
    LCDclear();
    LCDsetCursor(0, 0);
    LCD_Write_Str("Waiting for pulse...");
    
    // Timer with CCP config
    TMR1_StartTimer();
    ECCP1_SetTMR1CompareCount(1000); // 1000 usec or 1 ms

    while (true)
    {
        runApplication();
        
        if(ccp2Flag == true)
        {
            ccp2Flag = false;
            
            time_ccp = time_after_capture(cap_overflow_tmr3, &capture_value);
            ltoa(time_ccp/1000, uartBuff, 10);
            uart1_puts("Pulsetime/1000=");
            uart1_puts(uartBuff);
            uart1_newline();
            
            ltoa(time_ccp, uartBuff, 10);
            uart1_puts("Pulsetime=");
            uart1_puts(uartBuff);
            uart1_newline();
            
            // added
            cap_overflow_tmr3 = 0;
            capture_value.ccpL = 0;
            capture_value.ccpH = 0;
            
        }
        
    }// end while(true)

}// end main()

