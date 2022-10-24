/* 
 * File:   GPIO.h
 * Author: Shahzaib
 *
 * Created on August 17, 2022, 10:26 PM
 */

#ifndef GPIO_H
#define	GPIO_H

// --Includes-------------------------------------------------------------------
#include <xc.h>
#include "main.h"

// Macros Declaration-----------------------------------------------------------
#define INPUT   1
#define OUTPUT  0

#define HIGH    1
#define LOW     0

#define ANALOG      1
#define DIGITAL     0

#define PULL_UP_ENABLED      0
#define PULL_UP_DISABLED     1

#define ENABLE    1
#define DISABLE   0

// --EUSART1 Pins (RC6 = TX1, RC7 = RX1)----------------------------------------
// get/set RC6 procedures
#define RC6_SetHigh()            do { LATCbits.LATC6 = 1; } while(0)
#define RC6_SetLow()             do { LATCbits.LATC6 = 0; } while(0)
#define RC6_Toggle()             do { LATCbits.LATC6 = ~LATCbits.LATC6; } while(0)
#define RC6_GetValue()              PORTCbits.RC6
#define RC6_SetDigitalInput()    do { TRISCbits.TRISC6 = 1; } while(0)
#define RC6_SetDigitalOutput()   do { TRISCbits.TRISC6 = 0; } while(0)
#define RC6_SetAnalogMode()         do { ANSELCbits.ANSC6 = 1; } while(0)
#define RC6_SetDigitalMode()        do { ANSELCbits.ANSC6 = 0; } while(0)

// get/set RC7 procedures
#define RC7_SetHigh()            do { LATCbits.LATC7 = 1; } while(0)
#define RC7_SetLow()             do { LATCbits.LATC7 = 0; } while(0)
#define RC7_Toggle()             do { LATCbits.LATC7 = ~LATCbits.LATC7; } while(0)
#define RC7_GetValue()              PORTCbits.RC7
#define RC7_SetDigitalInput()    do { TRISCbits.TRISC7 = 1; } while(0)
#define RC7_SetDigitalOutput()   do { TRISCbits.TRISC7 = 0; } while(0)
#define RC7_SetAnalogMode()         do { ANSELCbits.ANSC7 = 1; } while(0)
#define RC7_SetDigitalMode()        do { ANSELCbits.ANSC7 = 0; } while(0)

// --I2C1 Pins (RC3 = SCL1, RC4 = SDA1)-----------------------------------------
// get/set RC3 procedures
#define RC3_SetHigh()            do { LATCbits.LATC3 = 1; } while(0)
#define RC3_SetLow()             do { LATCbits.LATC3 = 0; } while(0)
#define RC3_Toggle()             do { LATCbits.LATC3 = ~LATCbits.LATC3; } while(0)
#define RC3_GetValue()              PORTCbits.RC3
#define RC3_SetDigitalInput()    do { TRISCbits.TRISC3 = 1; } while(0)
#define RC3_SetDigitalOutput()   do { TRISCbits.TRISC3 = 0; } while(0)
#define RC3_SetAnalogMode()         do { ANSELCbits.ANSC3 = 1; } while(0)
#define RC3_SetDigitalMode()        do { ANSELCbits.ANSC3 = 0; } while(0)

// get/set RC4 procedures
#define RC4_SetHigh()            do { LATCbits.LATC4 = 1; } while(0)
#define RC4_SetLow()             do { LATCbits.LATC4 = 0; } while(0)
#define RC4_Toggle()             do { LATCbits.LATC4 = ~LATCbits.LATC4; } while(0)
#define RC4_GetValue()              PORTCbits.RC4
#define RC4_SetDigitalInput()    do { TRISCbits.TRISC4 = 1; } while(0)
#define RC4_SetDigitalOutput()   do { TRISCbits.TRISC4 = 0; } while(0)
#define RC4_SetAnalogMode()         do { ANSELCbits.ANSC4 = 1; } while(0)
#define RC4_SetDigitalMode()        do { ANSELCbits.ANSC4 = 0; } while(0)

