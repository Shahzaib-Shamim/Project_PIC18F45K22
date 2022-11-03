#include "Interrupt.h"
#include "UART.h"
#include "ECCP.h"
#include "TMR0.h"
#include "EXTI.h"
#include "Capture.h"

void INTERRUPT_Initialize(void)
{
    // Enable Interrupt Priority Vectors
    RCONbits.IPEN = 1;  
        
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
    if(PIE1bits.CCP1IE == 1 && PIR1bits.CCP1IF == 1)
    {
        ECCP1_CompareISR();
    }
    else if(INTCONbits.INT0IE == 1 && INTCONbits.INT0IF == 1)
    {
        INT0_ISR();
    }
    /*else if(INTCON3bits.INT1IE == 1 && INTCON3bits.INT1IF == 1)
    {
        INT1_ISR();
    }
    else if(INTCON3bits.INT2IE == 1 && INTCON3bits.INT2IF == 1)
    {
        INT2_ISR();
    }*/   
    else if(PIE2bits.CCP2IE == 1 && PIR2bits.CCP2IF == 1)
    {
//        CCP2_ISR();
    }    
    //TMR3
    else if(PIE2bits.TMR3IE == 1 && PIR2bits.TMR3IF == 1)
    {
//        TMR3_ISR();
    }
    //TMR5
//    else if(PIE5bits.TMR5IE == 1 && PIR5bits.TMR5IF == 1)
//    {
//        TMR5_ISR();
//    }
    else
    {
        //Unhandled Interrupt
    }
}

void __interrupt(low_priority) INTERRUPT_InterruptManagerLow(void)
{
    // interrupt handler
    /*if(PIE2bits.BCL1IE == 1 && PIR2bits.BCL1IF == 1)
    {
//        MSSP1_InterruptHandler();
    }
    else if(PIE1bits.SSP1IE == 1 && PIR1bits.SSP1IF == 1)
    {
//        MSSP1_InterruptHandler();
    }*/
    if(PIE1bits.TX1IE == 1 && PIR1bits.TX1IF == 1)
    {
        if (PIR1bits.TXIF)
        {
          PIR1bits.TXIF = 0;
        }
    }
    else if(PIE1bits.RC1IE == 1 && PIR1bits.RC1IF == 1)
    {
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
    else if(INTCONbits.RBIF == 1 && INTCONbits.RBIE == 1)
    {
        IOC_ISR();
    }
    else if(INTCON3bits.INT1IE == 1 && INTCON3bits.INT1IF == 1)
    {
        INT1_ISR();
    }
    else if(INTCON3bits.INT2IE == 1 && INTCON3bits.INT2IF == 1)
    {
        INT2_ISR();
    }
    else
    {
        //Unhandled Interrupt
    }
}
