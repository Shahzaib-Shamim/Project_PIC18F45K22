/* 
 * File:   TMR.h
 * Author: Shahzaib
 *
 * Created on August 18, 2022, 11:40 PM
 */

#ifndef TMR_H
#define	TMR_H

// --Includes-------------------------------------------------------------------
#include <xc.h>
#include "main.h"

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

// --Macro Declarations---------------------------------------------------------
#define CLEAR 0
#define SET   1
#define TIMER1_PERIOD 0xFFFF  // 65535

// T1CON - Timer1 control register
#define TMR1_START                          1
#define TMR1_STOP                           0
#define TMR1_16BIT_OPERATION                1
#define TMR1_8BIT_OPERATION                 0
#define TMR1_nSYNC_wFOSC                    1
#define TMR1_SYNC_wFOSC                     0
#define TMR1_SECONDARY_OSC_CIRCUIT_ENABLE   1   // clock source from TxCKI pin
#define TMR1_SECONDARY_OSC_CIRCUIT_DISABLE  0  // clock source from external oscillator
#define TMR1_PSC_1_1                        0
#define TMR1_PSC_1_2                        1
#define TMR1_PSC_1_4                        2
#define TMR1_PSC_1_8                        3
#define TMR1_CKSRC_FOSC_DIV4                0
#define TMR1_CKSRC_FOSC                     1
#define TMR1_CKSRC_PIN_OR_OSC               2    // must set T1SOSCEN = 1


/**
  @Summary
    Initializes the TMR1

  @Description
    This routine initializes the TMR1.
    This routine must be called before any other TMR1 routine is called.
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
    TMR1_Initialize();
    </code>
*/
void TMR1_Initialize(void);

/**
  @Summary
    Start TMR1

  @Description
    This routine is used to  Start TMR1.

  @Preconditions
    The TMR1_Initialize() routine should be called
    prior to use this routine.

  @Param
    None

  @Returns
    None

  @Example
    <code>
    // Initialize TMR1 module

    // Start TMR1
    TMR1_StartTimer();

    while(1)
    {
    }
    </code>
 */
void TMR1_StartTimer(void);

/**
  @Summary
    Stop TMR1

  @Description
    This routine is used to  Stop TMR1.

  @Preconditions
    The TMR1_Initialize() routine should be called
    prior to use this routine.

  @Param
    None

  @Returns
    None

  @Example
    </code>
    TMR1_Initialize();

    TMR1_StartTimer();
    if(TMR1_HasOverflowOccured())
    {
        TMR1_StopTimer();
    }
    <code>
 */
void TMR1_StopTimer(void);

/**
  @Summary
    Read TMR1 register.

  @Description
    This routine is used to  Read TMR1 register.

  @Preconditions
    The TMR1_Initialize() routine should be called
    prior to use this routine.

  @Param
    None

  @Returns
    TMR1 value at the time of the function call read as a 16-bit value

  @Example
    <code>
    uint16_t timerVal=0;

    TMR1_Initialize();

    TMR1_StartTimer();

    // some delay or code

    TMR1_StopTimer();

    timerVal=TMR1_ReadTimer();
    </code>
 */
uint16_t TMR1_ReadTimer(void);

/**
  @Summary
    Write TMR1 register.

  @Description
    This routine is used to Write TMR1 register.

  @Preconditions
    The TMR1_Initialize() routine should be called
    prior to use this routine.

  @Param
    timerVal : Timer value to be loaded

  @Returns
    None

  @Example
    <code>
    TMR1_Initialize();
    TMR1_WriteTimer(0x055);
    TMR1_StartTimer();
    </code>
 */
void TMR1_WriteTimer(uint16_t timerVal);

/**
  @Summary
    Reload TMR1 register.

  @Description
    This routine is used to reload TMR1 register.

  @Preconditions
    The TMR1_Initialize() routine should be called
    prior to use this routine.

  @Param
    None

  @Returns
    None

  @Example
    <code>
    TMR1_Initialize();
    TMR1_StartTimer();

    if(TMR1_HasOverflowOccured())
    {
        TMR1_StopTimer();
    }

    TMR1_Reload();}
    </code>
*/
void TMR1_Reload(void);

/**
  @Summary
    Starts the single pulse acquisition in TMR1 gate operation.

  @Description
    This function starts the single pulse acquisition in TMR1 gate operation.
    This function must be used when the TMR1 gate is enabled.

  @Preconditions
    Initialize  the TMR1 with gate enable before calling this function.

  @Param
    None

  @Returns
    None

  @Example
    <code>
    uint16_t xVal;
    uint16_t yVal;

    //enable TMR1 singlepulse mode
    TMR1_StartSinglePulseAcquistion();

    //check TMR1 gate status
    if(TMR1_CheckGateValueStatus()== 0)
    {
        xVal = TMR1_ReadTimer();
    }

    // wait untill gate interrupt occured
    while(TMR1GIF == 0)
    {
    }

    yVal = TMR1_ReadTimer();
    </code>
*/
void TMR1_StartSinglePulseAcquisition(void);

/**
  @Summary
    Check the current state of Timer1 gate.

  @Description
    This function reads the TMR1 gate value and return it.
    This function must be used when the TMR1 gate is enabled.

  @Preconditions
    Initialize  the TMR1 with gate enable before calling this function.

  @Param
    None

  @Returns
    None

  @Example
    <code>
    uint16_t xVal;
    uint16_t yVal;

    //enable TMR1 singlepulse mode
    TMR1_StartSinglePulseAcquistion();

    //check TMR1 gate status
    if(TMR1_CheckGateValueStatus()== 0)
    {
        xVal = TMR1_ReadTimer();
    }

    //wait untill gate interrupt occured
    while(TMR1IF == 0)
    {
    }

    yVal = TMR1_ReadTimer();
    </code>
*/
uint8_t TMR1_CheckGateValueStatus(void);

/**
  @Summary
    Get the TMR1 overflow status.

  @Description
    This routine get the TMR1 overflow status.

  @Preconditions
    The TMR1_Initialize() routine should be called
    prior to use this routine.

  @Param
    None

  @Returns
    true  - Overflow has occured.
    false - Overflow has not occured.

  @Comment


  @Example
     <code>
     TMR1_Initialize();

     TMR1_StartTimer();

    while(1)
    {
        if(TMR1_HasOverflowOccured())
        {
            TMR1_StopTimer();
        }
    }
    </code>
*/
bool TMR1_HasOverflowOccured(void);


#endif	/* TMR_H */

