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

// --Defines for serial msg input-----------------------------------------------
#define SER_MSG_FLAG_IDLE       0
#define SER_MSG_FLAG_TIME       1
#define SER_MSG_FLAG_DATE       2
#define SER_MSG_FLAG_PRES1      3
#define SER_MSG_FLAG_TEMP1      4
#define SER_MSG_FLAG_PRES2      5
#define SER_MSG_FLAG_TEMP2      6

#define charLen     10
#define numOfChar   8
#define timePacketLen   5
#define datePacketLen   6
#define NUM_OF_INT_Data    15

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

/**
 * Do parsing on the receive msg and set flags.
 */
void process_data(void);

/**
 * CRC16
 */
uint16_t checksumCalculator(uint8_t *, uint16_t);

/**
 * Functions to convert integer to ascii to send numbers over uart.
 */
void swap(char *x, char *y);
char* reverse(char *buffer, int i, int j);
char* itoa(int value, char* buffer, int numBase);


#endif	/* TASKROUTINES_H */

