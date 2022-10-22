#include "Interrupt.h"
#include "GPIO.h"
//#include "EUSART.h"
#include "UART.h"
#include "ECCP.h"
#include "I2C.h"
#include "TMR0.h"
#include "EXTI.h"
#include "Capture.h"

void INTERRUPT_Initialize(void)
{
    // Enable Interrupt Priority Vectors
    RCONbits.IPEN = 1;

    // Assign peripheral interrupt priority vectors
    
    // EXTI - high priority
    INTCON3bits.INT1IP = 1;
    INTCON3bits.INT2IP = 1;

    // TXI - low priority
    IPR1bits.TX1IP = 0;

    // RCI - low priority       // high
    IPR1bits.RC1IP = 0;         //1

    // CCPI - high priority
    IPR1bits.CCP1IP = 1;
    
    // TMR0 - low priority
    INTCON2bits.TMR0IP = 0;
    
    // BCLI - low priority
//    IPR2bits.BCL1IP = 0;    

    // SSPI - low priority
//    IPR1bits.SSP1IP = 0;

    // Enable high priority global interrupts
    INTERRUPT_GlobalInterruptHighEnable();

    // Enable low priority global interrupts.
    INTERRUPT_GlobalInterruptLowEnable();

    // Enable the Peripheral Interrupts
    INTERRUPT_PeripheralInterruptEnable();
}

void __interrupt(high_priority) INTERRUPT_InterruptManagerHigh(void)
{
    // interrupt handler
//    if(PIE1bits.RC1IE == 1 && PIR1bits.RC1IF == 1)
//    {
//        EUSART1_RxDefaultInterruptHandler();
//    }

    // else if
    if(PIE1bits.CCP1IE == 1 && PIR1bits.CCP1IF == 1)
    {
        ECCP1_CompareISR();
    }
    else if(INTCONbits.INT0IE == 1 && INTCONbits.INT0IF == 1)
    {
        INT0_ISR();
    }
    else if(INTCON3bits.INT1IE == 1 && INTCON3bits.INT1IF == 1)
    {
        INT1_ISR();
    }
    else if(INTCON3bits.INT2IE == 1 && INTCON3bits.INT2IF == 1)
    {
        INT2_ISR();
    }
    
    // CCP2 callback
    else if(PIE2bits.CCP2IE == 1 && PIR2bits.CCP2IF == 1)
    {
        CCP2_ISR();
    }   
    
    else
    {
        //Unhandled Interrupt
    }
}

void __interrupt(low_priority) INTERRUPT_InterruptManagerLow(void)
{
    // interrupt handler
    if(PIE2bits.BCL1IE == 1 && PIR2bits.BCL1IF == 1)
    {
//        MSSP1_InterruptHandler();
    }
    else if(PIE1bits.SSP1IE == 1 && PIR1bits.SSP1IF == 1)
    {
//        MSSP1_InterruptHandler();
    }
    else if(PIE1bits.TX1IE == 1 && PIR1bits.TX1IF == 1)
    {
//        EUSART1_TxDefaultInterruptHandler();
        if (PIR1bits.TXIF)
        {
          PIR1bits.TXIF = 0;
        }
    }
    else if(PIE1bits.RC1IE == 1 && PIR1bits.RC1IF == 1)
    {
//        EUSART1_RxDefaultInterruptHandler();
        if(PIR1bits.RCIF)
        {
            uart1_getc();
            PIR1bits.RCIF = 0;
        }
    }
    else if(INTCONbits.TMR0IE == 1 && INTCONbits.TMR0IF == 1)
    {
        TMR0_ISR();
    }
    
    else if(PIE2bits.TMR3IE == 1 && PIR2bits.TMR3IF == 1)
    {
        TMR3_ISR();
    }
    else
    {
        //Unhandled Interrupt
    }
}
