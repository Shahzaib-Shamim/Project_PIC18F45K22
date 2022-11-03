/* 
 * File:   Capture.h
 * Author: Shahzaib Shamim
 *
 * Created on October 22, 2022, 4:02 AM
 */

#ifndef CAPTURE_H
#define	CAPTURE_H

// --Includes-------------------------------------------------------------------
#include <xc.h>
#include "main.h"

/**
 * <Pulse counting calculation>
 * 
 * Tcyc = 4/Fosc = 4/64MHz = 62.5ns
 * Prescalar (PSC) = 1:2
 * Time per tick = Tcyc * PSC = 62.5e-9 * 2 = 125ns
 * Final Pulse time = Tcyc * PSC * (2^16 - 1) = 62.5e-9 * 2 * 65535 = 8.191875ms or 8191.875us
 * 
 * Note: Fosc = actual crystal value i.e. 16MHz???
 */

// if Fosc = 64MHz because of PLL
#define TIME_PER_OVERFLOW   (8191.875f)        //in usec
#define TIME_PER_TICK       (0.125f)  //0.0625f        //in usec
//#define TIME_PER_OVERFLOW   (8191875)        //in nsec
//#define TIME_PER_TICK       (125)  //0.0625f        //in nsec

// --Defines for CCP2-----------------------------------------------------------
#define CCP2_EVERY_RISING_EDGE      0x05
#define CCP2_EVERY_FALLING_EDGE     0x04
#define CCP2_TMR1_SEL               0x00
#define CCP2_TMR3_SEL               0x01
#define CCP2_TMR5_SEL               0x02

// --Defines for TMR3-----------------------------------------------------------
#define TMR3_CS_FOSC        0x01
#define TMR3_CS_FOSC_DIV_4  0x00
#define TMR3_PSC_1_8        0x03
#define TMR3_PSC_1_4        0x02
#define TMR3_PSC_1_2        0x01
#define TMR3_PSC_1_1        0x00
#define TMR3_16BIT_RD_WR    1
#define TMR3_8BIT_RD_WR     0
#define TMR3_START          1
#define TMR3_STOP           0

// --Global variables-----------------------------------------------------------
typedef struct
{
    uint8_t ccpL;
    uint8_t ccpH;
} cap_uint16;

extern volatile cap_uint16 capture_value;
extern uint16_t ccp2value;
extern volatile uint16_t capValue;
extern volatile uint32_t overflow_tmr3;
extern volatile uint32_t cap_overflow_tmr3;
extern volatile bool ccp2Flag;

// --Global functions-----------------------------------------------------------
void CCP2_Initialize(void);
uint32_t time_after_capture(uint32_t overflow, cap_uint16 *);
void TMR3_ISR(void);
void TMR5_ISR(void);
void CCP2_ISR(void);

#endif	/* CAPTURE_H */

