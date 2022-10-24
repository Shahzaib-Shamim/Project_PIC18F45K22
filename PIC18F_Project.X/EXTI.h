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
//extern volatile bool int0Flag;
//extern volatile bool int1Flag;
//extern volatile bool int2Flag;

// --Macros---------------------------------------------------------------------
#define EXTI_RISING_EDGE    1
#define EXTI_FALLING_EDGE   0
#define EXTI_ENABLE         1
#define EXTI_DISABLE        0
#define PRESSED             1       // active low
#define NOT_PRESSED         0       // active low (pullup)

extern volatile bool int0Flag;
extern volatile bool int1Flag;
extern volatile bool int2Flag;

// --Global functions-----------------------------------------------------------
void EXTI0_Initialize(void);
void EXTI1_Initialize(void);
void EXTI2_Initialize(void);

// --INTx ISR
void INT0_ISR(void);
void INT1_ISR(void);
void INT2_ISR(void);

#endif	/* EXTI_H */

