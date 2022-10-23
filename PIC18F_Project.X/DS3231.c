#include "DS3231.h"
#include "I2C.h"

uint8_t rtcWrite_Buff[10];
uint8_t rtcRead_Buff[10];

static uint8_t decToBCD(uint8_t value)
{
    return((value / 10 * 16) + (value % 10));
}

static uint8_t bcdToDEC(uint8_t value)
{
    return((value / 16 * 10) + (value % 16));
}

//static void setTime(uint8_t second, uint8_t minute, uint8_t hour, uint8_t dayOfWeek,
//                   uint8_t dayOfMonth, uint8_t month, uint8_t year)
//{
//    rtcWrite_Buff[0] = decToBCD(second); // set seconds
//    rtcWrite_Buff[1] = decToBCD(minute); // set minutes
//    rtcWrite_Buff[2] = decToBCD(hour); // set hours
//    rtcWrite_Buff[3] = decToBCD(dayOfWeek); // set day (1 = sunday, 7 = saturday)
//    rtcWrite_Buff[4] = decToBCD(dayOfMonth); // set date (1 to 31)
//    rtcWrite_Buff[5] = decToBCD(month); // set month (1 to 12))
//    rtcWrite_Buff[6] = decToBCD(year); // set year (0 to 99)
//
//    I2C1_Write1ByteRegister(DS3231_ADDR, 0x00, rtcWrite_Buff[0]);
//    I2C1_Write1ByteRegister(DS3231_ADDR, 0x01, rtcWrite_Buff[1]);
//    I2C1_Write1ByteRegister(DS3231_ADDR, 0x02, rtcWrite_Buff[2]);
//    I2C1_Write1ByteRegister(DS3231_ADDR, 0x03, rtcWrite_Buff[3]);
//    I2C1_Write1ByteRegister(DS3231_ADDR, 0x04, rtcWrite_Buff[4]);
//    I2C1_Write1ByteRegister(DS3231_ADDR, 0x05, rtcWrite_Buff[5]);
//    I2C1_Write1ByteRegister(DS3231_ADDR, 0x06, rtcWrite_Buff[6]);
//}

//static void setTime_2(uint8_t second, uint8_t minute, uint8_t hour, uint8_t dayOfWeek,
//                     uint8_t dayOfMonth, uint8_t month, uint8_t year)
//{
//    rtcWrite_Buff[0] = decToBCD(second); // set seconds
//    rtcWrite_Buff[1] = decToBCD(minute); // set minutes
//    rtcWrite_Buff[2] = decToBCD(hour); // set hours
//    rtcWrite_Buff[3] = decToBCD(dayOfWeek); // set day (1 = sunday, 7 = saturday)
//    rtcWrite_Buff[4] = decToBCD(dayOfMonth); // set date (1 to 31)
//    rtcWrite_Buff[5] = decToBCD(month); // set month (1 to 12))
//    rtcWrite_Buff[6] = decToBCD(year); // set year (0 to 99)
//
//    I2C1_WriteNBytes(DS3231_ADDR, rtcWrite_Buff, 7);
//}

//static void setTime_3(uint8_t second, uint8_t minute, uint8_t hour, uint8_t dayOfWeek,
//                     uint8_t dayOfMonth, uint8_t month, uint8_t year)
//{
//    uint8_t writeDataBuff[14];
//
//    writeDataBuff[0] = 0x00;
//    writeDataBuff[1] = decToBCD(second); // set seconds
//    writeDataBuff[2] = 0x01;
//    writeDataBuff[3] = decToBCD(minute); // set minutes
//    writeDataBuff[4] = 0x02;
//    writeDataBuff[5] = decToBCD(hour); // set hour
//    writeDataBuff[6] = 0x03;
//    writeDataBuff[7] = decToBCD(dayOfWeek); // set day (1 = sunday, 7 = saturday)
//    writeDataBuff[8] = 0x04;
//    writeDataBuff[9] = decToBCD(dayOfMonth); // set date (1 to 31)
//    writeDataBuff[10] = 0x05;
//    writeDataBuff[11] = decToBCD(month); // set month (1 to 12))
//    writeDataBuff[12] = 0x06;
//    writeDataBuff[13] = decToBCD(year); // set year (0 to 99)           
//
//    I2C1_WriteNBytes(DS3231_ADDR, writeDataBuff, 14);
//}

