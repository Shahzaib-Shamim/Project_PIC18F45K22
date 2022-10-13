/* 
 * File:   DS3231.h
 * Author: Shahzaib
 *
 * Created on September 5, 2022, 11:36 PM
 */

#ifndef DS3231_H
#define	DS3231_H

// --Includes-------------------------------------------------------------------
#include <xc.h>
#include "main.h"

// --Macro Declarations---------------------------------------------------------
#define DS3231_ADDR     0x68
#define DS3231_ADDR_WR  (DS3231_ADDR << 1) & 0xFE    // 0xD0
#define DS3231_ADDR_RD  (DS3231_ADDR << 1) | 0x01    // 0xD1

// --Registers------------------------------------------------------------------
#define SECONDS_REG         0x00
#define MINUTES_REG         0X01
#define HOURS_REG           0x02
#define DAY_OF_WEEK_REG     0x03
#define DAY_OF_MONTH_REG    0x04
#define MONTH_REG           0x05
#define YEAR_REG            0x06

//extern uint8_t rtcWrite_Buff[10];
//extern uint8_t rtcRead_Buff[10];

// --Local functions-----------------------------------------------------------
// Convert normal decimal numbers to binary coded decimal
static uint8_t decToBCD(uint8_t);

// Convert binary coded decimal to normal decimal numbers
static uint8_t bcdToDEC(uint8_t);

// Set date and time in once
static void setTime(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);

// Set date and time in once
static void setTime_2(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);

// Set date and time in once
static void setTime_3(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);

// Set seconds
static void setSeconds(uint8_t);

// Set minutes
static void setMinutes(uint8_t);

// Set hours
static void setHours(uint8_t);

// Set day of week
static void setWeekDay(uint8_t);

// Set date
static void setDate(uint8_t);

// Set month
static void setMonth(uint8_t);

// Set year
static void setYear(uint8_t);

// --Global Functions-----------------------------------------------------------
/**
 *  Set name of the month in string
 * @param char pointer
 */
char* setMonthName(uint8_t);

/**
 *  Set name of the week in string
 * @param char pointer
 */
char* setDayName(uint8_t);

/**
 * Read Time
 * @param 1st: Seconds (0-59)
 * @param 2nd: Minutes (0-59)
 * @param 3rd: Hours (24-hour format 0-23)
 */
void DS3231_setTime(uint8_t, uint8_t, uint8_t);

/**
 * Set Time
 * @param 1st: Day of the week. 1 for sunday, 7 for saturday
 * @param 2nd: Day of the month (1-31)
 * @param 3rd: Name of the month (1-12)
 * @param 4th: Year (0-99)
 */
void DS3231_setDate(uint8_t, uint8_t, uint8_t, uint8_t);

/**
 * Read Time
 * @param 1st: Seconds
 * @param 2nd: Minutes
 * @param 3rd: Hours
 */
void DS3231_getTime(uint8_t*, uint8_t*, uint8_t*);

/**
 * Read Time
 * @param 1st: Day of the week
 * @param 2nd: Day of the month
 * @param 3rd: Name of the month
 * @param 4th: Year
 */
void DS3231_getDate(uint8_t*, uint8_t*, uint8_t*, uint8_t*);

/**
 * Read time and date registers
 * @param 1st: Seconds
 * @param 2nd: Minutes
 * @param 3rd: Hours
 * @param 4th: Day of the week
 * @param 5th: Day of the month
 * @param 6th: Name of the month
 * @param 7th: Year
 */
void DS3231_getData(uint8_t*, uint8_t*, uint8_t*, uint8_t*, uint8_t*, uint8_t*, uint8_t*);

/**
 * Get one register value
 * @param register value from 00h to 06h
 */
void DS3231_getHours(uint8_t*);
void DS3231_getMinutes(uint8_t*);
void DS3231_getSeconds(uint8_t*);
void DS3231_getDayOfWeek(uint8_t*);
void DS3231_getDayOfMonth(uint8_t*);
void DS3231_getMonth(uint8_t*);
void DS3231_getYear(uint8_t*);

#endif	/* DS3231_H */

