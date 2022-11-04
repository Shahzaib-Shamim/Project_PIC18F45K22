/* 
 * File:   ADS1115.h
 * Author: Shahzaib
 *
 * Created on August 23, 2022, 10:34 PM
 */

#ifndef ADS1115_H
#define	ADS1115_H

// --Includes-------------------------------------------------------------------
#include <xc.h>
#include "main.h"
#include "I2C.h"

#define ADS1115_ADDR    0x48
#define ADS1115_ADDR_WR (ADS1115_ADDR << 1) | 0     // becomes 0x90
#define ADS1115_ADDR_RD (ADS1115_ADDR << 1) | 1  

// --Macro Declarations---------------------------------------------------------
#define REGISTER_CONVERSION					0x00
#define REGISTER_CONFIG                     0x01
#define REGISTER_LO_THRESH					0x02
#define REGISTER_HI_THRESH					0x03

#define COMP_QUEUE_ONE_CONVERSION			0
#define COMP_QUEUE_TWO_CONVERSION			1
#define COMP_QUEUE_FOUR_CONVERSION          2
#define COMP_QUEUE_DISABLE_COMPARATOR       3

#define COMP_LAT_NONLATCHING                0
#define COMP_LAT_LATCHING					1

#define COMP_POL_ACT_HIGH					1
#define COMP_POL_ACT_LOW					0

#define COMP_MODE_TRADITIONAL               0
#define COMP_MODE_WINDOW					1

#define DR_8_SPS							0
#define DR_16_SPS							1
#define DR_32_SPS							2
#define DR_64_SPS							3
#define DR_128_SPS							4
#define DR_250_SPS							5
#define DR_475_SPS							6
#define DR_860_SPS							7

#define MODE_CONT_CONV_MODE                 0
#define MODE_SINGLE_SHOT_OR_PWR_DOWN		1

#define PGA_FSR_6_144						0
#define PGA_FSR_4_096						1
#define PGA_FSR_2_048						2
#define PGA_FSR_1_024						3
#define PGA_FSR_0_512						4
#define PGA_FSR_0_256						5		// PGA value of 5, 6 & 7 yields the same FSR

#define MUX_DIFF_0_1						0
#define MUX_DIFF_0_3						1
#define MUX_DIFF_1_3						2
#define MUX_DIFF_2_3						3
#define MUX_SINGLE_0						4
#define MUX_SINGLE_1						5
#define MUX_SINGLE_2						6
#define MUX_SINGLE_3						7

#define OS_CONVERSION_IN_PROGRESS			0
#define OS_CONVERSION_COMPLETE				1
#define OS_START_SINGLE_CONVERSION          1

#define CHANNEL_0                           0
#define CHANNEL_1                           1
#define CHANNEL_2                           2
#define CHANNEL_3                           3

#define ADS1115_REG_ADDR_SIZE               1   // no of bytes

// --Data Type Section----------------------------------------------------------

typedef union {

    struct {
        unsigned compQue : 2;
        unsigned compLat : 1;
        unsigned compPol : 1;
        unsigned compMode : 1;
        unsigned dataRate : 3;
        unsigned mode : 1;
        unsigned pga : 3;
        unsigned mux : 3;
        unsigned os : 1;
    };
    uint16_t value;
} ADS1115_ConfigBits_t;

typedef struct {
    uint8_t address;
    ADS1115_ConfigBits_t config;
    uint8_t isPresent;
} ADS1115_Object_t;

// --Global Variable Section----------------------------------------------------


// --Global Functions Section---------------------------------------------------
void ADS1115_init(ADS1115_Object_t *instance);

// Polling method
float ADS1115_readSingleEnded(ADS1115_Object_t *instance, const uint8_t channel);

// Interrupt method
void ADS1115_setReadSingleEnded(ADS1115_Object_t *instance, const uint8_t channel);
float ADS1115_getVoltage(ADS1115_Object_t *instance, const uint8_t channel);


// Local Functions
static uint16_t conversionComplete(ADS1115_Object_t *instance);
static uint16_t getLastConversion(ADS1115_Object_t *instance);
static float computeVoltage(ADS1115_Object_t *instance, const uint16_t value);

//  --Low-level Functions Section-----------------------------------------------
static void ads1115_writeRegister(ADS1115_Object_t *instance, const uint8_t reg, const uint8_t value);
static uint16_t ads1115_readRegister(ADS1115_Object_t *instance, const uint8_t reg);


#endif	/* ADS1115_H */