static void setSeconds(uint8_t second)
{
//    I2C1_start();
//    I2C1_sendbyte(DS3231_ADDR_WR);
//    I2C1_sendbyte(SECONDS_REG);
//    I2C1_sendbyte(decToBCD(second));
//    I2C1_stop();
    
    I2C1_start(I2C_START_CMD);
    I2C1_write(DS3231_ADDR_WR);
    I2C1_write(SECONDS_REG);
    I2C1_write(decToBCD(second));
    I2C1_stop();
}

static void setMinutes(uint8_t minute)
{
//    I2C1_start();
//    I2C1_sendbyte(DS3231_ADDR_WR);
//    I2C1_sendbyte(MINUTES_REG);
//    I2C1_sendbyte(decToBCD(minute));
//    I2C1_stop();
    I2C1_start(I2C_START_CMD);
    I2C1_write(DS3231_ADDR_WR);
    I2C1_write(MINUTES_REG);
    I2C1_write(decToBCD(minute));
    I2C1_stop();
}

static void setHours(uint8_t hour)
{
//    I2C1_start();
//    I2C1_sendbyte(DS3231_ADDR_WR);
//    I2C1_sendbyte(HOURS_REG);
//    I2C1_sendbyte(decToBCD(hour));
//    I2C1_stop();
    
    I2C1_start(I2C_START_CMD);
    I2C1_write(DS3231_ADDR_WR);
    I2C1_write(HOURS_REG);
    I2C1_write(decToBCD(hour));
    I2C1_stop();
}

static void setWeekDay(uint8_t dayOfWeek)
{
//    I2C1_start();
//    I2C1_sendbyte(DS3231_ADDR_WR);
//    I2C1_sendbyte(DAY_OF_WEEK_REG);
//    I2C1_sendbyte(decToBCD(dayOfWeek));
//    I2C1_stop();
    I2C1_start(I2C_START_CMD);
    I2C1_write(DS3231_ADDR_WR);
    I2C1_write(DAY_OF_WEEK_REG);
    I2C1_write(decToBCD(dayOfWeek));
    I2C1_stop();
}

static void setDate(uint8_t date)
{
//    I2C1_start();
//    I2C1_sendbyte(DS3231_ADDR_WR);
//    I2C1_sendbyte(DAY_OF_MONTH_REG);
//    I2C1_sendbyte(decToBCD(date));
//    I2C1_stop();
    I2C1_start(I2C_START_CMD);
    I2C1_write(DS3231_ADDR_WR);
    I2C1_write(DAY_OF_MONTH_REG);
    I2C1_write(decToBCD(date));
    I2C1_stop();
}

static void setMonth(uint8_t month)
{
//    I2C1_start();
//    I2C1_sendbyte(DS3231_ADDR_WR);
//    I2C1_sendbyte(MONTH_REG);
//    I2C1_sendbyte(decToBCD(month));
//    I2C1_stop();
    
    I2C1_start(I2C_START_CMD);
    I2C1_write(DS3231_ADDR_WR);
    I2C1_write(MONTH_REG);
    I2C1_write(decToBCD(month));
    I2C1_stop();
}

static void setYear(uint8_t year)
{
//    I2C1_start();
//    I2C1_sendbyte(DS3231_ADDR_WR);
//    I2C1_sendbyte(YEAR_REG);
//    I2C1_sendbyte(decToBCD(year));
//    I2C1_stop();
    
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
    if(seconds > 59) seconds = 59;
//    if(seconds < 0) seconds = 0;
    setSeconds(seconds);
    
    if(minutes > 59) minutes = 59;
//    if(minutes < 0) minutes = 0;
    setMinutes(minutes);
    
    if(hours > 23) hours = 23;
//    if(hours < 0) hours = 0;
    setHours(hours);
}

