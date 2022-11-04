#include "EXTI.h"
#include "TMR0.h"
#include "UART.h"

volatile bool int0Flag = false;
volatile bool int1Flag = false;
volatile bool int2Flag = false;
volatile bool rb4Flag = false;
volatile bool rb5Flag = false;
volatile bool rb6Flag = false;

//Variable for saving the previous RBx status
volatile uint8_t previous_PORTB_RB4 = 0;  
volatile uint8_t previous_PORTB_RB5 = 0;
volatile uint8_t previous_PORTB_RB6 = 0;

void EXTI0_Initialize(void)
{
    // High priority interrupt
    INTCON2bits.INTEDG0 = EXTI_FALLING_EDGE;
    INTCONbits.INT0IF = 0; // clear flag before enabling interrupt
    INTCONbits.INT0IE = EXTI_ENABLE;
}

void EXTI1_Initialize(void)
{
    INTCON2bits.INTEDG1 = EXTI_RISING_EDGE;
    INTCON3bits.INT1IP = 0;
    INTCON3bits.INT1IF = 0; // clear flag before enabling interrupt
    INTCON3bits.INT1IE = EXTI_ENABLE;
}

void EXTI2_Initialize(void)
{
    INTCON2bits.INTEDG2 = EXTI_RISING_EDGE;
    INTCON3bits.INT2IP = 0;
    INTCON3bits.INT2IF = 0; // clear flag before enabling interrupt
    INTCON3bits.INT2IE = EXTI_ENABLE;
}

void IOC_Initialize(void)
{
    IOCBbits.IOCB4 = 1; //Enable IOC on RB4 pin
    IOCBbits.IOCB5 = 1; //Enable IOC on RB5 pin
    IOCBbits.IOCB6 = 1; //Enable IOC on RB6 pin
    PORTB;
    INTCON2bits.RBIP = 0;   //Configure as high prio.
    INTCONbits.RBIF = 0;    //Clear the flag
    INTCONbits.RBIE = 1;    //Enable the interrupt
}

void INT0_ISR(void)
{
    int0Flag = true;
    INTCONbits.INT0IF = 0;
}

void INT1_ISR(void)
{
    int1Flag = true;
    TMR0_WriteTimer(DEBOUNCE_TIME_50MS);
    INTCONbits.TMR0IF = 0;
    INTCONbits.TMR0IE = 1;
    TMR0_StartTimer();
    INTCON3bits.INT1IF = 0;    
}

void INT2_ISR(void)
{
    int2Flag = true;
    TMR0_WriteTimer(DEBOUNCE_TIME_50MS);
    INTCONbits.TMR0IF = 0;
    INTCONbits.TMR0IE = 1;
    TMR0_StartTimer();
    INTCON3bits.INT2IF = 0;
}

void IOC_ISR(void)
{
    //Delay for debouncing 
    TMR0_WriteTimer(DEBOUNCE_TIME_50MS);
    INTCONbits.TMR0IF = 0;
    INTCONbits.TMR0IE = 1;
    TMR0_StartTimer();

    //Read the current PORTB
    uint8_t current_PORTB = PORTB;

    //Extract the RB5 and RB4 pins from current PORTB
    uint8_t current_PORTB_RB4 = (current_PORTB & 0b00010000) >> 4;
    uint8_t current_PORTB_RB5 = (current_PORTB & 0b00100000) >> 5;
    uint8_t current_PORTB_RB6 = (current_PORTB & 0b01000000) >> 6;    
    
    //If RB4 changed state
    if(previous_PORTB_RB4 != current_PORTB_RB4)
    {
        rb4Flag = true;
    }
    //If RB5 changed state
    else if(previous_PORTB_RB5 != current_PORTB_RB5)
    {
        rb5Flag = true;
    }    
    //If RB6 changed state
    else if((previous_PORTB_RB6 & 0b01000000) != current_PORTB_RB6)
    {
        rb6Flag = true;
    }

    //Read the current PORB again and update the previous states
    current_PORTB = PORTB;
    previous_PORTB_RB4 = (current_PORTB & 0b00010000) >> 4;
    previous_PORTB_RB5 = (current_PORTB & 0b00100000) >> 5;
    previous_PORTB_RB6 = (current_PORTB & 0b01000000) >> 6;

    //Clear the flag
    INTCONbits.RBIF = 0;
}
