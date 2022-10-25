/* 
 * File:   TMR0.h
 * Author: FuelMotionInc
 *
 * Created on October 21, 2022, 4:35 PM
 */

#ifndef TMR0_H
#define	TMR0_H

// --Includes-------------------------------------------------------------------
#include <xc.h>
#include "main.h"

// Timer0 calculation----------------------------------------------
/**
 * x sec = (4/Fosc) * PSC * (65535 - Start value)
 * 
 * Example:
 * x sec = (4/64MHz) * 2 * (65535 - 36000)
 * x = 3.69 ms (on scope = 7.39 ms)
 * OR
 * Start value = 65535 - ((req. seconds * (Fosc * 4)) / (4 * PSC))
 * 
 * 
 * Note: Here, Fosc = actual crystal value i.e. 16MHz
 */

// --Macro Declarations---------------------------------------------------------
#define CLEAR 0
#define SET   1
#define TIMER0_PERIOD 0xFFFF  // 65535

// T1CON - Timer1 control register
#define TMR0_START                          1
#define TMR0_STOP                           0
#define TMR0_16BIT_OPERATION                0
#define TMR0_8BIT_OPERATION                 1
#define TMR0_COUNTER_MODE                   1
#define TMR0_TIMER_MODE                     0
#define TMR0_SRC_EDGE_HtoL_T0CKI            1
#define TMR0_SRC_EDGE_LtoH_T0CKI            0
#define TMR0_PSC_ASSIGN                     0
#define TMR0_PSC_NOT_ASSIGN                 1
#define TMR0_PSC_1_1                        0
#define TMR0_PSC_1_4                        1
#define TMR0_PSC_1_8                        2
#define TMR0_PSC_1_16                       3
#define TMR0_PSC_1_32                       4
#define TMR0_PSC_1_64                       5
#define TMR0_PSC_1_128                      6
#define TMR0_PSC_1_256                      7

#define DEBOUNCE_TIME_20MS          55535
#define DEBOUNCE_TIME_30MS          50535
#define DEBOUNCE_TIME_50MS          40535
#define DEBOUNCE_TIME_80MS          25535
#define DEBOUNCE_TIME_100MS         15535

// --Global variables-----------------------------------------------------------
//extern volatile bool int0Flag;
//extern volatile bool int1Flag;
//extern volatile bool int2Flag;
extern volatile bool tmr0Flag;


/**
  @Summary
    Initializes the TMR0.

  @Description
    This function initializes the TMR0 Registers.
    This function must be called before any other TMR0 function is called.

  @Preconditions
    None

  @Param
    None

  @Returns
    None

  @Comment
    
	
  @Example
    <code>
    main()
    {
        // Initialize TMR0 module
        TMR0_Initialize();

        // Do something else...
    }
    </code>
*/
void TMR0_Initialize(void);

/**
  @Summary
    This function starts the TMR0.

  @Description
    This function starts the TMR0 operation.
    This function must be called after the initialization of TMR0.

  @Preconditions
    Initialize  the TMR0 before calling this function.

  @Param
    None

  @Returns
    None

  @Example
    <code>
    // Initialize TMR0 module

    // Start TMR0
    TMR0_StartTimer();

    // Do something else...
    </code>
*/
void TMR0_StartTimer(void);

/**
  @Summary
    This function stops the TMR0.

  @Description
    This function stops the TMR0 operation.
    This function must be called after the start of TMR0.

  @Preconditions
    Initialize  the TMR0 before calling this function.

  @Param
    None

  @Returns
    None

  @Example
    <code>
    // Initialize TMR0 module

    // Start TMR0
    TMR0_StartTimer();

    // Do something else...

    // Stop TMR0;
    TMR0_StopTimer();
    </code>
*/
void TMR0_StopTimer(void);


/**
  @Summary
    Reads the 16 bits TMR0 register value.

  @Description
    This function reads the 16 bits TMR0 register value and return it.

  @Preconditions
    Initialize  the TMR0 before calling this function.

  @Param
    None

  @Returns
    This function returns the 16 bits value of TMR0 register.

  @Example
    <code>
    // Initialize TMR0 module

    // Start TMR0
    TMR0_StartTimer();

    // Read the current value of TMR0
    if(0 == TMR0_ReadTimer())
    {
        // Do something else...

        // Reload the TMR value
        TMR0_Reload();
    }
    </code>
*/
uint16_t TMR0_ReadTimer(void);

/**
  @Summary
    Writes the 16 bits value to TMR0 register.

  @Description
    This function writes the 16 bits value to TMR0 register.
    This function must be called after the initialization of TMR0.

  @Preconditions
    Initialize  the TMR0 before calling this function.

  @Param
    timerVal - Value to write into TMR0 register.

  @Returns
    None

  @Example
    <code>
    #define PERIOD 0x8000
    #define ZERO   0x0000

    while(1)
    {
        //Read the TMR0 register
        if(ZERO == TMR0_ReadTimer())
        {
            // Do something else...

            // Write the TMR0 register
            TMR0_WriteTimer(PERIOD);
        }

        // Do something else...
    }
    </code>
*/
void TMR0_WriteTimer(uint16_t timerVal);

/**
  @Summary
    Reload the 16 bits value to TMR0 register.

  @Description
    This function reloads the 16 bit value to TMR0 register.
    This function must be called to write initial value into TMR0 register.

  @Preconditions
    Initialize  the TMR0 before calling this function.

  @Param
    None

  @Returns
    None

  @Example
    <code>
    while(1)
    {
        if(TMR0IF)
        {
            // Do something else...

            // clear the TMR0 interrupt flag
            TMR0IF = 0;

            // Reload the initial value of TMR0
            TMR0_Reload();
        }
    }
    </code>
*/
void TMR0_Reload(void);

/**
  @Summary
    Timer Interrupt Service Routine

  @Description
    Timer Interrupt Service Routine is called by the Interrupt Manager.

  @Preconditions
    Initialize  the TMR0 module with interrupt before calling this isr.

  @Param
    None

  @Returns
    None
 */
void TMR0_ISR(void);

#endif	/* TMR0_H */

