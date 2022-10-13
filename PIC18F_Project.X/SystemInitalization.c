// --Includes-------------------------------------------------------------------
#include "SystemInitalization.h"
#include "GPIO.h"
//#include "EUSART.h"
#include "UART.h"
#include "Interrupt.h"
#include "ECCP.h"
#include "TMR.h"
#include "I2C.h"

void SYSTEM_Initialization(void)
{
    OSCILLATOR_Init();
    PIN_MANAGER_Initialize();
//    EUSART1_Initialize();
    uart_init();
    INTERRUPT_Initialize();
    ECCP1_Initialize();
    I2C1_initialize();
    TMR1_Initialize();
}

static void OSCILLATOR_Init(void)
{   
    // SCS FOSC; IRCF 1MHz_HFINTOSC/16; IDLEN disabled; 
    OSCCON = 0x30;
    // PRISD enabled; SOSCGO disabled; MFIOSEL disabled; 
    OSCCON2 = 0x04;
    // INTSRC disabled; PLLEN disabled; TUN 0; 
    OSCTUNE = 0x00;
    // Wait for PLL to stabilize
    while(PLLRDY == 0);
}
