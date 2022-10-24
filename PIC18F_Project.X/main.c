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
    
    
//    uint32_t time_ccp = 0;
//    CCP2_Initialize();
    
    // Timer with CCP config
    TMR1_StartTimer();
    ECCP1_SetTMR1CompareCount(1000); // 1000 usec or 1 ms

    while (true)
    {
        runApplication();

        /*if(tmr0Flag == true)
        {
            tmr0Flag = false;
            // INT0 RB0 button
            if(PORTBbits.RB0 == PRESSED)
            {
                static uint8_t pressCount = 0;
                pressCount++;
                itoa(pressCount, print_buffer, 10);
                uart1_puts("RB0 pressed debounced ");
                uart1_puts(print_buffer);
                uart1_newline();
            }
            // INT1 RB1 button
            if(PORTBbits.RB1 == PRESSED)
            {
                static uint8_t pressCount = 0;
                pressCount++;
                itoa(pressCount, print_buffer, 10);
                uart1_puts("debounced ");
                uart1_puts(print_buffer);
                uart1_newline();
                uart1_newline();
            }
            // INT2 RB2 button
            if(PORTBbits.RB2 == PRESSED)
            {
                static uint8_t pressCount = 0;
                pressCount++;
                itoa(pressCount, print_buffer, 10);
                uart1_puts("RB2 pressed debounced ");
                uart1_puts(print_buffer);
                uart1_newline();
            } 
        }*/
        
        /*if(ccp2Flag == true)
        {
            ccp2Flag = false;
            
            time_ccp = time_after_capture(cap_overflow_tmr3, &capture_value);
            sprintf(print_buffer, "\rtime press %10lu ms", time_ccp / 1000);
            uart1_puts(print_buffer);
            
            // added
            cap_overflow_tmr3 = 0;
            capture_value.ccpL = 0;
            capture_value.ccpH = 0;
            
        }*/
        
    }// end while(true)

}// end main()

