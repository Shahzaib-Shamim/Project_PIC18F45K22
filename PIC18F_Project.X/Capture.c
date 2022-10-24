#include "Capture.h"

//typedef struct
//{
//    uint8_t ccpL;
//    uint8_t ccpH;
//} cap_uint16;

volatile cap_uint16 capture_value = {
    .ccpH = 0,
    .ccpL = 0
};
uint16_t ccp2value = 0;
volatile uint32_t overflow_tmr3 = 0;
volatile uint32_t cap_overflow_tmr3 = 0;
volatile bool ccp2Flag = false;

void CCP2_Initialize(void)
{
    // set to detect every rising edge initially
    CCP2CONbits.CCP2M = 0x05;       

    T3CONbits.T3RD16 = 0;       // timer read write operation in 8bit
    //    T3CONbits.T3CCP2 = 0;       // TODO
    //    T3CONbits.T3CCP1 = 1;       // TODO
    CCPTMRS0bits.C2TSEL = 0x02;

    T3CONbits.T3CKPS = 1;       // 1:2 prescalar
    T3CONbits.TMR3CS = 0;       // timer3 clock source is Fosc/4

    // interrupt priorities for timer3 and ccp2
    IPR2bits.TMR3IP = 0;
    IPR2bits.CCP2IP = 1;

    PIR2bits.TMR3IF = 0;    // added
    PIE2bits.TMR3IE = 1;
    PIR2bits.CCP2IF = 0;    // added
    PIE2bits.CCP2IE = 1;    
}

uint32_t time_after_capture(uint32_t overflow, cap_uint16 *capture_now)
{
    uint16_t cap_value = 0;
    memcpy(&cap_value, capture_now, sizeof(cap_uint16));
    return ((overflow * TIME_PER_OVERFLOW) + (cap_value * TIME_PER_TICK));
}

void TMR3_ISR(void)
{
    overflow_tmr3++;
    PIR2bits.TMR3IF = 0;
}

void CCP2_ISR(void)
{
    if(PIR2bits.CCP2IF == 1 && CCP2CONbits.CCP2M == 0x05)
    {
        T3CONbits.TMR3ON = 1;
//        PIE2bits.CCP2IE = 0;
        CCP2CONbits.CCP2M = 0x04;
//        PIE2bits.CCP2IE = 1;
        PIR2bits.CCP2IF = 0;
    }
    if(PIR2bits.CCP2IF == 1 && CCP2CONbits.CCP2M == 0x04)
    {
//        PIE2bits.CCP2IE = 0;
        cap_overflow_tmr3 = overflow_tmr3;
        capture_value.ccpL = CCPR2L;
        capture_value.ccpH = CCPR2H;
        overflow_tmr3 = 0;
        ccp2Flag = true;
        CCP2CONbits.CCP2M = 0x05;
//        PIE2bits.CCP2IE = 1;
        PIR2bits.CCP2IF = 0;
        T3CONbits.TMR3ON = 0;
    }
}