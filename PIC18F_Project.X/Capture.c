#include "Capture.h"

volatile cap_uint16 capture_value = {.ccpH = 0, .ccpL = 0};
uint16_t ccp2value = 0;
volatile uint32_t overflow_tmr3 = 0;
volatile uint32_t cap_overflow_tmr3 = 0;
volatile bool ccp2Flag = false;
volatile uint16_t capValue = 0;

void CCP2_Initialize(void)
{
    // Configure CCP2 module
    // Set to detect every rising edge initially
    CCP2CONbits.CCP2M = CCP2_EVERY_RISING_EDGE;
    //    CCPTMRS0bits.C2TSEL = CCP2_TMR3_SEL;  //0x02
    CCPTMRS0bits.C2TSEL0 = 1;     
    CCPTMRS0bits.C2TSEL1 = 0;
    IPR2bits.CCP2IP = 1;
    PIR2bits.CCP2IF = 0;
    PIE2bits.CCP2IE = 1;

    // Configure TMR3 for CCP2
    T3CONbits.T3RD16 = TMR3_8BIT_RD_WR;
//    T3CONbits.T3CCP2 = 0;
//    T3CONbits.T3CCP1 = 1;    
    T3CONbits.T3CKPS = TMR3_PSC_1_2;
    T3CONbits.TMR3CS = TMR3_CS_FOSC_DIV_4;
    T3CONbits.TMR3ON = TMR3_STOP;
    IPR2bits.TMR3IP = 1;
    PIR2bits.TMR3IF = 0;
    PIE2bits.TMR3IE = 1;
    
    // Configure TMR5 for CCP2
//    T5CONbits.T5RD16 = TMR3_8BIT_RD_WR;
//    T5CONbits.T5CKPS = TMR3_PSC_1_2;
//    T5CONbits.TMR5CS = TMR3_CS_FOSC_DIV_4;
//    T5CONbits.TMR5ON = TMR3_STOP;
//    IPR5bits.TMR5IP = 1;
//    PIR5bits.TMR5IF = 0;
//    PIE5bits.TMR5IE = 1;
    
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

//void TMR5_ISR(void)
//{
//    overflow_tmr3++;
//    PIR5bits.TMR5IF = 0;
//}

void CCP2_ISR(void)
{
    if(PIR2bits.CCP2IF == 1 && CCP2CONbits.CCP2M == CCP2_EVERY_RISING_EDGE)
    {
        /*
        T3CONbits.TMR3ON = 1;
//        PIE2bits.CCP2IE = 0;
        CCP2CONbits.CCP2M = CCP2_EVERY_FALLING_EDGE;
//        PIE2bits.CCP2IE = 1;
        PIR2bits.CCP2IF = 0;
         */
        
        //using tmr3
        T3CONbits.TMR3ON = 1;
        PIR2bits.CCP2IF = 0;
        PIE2bits.CCP2IE = 0;
        CCP2CONbits.CCP2M = CCP2_EVERY_FALLING_EDGE;
        PIR2bits.CCP2IF = 0;
        PIE2bits.CCP2IE = 1;
        
        //using tmr5
//        T5CONbits.TMR5ON = 1;
//        PIR2bits.CCP2IF = 0;
//        PIE2bits.CCP2IE = 0;
//        CCP2CONbits.CCP2M = CCP2_EVERY_FALLING_EDGE;
//        PIR2bits.CCP2IF = 0;
//        PIE2bits.CCP2IE = 1;
    }
    if(PIR2bits.CCP2IF == 1 && CCP2CONbits.CCP2M == CCP2_EVERY_FALLING_EDGE)
    {
        /*
//        PIE2bits.CCP2IE = 0;
        cap_overflow_tmr3 = overflow_tmr3;
        capture_value.ccpL = CCPR2L;
        capture_value.ccpH = CCPR2H;
        overflow_tmr3 = 0;
        ccp2Flag = true;
        CCP2CONbits.CCP2M = CCP2_EVERY_RISING_EDGE;
//        PIE2bits.CCP2IE = 1;
        PIR2bits.CCP2IF = 0;
        T3CONbits.TMR3ON = 0;
        */
        
        //using tmr3
        cap_overflow_tmr3 = overflow_tmr3;
        T3CONbits.TMR3ON = 0;
        PIR2bits.CCP2IF = 0;
        PIE2bits.CCP2IE = 0;
        capture_value.ccpL = CCPR2L;
        capture_value.ccpH = CCPR2H;
        overflow_tmr3 = 0;
        ccp2Flag = true;
        CCP2CONbits.CCP2M = CCP2_EVERY_RISING_EDGE;
        PIR2bits.CCP2IF = 0;
        PIE2bits.CCP2IE = 1;
        
        
        //using tmr5
//        cap_overflow_tmr3 = overflow_tmr3;
//        T5CONbits.TMR5ON = 0;
//        PIR2bits.CCP2IF = 0;
//        PIE2bits.CCP2IE = 0;
//        capture_value.ccpL = CCPR2L;
//        capture_value.ccpH = CCPR2H;
//        //        capValue = (CCPR2H << 8) | (CCPR2L & 0xff);
//        overflow_tmr3 = 0;
//        ccp2Flag = true;
//        CCP2CONbits.CCP2M = CCP2_EVERY_RISING_EDGE;
//        PIR2bits.CCP2IF = 0;
//        PIE2bits.CCP2IE = 1;
    }
}