#include "ECCP.h"
#include "TaskRoutines.h"
#include "GPIO.h"

void ECCP1_Initialize(void)
{
    // Set the ECCP1 to the options selected in the User Interface

    // CCP1M Special Event Trigger; DC1B 0; P1M single;     
    CCP1CON = 0x00;     // reset register
    CCP1CONbits.CCP1M = ECCP1_SPECIAL_EVENT_TRIGGER;

    // CCPR1H 39, initial compare value (may set to 0)
    CCPR1H = 0x00;

    // CCPR1L 16, initial compare value (may set to 0) 
    CCPR1L = 0xC8;

    // Interrupt priority
    IPR1bits.CCP1IP = 1;

    // Clear the ECCP1 interrupt flag
    PIR1bits.CCP1IF = 0;

    // Enable the ECCP1 interrupt
    PIE1bits.CCP1IE = 1;

    // Selecting CCPTMRS0
    CCPTMRS0bits.C1TSEL = ECCP1_CCP1_TMR1_PWM_TMR2;
}

void ECCP1_SetCompareCount(uint16_t compareCount)
{
    CCP1_PERIOD_REG_T module;

    module.ccpr1_16Bit = compareCount - 1; // '-1' added

    CCPR1L = module.ccpr1l;
    CCPR1H = module.ccpr1h;
}

void ECCP1_CompareISR(void)
{
//    static uint32_t i = 0;

//    PIR1bits.CCP1IF = 0;

    timer1.timer1_1msFlag = true;
    timer1.timer1_1msCounter++;

    if((timer1.timer1_1msCounter % LED_BLINK_TASK_TIME) == 0)
    {
        sysFlags.ledBlinkTask = true;
    }
//    if((timer1.timer1_1msCounter % DS3231_TIMESTAMP_TASK) == 0)
//    {
//        sysFlags.ds3231Task = true;
//    }
    
    if((timer1.timer1_1msCounter % ADS1115_SAMPLE_TASK_TIME) == 0)
    {
        sysFlags.ads1115SampleTask = true;
    }
    
    if((timer1.timer1_1msCounter % UART_TX_TASK_TIME) == 0)
    {
        sysFlags.uartTxTask = true;
    }

    if((timer1.timer1_1msCounter % LCD_TASK) == 0)
    {
        sysFlags.lcdTask = true;
    }

    if((timer1.timer1_1msCounter % PULSE_TASK) == 0)
    {
        sysFlags.pulseTask = true;
    }

//    if((timer1.timer1_1msCounter % COEF_TASK) == 0)
//    {
//        sysFlags.coefTask = true;
//    }

    // Reset timer
    if(timer1.timer1_1msCounter == 1000)
    {
        timer1.timer1_1msCounter = 0;
    }
    
    PIR1bits.CCP1IF = 0;
}

void ECCP1_SetTMR1CompareCount(uint16_t timeoutVal)
{
    uint16_t countValue;
    countValue = (uint16_t) (timeoutVal * (_XTAL_FREQ / 1000000)) / (4 * 8);
    ECCP1_SetCompareCount(countValue - 1);
}
