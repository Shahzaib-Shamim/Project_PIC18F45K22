#include "DS3231.h"
#include "I2C.h"

uint8_t rtcRead_Buff[8];

static uint8_t decToBCD(uint8_t value)
{
    return ((value / 10 * 16) + (value % 10));
}

static uint8_t bcdToDEC(uint8_t value)
{
    return ((value / 16 * 10) + (value % 16));
}

static void setSeconds(uint8_t second)
{
    I2C1_start(I2C_START_CMD);
    I2C1_write(DS3231_ADDR_WR);
    I2C1_write(SECONDS_REG);
    I2C1_write(decToBCD(second));
    I2C1_stop();
}

static void setMinutes(uint8_t minute)
{
    I2C1_start(I2C_START_CMD);
    I2C1_write(DS3231_ADDR_WR);
    I2C1_write(MINUTES_REG);
    I2C1_write(decToBCD(minute));
    I2C1_stop();
}

static void setHours(uint8_t hour)
{
    I2C1_start(I2C_START_CMD);
    I2C1_write(DS3231_ADDR_WR);
    I2C1_write(HOURS_REG);
    I2C1_write(decToBCD(hour));
    I2C1_stop();
}

static void setWeekDay(uint8_t dayOfWeek)
{
    I2C1_start(I2C_START_CMD);
    I2C1_write(DS3231_ADDR_WR);
    I2C1_write(DAY_OF_WEEK_REG);
    I2C1_write(decToBCD(dayOfWeek));
    I2C1_stop();
}

static void setDate(uint8_t date)
{
    I2C1_start(I2C_START_CMD);
    I2C1_write(DS3231_ADDR_WR);
    I2C1_write(DAY_OF_MONTH_REG);
    I2C1_write(decToBCD(date));
    I2C1_stop();
}

static void setMonth(uint8_t month)
{
    I2C1_start(I2C_START_CMD);
    I2C1_write(DS3231_ADDR_WR);
    I2C1_write(MONTH_REG);
    I2C1_write(decToBCD(month));
    I2C1_stop();
}

static void setYear(uint8_t year)
{
    I2C1_start(I2C_START_CMD);
    I2C1_write(DS3231_ADDR_WR);
    I2C1_write(YEAR_REG);
    I2C1_write(decToBCD(year));
    I2C1_stop();
}

char* setMonthName(uint8_t nameOfMonth)
{
    char *month;
    switch(nameOfMonth)
    {
        case 1:
            month = "Jan";
            break;
        case 2:
            month = "Feb";
            break;
        case 3:
            month = "Mar";
            break;
        case 4:
            month = "Apr";
            break;
        case 5:
            month = "May";
            break;
        case 6:
            month = "Jun";
            break;
        case 7:
            month = "Jul";
            break;
        case 8:
            month = "Aug";
            break;
        case 9:
            month = "Sep";
            break;
        case 10:
            month = "Oct";
            break;
        case 11:
            month = "Nov";
            break;
        case 12:
            month = "Dec";
            break;
    }
    return month;
}

char* setDayName(uint8_t nameOfDay)
{
    char *day;
    switch(nameOfDay)
    {
        case 1:
            day = "Sun";
            break;
        case 2:
            day = "Mon";
            break;
        case 3:
            day = "Tue";
            break;
        case 4:
            day = "Wed";
            break;
        case 5:
            day = "Thu";
            break;
        case 6:
            day = "Fri";
            break;
        case 7:
            day = "Sat";
            break;
    }
    return day;
}

void DS3231_setTime(uint8_t seconds, uint8_t minutes, uint8_t hours)
{
    if(seconds > 59) 
        seconds = 59;
    setSeconds(seconds);

    if(minutes > 59)
        minutes = 59;
    setMinutes(minutes);

    if(hours > 23)
        hours = 23;
    setHours(hours);
}

void DS3231_setDate(uint8_t dayOfWeek, uint8_t dayOfMonth, uint8_t nameOfMonth, uint8_t year)
{
    if(dayOfWeek > 7)
        dayOfWeek = 7;
    if(dayOfWeek < 1)
        dayOfWeek = 1;
    setWeekDay(dayOfWeek);

    if(dayOfMonth > 31)
        dayOfMonth = 31;
    if(dayOfMonth < 1)
        dayOfMonth = 1;
    setDate(dayOfMonth);

    if(nameOfMonth > 12)
        nameOfMonth = 12;
    if(nameOfMonth < 1)
        nameOfMonth = 1;
    setMonth(nameOfMonth);

    if(year > 99)
        year = 99;
    setYear(year);
}

void DS3231_getTime(uint8_t *second, uint8_t *minute, uint8_t *hour)
{
    I2C1_readblock(DS3231_ADDR, SECONDS_REG, 3, rtcRead_Buff); // set register pointer to 00h

    // request seven bytes of data from DS3231 starting from register 00h
    *second = bcdToDEC(rtcRead_Buff[0] & 0x7f);
    *minute = bcdToDEC(rtcRead_Buff[1]);
    *hour = bcdToDEC(rtcRead_Buff[2] & 0x3f);
}

