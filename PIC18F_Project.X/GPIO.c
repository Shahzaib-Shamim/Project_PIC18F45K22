#include "GPIO.h"

void PIN_MANAGER_Initialize(void)
{
    // --PORT bits state initialize---------------------------------------------
//    LATE = 0x00;
    LATD = 0x00;
//    LATA = 0x00;
//    LATB = 0x00;
    LATC = 0x00;
    LATCbits.LATC6 = HIGH;  // RX1 (added)

    // --GPIO set direction-----------------------------------------------------
//    TRISE = 0x07;
//    TRISA = 0xFF;
//    TRISB = 0xFF;
//    TRISC = 0xBF;
    // CCP1 pin
    TRISCbits.TRISC2 = OUTPUT;
    
    // I2C1 pins
    TRISCbits.TRISC3 = INPUT;   // SCL1
    TRISCbits.TRISC4 = INPUT;   // SDA1
    
//    TRISD = 0xFB;
    
    // EUSART1 pins
    TRISCbits.TRISC6 = OUTPUT;  // TX1 pin
    TRISCbits.TRISC7 = INPUT;   // RX1 pin

    // --GPIO mode select-------------------------------------------------------
//    ANSELD = 0xFB;
    
    // LED
    ANSELDbits.ANSD2 = DIGITAL;           // LED
//    ANSELC = 0x3C;
//    ANSELB = 0x3F;
//    ANSELE = 0x07;
//    ANSELA = 0x2F;
    
    // EUSART1
    ANSELCbits.ANSC6 = DIGITAL;           // TX1
    ANSELCbits.ANSC7 = DIGITAL;           // RX1
    
    // I2C1
    ANSELCbits.ANSC3 = DIGITAL;           // SCL1
    ANSELCbits.ANSC4 = DIGITAL;           // SDA1

    // --Pullup enable/disable--------------------------------------------------
    WPUB = 0x00;
//    INTCON2bits.nRBPU = PULL_UP_ENABLED;
    INTCON2bits.nRBPU = 1;      // Pullup disabled
}
  
void PIN_MANAGER_IOC(void)
{   
	// Clear global Interrupt-On-Change flag
    INTCONbits.RBIF = 0;
}

/**
 End of File
*/