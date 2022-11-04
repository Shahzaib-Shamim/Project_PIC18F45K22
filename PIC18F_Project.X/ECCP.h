/* 
 * File:   ECCP.h
 * Author: Shahzaib
 *
 * Created on August 18, 2022, 11:53 PM
 */

#ifndef ECCP_H
#define	ECCP_H

// Timer1 compare mode calculation----------------------------------------------
/**
 * x sec = (4/Fosc) * PSC * (65535 - Start value)
 * 
 * Example:
 * x sec = (4/64MHz) * 2 * (65535 - 36000)
 * x = 3.69 ms (on scope = 7.39 ms)
 * OR
 * Start value = 65535 - ((req. seconds * (Fosc * 4)) / (4 * PSC))
 */

// --Includes-------------------------------------------------------------------
#include <xc.h>
#include "main.h"

// --Macro Declarations---------------------------------------------------------
// CCP1CON - ECCP1 control register
#define ECCP1_MODULE_OFF                          0
#define ECCP1_RESERVED                            1
#define ECCP1_TOGGLE_ON_COMPARE                   2
//#define ECCP1_RESERVED                          3
#define ECCP1_CAPTURE_ON_EVERY_FALLING_EDGE       4
#define ECCP1_CAPTURE_ON_EVERY_RISING_EDGE        5
#define ECCP1_CAPTURE_ON_EVERY_4TH_RISING_EDGE    6
#define ECCP1_CAPTURE_ON_EVERY_16TH_RISING_EDGE   7
#define ECCP1_SET_ON_COMPARE_MATCH                8
#define ECCP1_CLEAR_ON_COMPARE_MATCH              9
#define ECCP1_INTERRUPT_ON_COMPARE_MATCH          10
#define ECCP1_SPECIAL_EVENT_TRIGGER               11
//#define ECCP1_PWM_MODE                          12

// CCPTMRS0 - PWM Timer selection control register0
#define ECCP1_CCP1_TMR1_PWM_TMR2 0
#define ECCP1_CCP1_TMR3_PWM_TMR4 1
#define ECCP1_CCP1_TMR5_PWM_TMR6 2

// --Data type definitions------------------------------------------------------

/**
 @Summary
   Defines the values to convert from 16bit to two 8 bit and vice versa

 @Description
   This routine used to get two 8 bit values from 16bit also
   two 8 bit value are combine to get 16bit.

 Remarks:
   None
 */

typedef union CCPR1Reg_tag {

    struct {
        uint8_t ccpr1l;
        uint8_t ccpr1h;
    };

    struct {
        uint16_t ccpr1_16Bit;
    };
} CCP1_PERIOD_REG_T;


/**
  @Summary
    Initializes the ECCP1

  @Description
    This routine initializes the ECCP1_Initialize.
    This routine must be called before any other ECCP1 routine is called.
    This routine should only be called once during system initialization.

  @Preconditions
    None

  @Param
    None

  @Returns
    None

  @Comment
    

  @Example
    <code>
    uint16_t compare;

    ECCP1_Initialize();
    ECCP1_SetCompareCount(compare);
    </code>
 */
void ECCP1_Initialize(void);

/**
  @Summary
    Loads 16-bit compare value.

  @Description
    This routine loads the 16 bit compare value.

  @Preconditions
    ECCP1_Initialize() function should have been
    called before calling this function.

  @Param
    compareCount: 16-bit unsigned value

  @Returns
    None

  @Example
    <code>
    uint16_t compare;

    ECCP1_Initialize();
    ECCP1_SetCompareCount(compare);
    </code>
 */
void ECCP1_SetCompareCount(uint16_t compareCount);

/**
  @Summary
    Implements ISR

  @Description
    This routine is used to implement the ISR for the interrupt-driven
    implementations.

  @Returns
    None

  @Param
    None
 */
void ECCP1_CompareISR(void);

/**
 * Sets compare value for timer1
 * @param timeoutVal : in usec
 */
void ECCP1_SetTMR1CompareCount(uint16_t);


#endif	/* ECCP_H */