// --Custom LED pin (RD2)-------------------------------------------------------
// get/set LED aliases
#define LED_TRIS                 TRISDbits.TRISD2
#define LED_LAT                  LATDbits.LATD2
#define LED_PORT                 PORTDbits.RD2
#define LED_ANS                  ANSELDbits.ANSD2
#define LED_SetHigh()            do { LATDbits.LATD2 = 1; } while(0)
#define LED_SetLow()             do { LATDbits.LATD2 = 0; } while(0)
#define LED_Toggle()             do { LATDbits.LATD2 = ~LATDbits.LATD2; } while(0)
#define LED_GetValue()           PORTDbits.RD2
#define LED_SetDigitalInput()    do { TRISDbits.TRISD2 = 1; } while(0)
#define LED_SetDigitalOutput()   do { TRISDbits.TRISD2 = 0; } while(0)
#define LED_SetAnalogMode()      do { ANSELDbits.ANSD2 = 1; } while(0)
#define LED_SetDigitalMode()     do { ANSELDbits.ANSD2 = 0; } while(0)

// --EXTI Pins (INT0 = RB0, INT1 = RB1, INT2 = RB2)-----------------------------
// get/set RB0 procedures
#define RB0_SetHigh()            do { LATBbits.LATB0 = 1; } while(0)
#define RB0_SetLow()             do { LATBbits.LATB0 = 0; } while(0)
#define RB0_Toggle()             do { LATBbits.LATB0 = ~LATBbits.LATB0; } while(0)
#define RB0_GetValue()              PORTBbits.RB0
#define RB0_SetDigitalInput()    do { TRISBbits.TRISB0 = 1; } while(0)
#define RB0_SetDigitalOutput()   do { TRISBbits.TRISB0 = 0; } while(0)
#define RB0_SetPullup()          do { WPUBbits.WPUB0 = 1; } while(0)
#define RB0_ResetPullup()        do { WPUBbits.WPUB0 = 0; } while(0)
#define RB0_SetAnalogMode()      do { ANSELBbits.ANSB0 = 1; } while(0)
#define RB0_SetDigitalMode()     do { ANSELBbits.ANSB0 = 0; } while(0)

// get/set RB1 procedures
#define RB1_SetHigh()            do { LATBbits.LATB1 = 1; } while(0)
#define RB1_SetLow()             do { LATBbits.LATB1 = 0; } while(0)
#define RB1_Toggle()             do { LATBbits.LATB1 = ~LATBbits.LATB1; } while(0)
#define RB1_GetValue()              PORTBbits.RB1
#define RB1_SetDigitalInput()    do { TRISBbits.TRISB1 = 1; } while(0)
#define RB1_SetDigitalOutput()   do { TRISBbits.TRISB1 = 0; } while(0)
#define RB1_SetPullup()          do { WPUBbits.WPUB1 = 1; } while(0)
#define RB1_ResetPullup()        do { WPUBbits.WPUB1 = 0; } while(0)
#define RB1_SetAnalogMode()      do { ANSELBbits.ANSB1 = 1; } while(0)
#define RB1_SetDigitalMode()     do { ANSELBbits.ANSB1 = 0; } while(0)

// get/set RB2 procedures
#define RB2_SetHigh()            do { LATBbits.LATB2 = 1; } while(0)
#define RB2_SetLow()             do { LATBbits.LATB2 = 0; } while(0)
#define RB2_Toggle()             do { LATBbits.LATB2 = ~LATBbits.LATB2; } while(0)
#define RB2_GetValue()              PORTBbits.RB2
#define RB2_SetDigitalInput()    do { TRISBbits.TRISB2 = 1; } while(0)
#define RB2_SetDigitalOutput()   do { TRISBbits.TRISB2 = 0; } while(0)
#define RB2_SetPullup()          do { WPUBbits.WPUB2 = 1; } while(0)
#define RB2_ResetPullup()        do { WPUBbits.WPUB2 = 0; } while(0)
#define RB2_SetAnalogMode()      do { ANSELBbits.ANSB2 = 1; } while(0)
#define RB2_SetDigitalMode()     do { ANSELBbits.ANSB2 = 0; } while(0)

// --Global functions-----------------------------------------------------------
/**
   @Param
    none
   @Returns
    none
   @Description
    GPIO and peripheral I/O initialization
   @Example
    PIN_MANAGER_Initialize();
 */
void PIN_MANAGER_Initialize(void);

/**
 * @Param
    none
 * @Returns
    none
 * @Description
    Interrupt on Change Handling routine
 * @Example
    PIN_MANAGER_IOC();
 */
void PIN_MANAGER_IOC(void);


#endif	/* GPIO_H */

