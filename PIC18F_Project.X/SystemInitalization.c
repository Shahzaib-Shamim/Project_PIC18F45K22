// --Includes-------------------------------------------------------------------
#include "SystemInitalization.h"
#include "GPIO.h"
#include "UART.h"
#include "Interrupt.h"
#include "ECCP.h"
#include "TMR.h"
#include "I2C.h"
#include "EXTI.h"
#include "TMR0.h"

void SYSTEM_Initialization(void)
{
    OSCILLATOR_Init();
    PIN_MANAGER_Initialize();
    uart1_init();
    INTERRUPT_Initialize();
    ECCP1_Initialize();
    I2C1_initialize();
//    EXTI0_Initialize();
    EXTI1_Initialize();
    EXTI2_Initialize();
    IOC_Initialize();
    TMR0_Initialize();
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
