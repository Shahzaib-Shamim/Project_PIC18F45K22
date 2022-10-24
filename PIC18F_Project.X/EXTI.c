#include "EXTI.h"
#include "TMR0.h"
#include "UART.h"

volatile bool int0Flag = false;
volatile bool int1Flag = false;
volatile bool int2Flag = false;

void EXTI0_Initialize(void)
{
  // High priority interrupt
  INTCON2bits.INTEDG0 = EXTI_RISING_EDGE;      //EXTI_FALLING_EDGE
  INTCONbits.INT0IF = 0;  // clear flag before enabling interrupt
  INTCONbits.INT0IE = EXTI_ENABLE;
}

void EXTI1_Initialize(void)
{
  INTCON2bits.INTEDG1 = EXTI_RISING_EDGE;      //EXTI_FALLING_EDGE
  INTCON3bits.INT1IP = 1;
  INTCON3bits.INT1IF = 0;  // clear flag before enabling interrupt
  INTCON3bits.INT1IE = EXTI_ENABLE;
}

void EXTI2_Initialize(void)
{
  INTCON2bits.INTEDG2 = EXTI_RISING_EDGE;      //EXTI_FALLING_EDGE
  INTCON3bits.INT2IP = 1;
  INTCON3bits.INT2IF = 0;  // clear flag before enabling interrupt
  INTCON3bits.INT2IE = EXTI_ENABLE;
}

void INT0_ISR(void)
{
  TMR0_WriteTimer(DEBOUNCE_TIME_100MS);
  INTCONbits.TMR0IF = 0;
  INTCONbits.TMR0IE = 1;
  TMR0_StartTimer();
  INTCONbits.INT0IF = 0;
}

void INT1_ISR(void)
{
  /*TMR0_WriteTimer(DEBOUNCE_TIME_100MS);
  INTCONbits.TMR0IF = 0;
  INTCONbits.TMR0IE = 1;
  TMR0_StartTimer();
  INTCON3bits.INT1IF = 0;*/
    
    //old start
//    INTCON3bits.INT1IF = 0;
//    if(INTCON2bits.INTEDG1 == EXTI_FALLING_EDGE)
//    {
//        uart1_puts("\nRB1 falling---\n");
//        INTCON2bits.INTEDG1 = EXTI_RISING_EDGE;
////        INTCON3bits.INT1IF = 0;  // clear flag before enabling interrupt
//    }
//    else if(INTCON2bits.INTEDG1 == EXTI_RISING_EDGE)
//    {
//        uart1_puts("---RB1 rising\n\n");
//        INTCON2bits.INTEDG1 = EXTI_FALLING_EDGE;
////        INTCON3bits.INT1IF = 0;  // clear flag before enabling interrupt
//    }
    //old stop
    
    INTCON3bits.INT1IF = 0;
    /*if(INTCON2bits.INTEDG1 == EXTI_RISING_EDGE)
    {
        uart1_puts("\nRB1 rising---\n");
        INTCON2bits.INTEDG1 = EXTI_FALLING_EDGE;
//        INTCON3bits.INT1IF = 0;  // clear flag before enabling interrupt
    }
    else if(INTCON2bits.INTEDG1 == EXTI_FALLING_EDGE)
    {
        uart1_puts("---RB1 falling\n\n");
        INTCON2bits.INTEDG1 = EXTI_RISING_EDGE;
//        INTCON3bits.INT1IF = 0;  // clear flag before enabling interrupt
    }*/
}

void INT2_ISR(void)
{
  TMR0_WriteTimer(DEBOUNCE_TIME_100MS);
  INTCONbits.TMR0IF = 0;
  INTCONbits.TMR0IE = 1;
  TMR0_StartTimer();
  INTCON3bits.INT2IF = 0;
}

