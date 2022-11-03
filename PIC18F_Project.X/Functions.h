/* 
 * File:   Functions.h
 * Author: Shahzaib Shamim
 *
 * Created on October 30, 2022, 8:45 PM
 */

#ifndef FUNCTIONS_H
#define	FUNCTIONS_H

// --Includes-------------------------------------------------------------------
#include <xc.h>
#include "main.h"

// Data Type Section
typedef struct
{
    uint16_t size;
    float *buffer;

} MAVFilter_f_t;

// --Global Variable Section----------------------------------------------------

// --Global Functions Section---------------------------------------------------
MAVFilter_f_t * Initialize_MAVFilter_f(const uint16_t size);
float calculateMovingAverage_f(MAVFilter_f_t *obj, const float newValue);
float calculateRMS(MAVFilter_f_t *obj, const float newValue);

// --Local Functions Section----------------------------------------------------

#endif	/* FUNCTIONS_H */

