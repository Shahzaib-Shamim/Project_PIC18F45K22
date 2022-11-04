/* 
 * File:   EXTI.h
 * Author: FuelMotionInc
 *
 * Created on October 21, 2022, 3:50 PM
 */

#ifndef EXTI_H
#define	EXTI_H

// --Includes-------------------------------------------------------------------
#include <xc.h>
#include "main.h"

// --Global variables-----------------------------------------------------------

// --Macros---------------------------------------------------------------------
#define EXTI_RISING_EDGE    1
#define EXTI_FALLING_EDGE   0
#define EXTI_ENABLE         1
#define EXTI_DISABLE        0
#define PRESSED             1       // active high
#define NOT_PRESSED         0       // active high (pulldown)

#define INT0_FLAG           0x01
#define INT1_FLAG           0x02
#define INT2_FLAG           0x04
#define RB4_FLAG            0x10
#define RB5_FLAG            0x20
#define RB6_FLAG            0x40

extern volatile bool int0Flag;
extern volatile bool int1Flag;
extern volatile bool int2Flag;
extern volatile uint8_t buttonsEventFlag;
extern volatile bool rb4Flag;
extern volatile bool rb5Flag;
extern volatile bool rb6Flag;

// --Global functions-----------------------------------------------------------
void EXTI0_Initialize(void);
void EXTI1_Initialize(void);
void EXTI2_Initialize(void);
void IOC_Initialize(void);


// --INTx ISR
void INT0_ISR(void);
void INT1_ISR(void);
void INT2_ISR(void);
void IOC_ISR(void);

#endif	/* EXTI_H */

