#include "GPIO.h"

void PIN_MANAGER_Initialize(void)
{
    // --GPIO set direction-----------------------------------------------------
//    TRISE = 0x07;
//    TRISA = 0xFF;
//    TRISB = 0xFF;
//    TRISC = 0xBF;
    
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
    
    // --GPIO mode select-------------------------------------------------------
    
    // GPIOs
    ANSELDbits.ANSD2 = DIGITAL;           // LED
//    ANSELC = 0x3C;
//    ANSELB = 0x3F;
//    ANSELE = 0x07;
//    ANSELA = 0x2F;
    
    // EXTI pins (RB0 = INT0, RB1 = INT1, RB2 = INT2)
    TRISBbits.TRISB0 = INPUT;
    TRISBbits.TRISB1 = INPUT;
    TRISBbits.TRISB2 = INPUT;
    ANSELBbits.ANSB0 = DIGITAL;
    ANSELBbits.ANSB1 = DIGITAL;
    ANSELBbits.ANSB2 = DIGITAL;
    
    // CCP2 pin input
    TRISCbits.TRISC1 = INPUT;
    
    // --PORTB Pullups----------------------------------------------------------
    WPUB = 0x00;        // PORTB pullup disable
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