void DS3231_getDate(uint8_t *dayOfWeek, uint8_t *dayOfMonth, uint8_t *month, uint8_t *year)
{
    I2C1_readblock(DS3231_ADDR, DAY_OF_WEEK_REG, 4, rtcRead_Buff); // set register pointer to 03h

    // request seven bytes of data from DS3231 starting from register 00h

    *dayOfWeek = bcdToDEC(rtcRead_Buff[0]);
    *dayOfMonth = bcdToDEC(rtcRead_Buff[1]);
    *month = bcdToDEC(rtcRead_Buff[2]);
    *year = bcdToDEC(rtcRead_Buff[3]);
}

void DS3231_getData(uint8_t *second, uint8_t *minute, uint8_t *hour, uint8_t *dayOfWeek,
        uint8_t *dayOfMonth, uint8_t *month, uint8_t *year)
{
    I2C1_readblock(DS3231_ADDR, SECONDS_REG, 7, rtcRead_Buff); // set register pointer to 00h

    // request seven bytes of data from DS3231 starting from register 00h
    *second = bcdToDEC(rtcRead_Buff[0] & 0x7f);
    *minute = bcdToDEC(rtcRead_Buff[1]);
    *hour = bcdToDEC(rtcRead_Buff[2] & 0x3f);
    *dayOfWeek = bcdToDEC(rtcRead_Buff[3]);
    *dayOfMonth = bcdToDEC(rtcRead_Buff[4]);
    *month = bcdToDEC(rtcRead_Buff[5]);
    *year = bcdToDEC(rtcRead_Buff[6]);
}

void DS3231_getHours(uint8_t* hours)
{
    I2C1_start(I2C_START_CMD);
    I2C1_write(DS3231_ADDR_WR);
    I2C1_write(HOURS_REG);
    I2C1_start(I2C_REP_START_CMD);
    I2C1_write(DS3231_ADDR_RD);
    *hours = I2C1_read();
    I2C1_master_ack(I2C_DATA_NOACK);
    I2C1_stop();
}

void DS3231_getMinutes(uint8_t* minutes)
{
    I2C1_start(I2C_START_CMD);
    I2C1_write(DS3231_ADDR_WR);
    I2C1_write(MINUTES_REG);
    I2C1_start(I2C_REP_START_CMD);
    I2C1_write(DS3231_ADDR_RD);
    *minutes = I2C1_read();
    I2C1_master_ack(I2C_DATA_NOACK);
    I2C1_stop();
}

void DS3231_getSeconds(uint8_t* seconds)
{
    I2C1_start(I2C_START_CMD);
    I2C1_write(DS3231_ADDR_WR);
    I2C1_write(SECONDS_REG);
    I2C1_start(I2C_REP_START_CMD);
    I2C1_write(DS3231_ADDR_RD);
    *seconds = I2C1_read();
    I2C1_master_ack(I2C_DATA_NOACK);
    I2C1_stop();
}

void DS3231_getDayOfWeek(uint8_t* dayOfWeek)
{
    I2C1_start(I2C_START_CMD);
    I2C1_write(DS3231_ADDR_WR);
    I2C1_write(DAY_OF_WEEK_REG);
    I2C1_start(I2C_REP_START_CMD);
    I2C1_write(DS3231_ADDR_RD);
    *dayOfWeek = I2C1_read();
    I2C1_master_ack(I2C_DATA_NOACK);
    I2C1_stop();
}

void DS3231_getDayOfMonth(uint8_t* dayOfMonth)
{
    I2C1_start(I2C_START_CMD);
    I2C1_write(DS3231_ADDR_WR);
    I2C1_write(DAY_OF_MONTH_REG);
    I2C1_start(I2C_REP_START_CMD);
    I2C1_write(DS3231_ADDR_RD);
    *dayOfMonth = I2C1_read();
    I2C1_master_ack(I2C_DATA_NOACK);
    I2C1_stop();
}

void DS3231_getMonth(uint8_t* month)
{
    I2C1_start(I2C_START_CMD);
    I2C1_write(DS3231_ADDR_WR);
    I2C1_write(MONTH_REG);
    I2C1_start(I2C_REP_START_CMD);
    I2C1_write(DS3231_ADDR_RD);
    *month = I2C1_read();
    I2C1_master_ack(I2C_DATA_NOACK);
    I2C1_stop();
}

void DS3231_getYear(uint8_t* year)
{
    I2C1_start(I2C_START_CMD);
    I2C1_write(DS3231_ADDR_WR);
    I2C1_write(YEAR_REG);
    I2C1_start(I2C_REP_START_CMD);
    I2C1_write(DS3231_ADDR_RD);
    *year = I2C1_read();
    I2C1_master_ack(I2C_DATA_NOACK);
    I2C1_stop();
}