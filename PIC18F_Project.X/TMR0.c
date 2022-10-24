#include  "TMR0.h"
#include "UART.h"
#include "EXTI.h"

volatile uint16_t timer0ReloadVal;
//volatile bool int0Flag = false;
//volatile bool int1Flag = false;
//volatile bool int2Flag = false;
volatile bool tmr0Flag = false;


void TMR0_Initialize(void)
{
    // TMR0 config
    T0CONbits.TMR0ON = TMR0_STOP;
    T0CONbits.T08BIT = TMR0_16BIT_OPERATION;
    T0CONbits.T0CS = TMR0_TIMER_MODE;  
    T0CONbits.T0SE = TMR0_SRC_EDGE_HtoL_T0CKI;
    T0CONbits.PSA = TMR0_PSC_ASSIGN;
    T0CONbits.T0PS = TMR0_PSC_1_32;    

    // 80ms = 0x63BF
    // TMR0H 99; 
    TMR0H = 0x63;
    // TMR0L 191; 
    TMR0L = 0xBF;

    // Load TMR0 value to the 16-bit reload variable
    timer0ReloadVal = (uint16_t)((TMR0H << 8) | TMR0L);
    
    INTCON2bits.TMR0IP = 0;
    
    // Clear Interrupt flag before enabling the interrupt
    INTCONbits.TMR0IF = 0;

    // Disabling TMR0 interrupt.
    INTCONbits.TMR0IE = 0;
    
    T0CONbits.TMR0ON = TMR0_STOP;
}

void TMR0_StartTimer(void)
{
    // Start the Timer by writing to TMR0ON bit
    T0CONbits.TMR0ON = 1;
}

void TMR0_StopTimer(void)
{
    // Stop the Timer by writing to TMR0ON bit
    T0CONbits.TMR0ON = 0;
}

uint16_t TMR0_ReadTimer(void)
{
    uint16_t readVal;
    uint8_t readValLow;
    uint8_t readValHigh;

    readValLow  = TMR0L;
    readValHigh = TMR0H;
    readVal  = ((uint16_t)readValHigh << 8) + readValLow;

    return readVal;
}

void TMR0_WriteTimer(uint16_t timerVal)
{
    // Write to the Timer0 register
    TMR0H = timerVal >> 8;
    TMR0L = (uint8_t) timerVal;
}

void TMR0_Reload(void)
{
    // Write to the Timer0 register
    TMR0H = timer0ReloadVal >> 8;
    TMR0L = (uint8_t) timer0ReloadVal;
}

volatile uint8_t state = 0;

void TMR0_ISR(void)
{
//    tmr0Flag = true;
    /*if(INTCON2bits.INTEDG1 == EXTI_FALLING_EDGE)
    {
        uart1_puts("RB1 falling---\n");
        INTCON2bits.INTEDG1 = EXTI_RISING_EDGE;
        INTCON3bits.INT1IF = 0;  // clear flag before enabling interrupt
    }
    else if(INTCON2bits.INTEDG1 == EXTI_RISING_EDGE)
    {
        uart1_puts("---RB1 rising\n");
        INTCON2bits.INTEDG1 = EXTI_FALLING_EDGE;
        INTCON3bits.INT1IF = 0;  // clear flag before enabling interrupt
    }
    // clear the TMR0 interrupt flag
    INTCONbits.TMR0IF = 0;
    INTCONbits.TMR0IE = 0;*/

    // reload TMR0
    // Write to the Timer0 register
//    TMR0H = timer0ReloadVal >> 8;
//    TMR0L = (uint8_t) timer0ReloadVal;
    
//    TMR0_WriteTimer(25535);
//    TMR0_Reload();
    
}

