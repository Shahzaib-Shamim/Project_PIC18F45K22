/* 
 * File:   TaskRoutines.h
 * Author: Shahzaib
 *
 * Created on August 25, 2022, 2:29 AM
 */

#ifndef TASKROUTINES_H
#define	TASKROUTINES_H

// --Includes-------------------------------------------------------------------
#include <xc.h>
#include "main.h"

// --Global variables-----------------------------------------------------------
typedef struct
{
  bool timer1_1msFlag;
  uint16_t timer1_1msCounter;
} TIMER1_t;

extern volatile TIMER1_t timer1;

// Tasks in ms
// --Task routines macros-------------------------------------------------------
#define LED_BLINK_TASK_TIME       50
#define UART_TX_TASK_TIME         1000
#define ADS1115_SAMPLE_TASK_TIME  200
#define DS3231_TIMESTAMP_TASK     900
#define EEPROM_TASK               400
#define LCD_TASK                  1000

// add task routines here
typedef struct
{
  uint8_t ledBlinkTask      : 1;
  uint8_t ads1115SampleTask : 1;
  uint8_t uartTxTask        : 1;
  uint8_t ds3231Task        : 1;
  uint8_t eepromTask        : 1;  
  uint8_t lcdTask           : 1;
} ApplicationFlags_t;

extern volatile ApplicationFlags_t sysFlags;

/**
 * Global function to initialize variables
 */
void initSysVariables(void);

/**
 * Global function to run main application code for different tasks
 */
void runApplication(void);

// --Global task routines-------------------------------------------------------
void ledBlink(void);
void ads1115SampleData(void);
void ds3232Timestamp(void);


#endif	/* TASKROUTINES_H */

