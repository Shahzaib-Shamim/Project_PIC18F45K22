/* 
 * File:   SystemInitalization.h
 * Author: Shahzaib
 *
 * Created on August 16, 2022, 12:31 AM
 */

#ifndef SYSTEMINITALIZATION_H
#define	SYSTEMINITALIZATION_H

#include <xc.h>
#include "main.h"

// --GLobal functions-----------------------------------------------------------
/**
 * Initialize system state and variables
 */
void SYSTEM_Initialization(void);

// --Local functions------------------------------------------------------------
/**
 * External oscillator initialization
 */
static void OSCILLATOR_Init(void);

#endif	/* SYSTEMINITALIZATION_H */

