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

// ok code previous video, this is i am using
#define TIME_PER_OVERFLOW   (8191.875)         //(65535u)   // us
#define TIME_PER_TICK       (0.0625f)          //(1u)       // us

typedef struct
{
    uint8_t ccpL;
    uint8_t ccpH;
} cap_uint16;

extern volatile cap_uint16 capture_value;

extern uint16_t ccp2value;
extern volatile uint32_t overflow_tmr3;
extern volatile uint32_t cap_overflow_tmr3;
extern volatile bool ccp2Flag;

// --Global functions-----------------------------------------------------------
void CCP2_Initialize(void);
//uint32_t time_after_capture(uint32_t overflow, cap_uint16 *capture_now);
uint32_t time_after_capture(uint32_t overflow, cap_uint16 *);
void TMR3_ISR(void);
void CCP2_ISR(void);

#endif	/* CAPTURE_H */

