#include "TMR.h"

// --Global variable defination-------------------------------------------------
volatile uint16_t timer1ReloadVal;

void TMR1_Initialize(void)
{
    //Set the Timer to the options selected in the GUI

    // Timer1 gate function disabled
    T1GCON = 0x00;

    // reset high and low byte registers
    TMR1H = 0x00;
    TMR1L = 0x00;

    // Load the TMR value to reload variable
    timer1ReloadVal = TMR1;

    // Clearing IF flag.
    PIR1bits.TMR1IF = 0;

//    T1CON = 0x31;
    T1CONbits.TMR1ON = TMR1_STOP;          // start
    T1CONbits.T1RD16 = TMR1_8BIT_OPERATION;
    T1CONbits.T1SYNC = TMR1_SYNC_wFOSC;
    T1CONbits.T1SOSCEN = TMR1_SECONDARY_OSC_CIRCUIT_DISABLE;
    T1CONbits.T1CKPS = TMR1_PSC_1_8;
    T1CONbits.TMR1CS = TMR1_CKSRC_FOSC_DIV4;
}

void TMR1_StartTimer(void)
{
    T1CONbits.TMR1ON = TMR1_START;
}

void TMR1_StopTimer(void)
{
    T1CONbits.TMR1ON = TMR1_STOP;
}

uint16_t TMR1_ReadTimer(void)
{
    uint16_t readVal;
    uint8_t readValHigh;
    uint8_t readValLow;

    T1CONbits.T1RD16 = TMR1_16BIT_OPERATION;

    readValLow = TMR1L;
    readValHigh = TMR1H;

    readVal = ((uint16_t) readValHigh << 8) | readValLow;

    return readVal;
}

void TMR1_WriteTimer(uint16_t timerVal)
{
    if(T1CONbits.T1SYNC == 1)
    {
        T1CONbits.TMR1ON = 0;

        TMR1H = (uint8_t) (timerVal >> 8);
        TMR1L = (uint8_t) timerVal;

        T1CONbits.TMR1ON = 1;
    }
    else
    {
        TMR1H = (uint8_t) (timerVal >> 8);
        TMR1L = (uint8_t) timerVal;
    }
}

void TMR1_Reload(void)
{
    TMR1_WriteTimer(timer1ReloadVal);
}

void TMR1_StartSinglePulseAcquisition(void)
{
    T1GCONbits.T1GGO = 1;
}

uint8_t TMR1_CheckGateValueStatus(void)
{
    return T1GCONbits.T1GVAL;
}

bool TMR1_HasOverflowOccured(void)
{
    return(PIR1bits.TMR1IF);
}