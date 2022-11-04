#include "GPIO.h"

void PIN_MANAGER_Initialize(void)
{
    // I2C1 pins (C3 = SCL1, C4 = SDA1)
    TRISCbits.TRISC3 = INPUT;
    TRISCbits.TRISC4 = INPUT;
    ANSELCbits.ANSC3 = DIGITAL;
    ANSELCbits.ANSC4 = DIGITAL;

    // EUSART1 pins (C6 = TX1, C7 = RX1)
    LATCbits.LATC6 = HIGH;
    TRISCbits.TRISC6 = OUTPUT;
    TRISCbits.TRISC7 = INPUT;
    ANSELCbits.ANSC6 = DIGITAL;
    ANSELCbits.ANSC7 = DIGITAL;

    // GPIOs
    // LED
    ANSELDbits.ANSD2 = DIGITAL; 
    TRISDbits.TRISD2 = OUTPUT;
    LATDbits.LATD2 = LOW;

    // Relay1 to 5 = A0 to A4
    ANSELAbits.ANSA0 = DIGITAL;
    TRISAbits.TRISA0 = OUTPUT;
    LATAbits.LATA0 = LOW;
    ANSELAbits.ANSA1 = DIGITAL;
    TRISAbits.TRISA1 = OUTPUT;
    LATAbits.LATA1 = LOW;
    ANSELAbits.ANSA2 = DIGITAL;
    TRISAbits.TRISA2 = OUTPUT;
    LATAbits.LATA2 = LOW;
    ANSELAbits.ANSA3 = DIGITAL;
    TRISAbits.TRISA3 = OUTPUT;
    LATAbits.LATA3 = LOW;
    TRISAbits.TRISA4 = OUTPUT;
    LATAbits.LATA4 = LOW;

    // Pulse counter A5
    ANSELAbits.ANSA5 = DIGITAL;
    TRISAbits.TRISA5 = OUTPUT;
    LATAbits.LATA5 = LOW;

    // EXTI pins (RB0 = INT0, RB1 = INT1, RB2 = INT2)
    TRISBbits.TRISB0 = INPUT;
    TRISBbits.TRISB1 = INPUT;
    TRISBbits.TRISB2 = INPUT;
    ANSELBbits.ANSB0 = DIGITAL;
    ANSELBbits.ANSB1 = DIGITAL;
    ANSELBbits.ANSB2 = DIGITAL;

    // IOC pins (RB4, RB5, RB6)
    TRISBbits.TRISB4 = INPUT;
    TRISBbits.TRISB5 = INPUT;
    TRISBbits.TRISB6 = INPUT;
    ANSELBbits.ANSB4 = DIGITAL;
    ANSELBbits.ANSB5 = DIGITAL;

    // CCP2 pin input
    TRISCbits.TRISC1 = INPUT;
    TRISCbits.TRISC1 = DIGITAL;

    // --PORTB Pullups----------------------------------------------------------
    WPUB = 0x00; // PORTB pullup disable
    INTCON2bits.nRBPU = PULL_UP_DISABLED;
}

void PIN_MANAGER_IOC(void)
{
    // Clear global Interrupt-On-Change flag
    INTCONbits.RBIF = 0;
}

/**
 End of File
 */