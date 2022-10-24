/* 
 * File:   main.h
 * Author: Shahzaib
 *
 * Created on August 16, 2022, 12:08 AM
 */

#ifndef MAIN_H
#define	MAIN_H

// --Includes-------------------------------------------------------------------
#include <stdint.h>
#include <stdbool.h>
//#include <stdio.h>
#include <string.h>

// --Defines--------------------------------------------------------------------
/**
 * 16MHz crystal, PLL 4x enabled
 * Instruction frequency = 64MHz/4
 */
#define OSCILLATOR_FREQ   16000000UL
#define _XTAL_FREQ        64000000UL
#define FCY               (_XTAL_FREQ/4)     // instruction frequency

// --General purpose defines----------------------------------------------------
#define true    1
#define false   0


#endif	/* MAIN_H */