void DS3231_setDate(uint8_t dayOfWeek, uint8_t dayOfMonth, uint8_t nameOfMonth, uint8_t year)
{
    if(dayOfWeek > 7) dayOfWeek = 7;
    if(dayOfWeek < 1) dayOfWeek = 1;
    setWeekDay(dayOfWeek);
    
    if(dayOfMonth > 31) dayOfMonth = 31;
    if(dayOfMonth < 1) dayOfMonth = 1;
    setDate(dayOfMonth);
    
    if(nameOfMonth > 12) nameOfMonth = 12;
    if(nameOfMonth < 1) nameOfMonth = 1;
    setMonth(nameOfMonth);
    
    if(year > 99) year = 99;
//    if(year < 0) year = 0;
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
//    I2C1_start();
//    I2C1_sendbyte(DS3231_ADDR_WR);
//    I2C1_sendbyte(HOURS_REG);
//    I2C1_restart();
//    I2C1_sendbyte(DS3231_ADDR_RD);
//    *hours = I2C1_recvbyte(1);      //NACK
//    I2C1_stop();
    
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
//    I2C1_start();
//    I2C1_sendbyte(DS3231_ADDR_WR);
//    I2C1_sendbyte(MINUTES_REG);
//    I2C1_restart();
//    I2C1_sendbyte(DS3231_ADDR_RD);
//    *minutes = I2C1_recvbyte(1);      //NACK
//    I2C1_stop();
    
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
//    I2C1_start();
//    I2C1_sendbyte(DS3231_ADDR_WR);
//    I2C1_sendbyte(SECONDS_REG);
//    I2C1_restart();
//    I2C1_sendbyte(DS3231_ADDR_RD);
//    *seconds = I2C1_recvbyte(1);      //NACK
//    I2C1_stop();
    
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
//    I2C1_start();
//    I2C1_sendbyte(DS3231_ADDR_WR);
//    I2C1_sendbyte(DAY_OF_WEEK_REG);
//    I2C1_restart();
//    I2C1_sendbyte(DS3231_ADDR_RD);
//    *dayOfWeek = I2C1_recvbyte(1);      //NACK
//    I2C1_stop();
    
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
//    I2C1_start();
//    I2C1_sendbyte(DS3231_ADDR_WR);
//    I2C1_sendbyte(DAY_OF_MONTH_REG);
//    I2C1_restart();
//    I2C1_sendbyte(DS3231_ADDR_RD);
//    *dayOfMonth = I2C1_recvbyte(1);      //NACK
//    I2C1_stop();
    
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
//    I2C1_start();
//    I2C1_sendbyte(DS3231_ADDR_WR);
//    I2C1_sendbyte(MONTH_REG);
//    I2C1_restart();
//    I2C1_sendbyte(DS3231_ADDR_RD);
//    *month = I2C1_recvbyte(1); //NACK
//    I2C1_stop();
    
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
//    I2C1_start();
//    I2C1_sendbyte(DS3231_ADDR_WR);
//    I2C1_sendbyte(YEAR_REG);
//    I2C1_restart();
//    I2C1_sendbyte(DS3231_ADDR_RD);
//    *year = I2C1_recvbyte(1); //NACK
//    I2C1_stop();
    
    I2C1_start(I2C_START_CMD);
    I2C1_write(DS3231_ADDR_WR);
    I2C1_write(YEAR_REG);
    I2C1_start(I2C_REP_START_CMD);
    I2C1_write(DS3231_ADDR_RD);
    *year = I2C1_read();
    I2C1_master_ack(I2C_DATA_NOACK);
    I2C1_stop();
}