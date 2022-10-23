#include "TaskRoutines.h"
#include "GPIO.h"
#include "UART.h"
#include "ADS1115.h"
#include "DS3231.h"
#include "EEPROM.h"
#include "LCD_HD44780_20x4.h"

volatile TIMER1_t timer1 = {
    .timer1_1msFlag = false,
    .timer1_1msCounter = 0
};

ADS1115_Object_t myADC;
char txBuffer[50] = {0}; //uint8_t
uint8_t readBuff[32];

volatile ApplicationFlags_t sysFlags;

bool ads1115Flag = false;
bool ds3231Flag = false;

uint8_t serialMsgFlags = SER_MSG_FLAG_IDLE;
uint8_t serialMsgErrorFlag = false;
char parsedStrings[charLen][numOfChar];
uint8_t dataPosition = 0;
uint8_t dataCount = 0;
const char delimiter[] = ",";
char *keywordPointer;
uint8_t integerData[NUM_OF_INT_Data];
float floatData[NUM_OF_FLOAT_DATA];
char *keyword[5]; // store starting 3 bytes of data to detect which serial msg is received
bool dateFlag = false;
bool timeFlag = false;
bool pres1Flag = false;
bool pres2Flag = false;
bool temp1Flag = false;
bool temp2Flag = false;
bool txEnableFlag = true; // false
bool eepromFlag = false;
uint8_t eepromStateFlags = 0;
bool calibrationEnableFlag = true; //false
float pres1Factor = 1.0f;
float pres2Factor = 1.0f;
float temp1Factor = 1.0f;
float temp2Factor = 1.0f;
float voltage[4];
uint8_t ds3231_readBuff[7];

float coefA_P1 = 1.0f;
float coefB_P1 = 1.0f;
float coefC_P1 = 1.0f;
float coefD_P1 = 1.0f;
float coefA_T1 = 1.0f;
float coefB_T1 = 1.0f;
float coefC_T1 = 1.0f;
float coefD_T1 = 1.0f;
float coefA_P2 = 1.0f;
float coefB_P2 = 1.0f;
float coefC_P2 = 1.0f;
float coefD_P2 = 1.0f;
float coefA_T2 = 1.0f;
float coefB_T2 = 1.0f;
float coefC_T2 = 1.0f;
float coefD_T2 = 1.0f;

void initSysVariables(void)
{
    sysFlags.ledBlinkTask = false;
    sysFlags.uartTxTask = false;
    sysFlags.ads1115SampleTask = false;
    sysFlags.ds3231Task = false;
    sysFlags.eepromTask = false;
    sysFlags.lcdTask = false;

    // ADS1115 init (disabled for testing)
    // ADS1115 configuration
    myADC.address = ADS1115_ADDR;
    myADC.config.value = 0; // reset
    myADC.config.mux = MUX_SINGLE_0;
    myADC.config.pga = PGA_FSR_6_144;
    myADC.config.mode = MODE_SINGLE_SHOT_OR_PWR_DOWN;
    myADC.config.dataRate = DR_128_SPS;
    myADC.config.compMode = COMP_MODE_TRADITIONAL;
    myADC.config.compPol = COMP_POL_ACT_LOW;
    myADC.config.compLat = COMP_LAT_NONLATCHING;
    myADC.config.compQue = COMP_QUEUE_DISABLE_COMPARATOR;
    ADS1115_init(&myADC);

    //    if(myADC.isPresent == 1)
    //    {
    //        sprintf((char *) txBuffer, "ADS1115 Found\r\n");
    //        uart1_puts((uint8_t *) txBuffer);
    //    }
    //    if(myADC.isPresent == 0)
    //    {
    //        sprintf((char *) txBuffer, "ADS1115 Not found\r\n");
    //        uart1_puts((uint8_t *) txBuffer);
    //    }
}

//char str[20];
int counterIndex = 0;

void runApplication(void)
{
    //    if(sysFlags.ledBlinkTask == true)
    //    {
    //        sysFlags.ledBlinkTask = false;
    //        ledBlink();
    //    }

    if(sysFlags.uartTxTask == true && txEnableFlag == true)
    {
        static uint8_t counter = 0;
        sysFlags.uartTxTask = false;
        //        ds3232Timestamp();        // tested
        //        ads1115SampleData();
    }

    if(sysFlags.ads1115SampleTask == true && txEnableFlag == true)
    {
        sysFlags.ads1115SampleTask = false;
        //        ads1115SampleData();
    }

    //    if(sysFlags.ds3231Task == true && serialMsgFlags == txEnableFlag == true)
    if(sysFlags.ds3231Task == true && txEnableFlag == true)
    {
        sysFlags.ds3231Task = false;
        //        ds3232Timestamp();
    }

    if(sysFlags.eepromTask == true)
    {
        sysFlags.eepromTask = false;

        // added
        //        EEPROM_readMultipleBytes(EEPROM_ADDR, 0, 0, readBuff, 32);
        //        sprintf((char *) txBuffer, "%u %u %u %u\n\r",
        //                readBuff[0], readBuff[1], readBuff[2], readBuff[3]);
        //        EUSART1_send_string((uint8_t *) txBuffer);
    }

    if(sysFlags.lcdTask == true)
    {
        sysFlags.lcdTask = false;

        //        LCDsetCursor(0, 0);
        //        LCD_Write_Str("Counter = ");
        //        LCDsetCursor(0, 1);
        ////        sprintf(str, "%lu", counterIndex);
        //        ltoa(counterIndex, txBuffer, 10);
        //        counterIndex++;
        //        LCD_Write_Str(txBuffer);

        //        int adcData[4];
        //        
        //        LCDclear();
        //        LCDsetCursor(0, 0);
        //        LCD_Write_Str("Pulsetime=");
        //                
        ////        LCDclear();
        //        LCDsetCursor(0, 1);
        //        LCD_Write_Str("P1=");
        //        adcData[0] = (voltage[0] * 1000) * pres1Factor;
        //        itoa(adcData[0], txBuffer, 10);
        //        LCDsetCursor(3, 1);
        //        LCD_Write_Str(txBuffer);
        //        
        ////        LCDclear();
        //        LCDsetCursor(0, 2);
        //        LCD_Write_Str("T1=");
        //        adcData[1] = (voltage[1] * 1000) * temp1Factor;
        //        itoa(adcData[1], txBuffer, 10);
        //        LCDsetCursor(3, 2);
        //        LCD_Write_Str(txBuffer);
        //        
        ////        LCDclear();
        //        LCDsetCursor(11, 1);
        //        LCD_Write_Str("P2=");
        //        adcData[2] = (voltage[2] * 1000) * pres2Factor;
        //        itoa(adcData[2], txBuffer, 10);
        //        LCDsetCursor(14, 1);
        //        LCD_Write_Str(txBuffer);
        //        
        ////        LCDclear();
        //        LCDsetCursor(11, 2);
        //        LCD_Write_Str("T2=");
        //        adcData[3] = (voltage[3] * 1000) * temp2Factor;
        //        itoa(adcData[3], txBuffer, 10);
        //        LCDsetCursor(14, 2);
        //        LCD_Write_Str(txBuffer);
        //        
        //        // time write
        //        LCDsetCursor(0, 3);
        //        itoa(ds3231_readBuff[2], txBuffer, 10); LCD_Write_Str(txBuffer);
        //        LCDsetCursor(2, 3); LCD_Write_Str(":");
        //        LCDsetCursor(3, 3);
        //        itoa(ds3231_readBuff[1], txBuffer, 10); LCD_Write_Str(txBuffer);
        //        LCD_Write_Str(":");
        //        itoa(ds3231_readBuff[0], txBuffer, 10); LCD_Write_Str(txBuffer);
        //    
        //        // date write
        //        LCDsetCursor(9, 3);
        //        itoa(ds3231_readBuff[4], txBuffer, 10); LCD_Write_Str(txBuffer);
        //        LCDsetCursor(11, 3); LCD_Write_Str("-");
        //        LCDsetCursor(12, 3);
        //        char *MonthName = "";
        //        MonthName = setDayName(ds3231_readBuff[3]);
        //        strncpy(txBuffer, MonthName, 3);
        //        /*itoa(ds3231_readBuff[5], txBuffer, 10);*/ LCD_Write_Str(txBuffer);
        //        LCDsetCursor(15, 3); LCD_Write_Str("-");
        //        LCDsetCursor(16, 3);
        //        itoa(ds3231_readBuff[6], txBuffer, 10); LCD_Write_Str(txBuffer);
    }

    // --Serial Msg process-----------------------------------------------------
    if(uartProcessFlag == true)
    {
        uartProcessFlag = false;

        // Time calibration
        //        if(input_line[0] == 'C' && input_line[1] == '0' && input_line[2] == '0')
        if((strncmp(input_line, "C00", 3) == 0))
        {
            serialMsgFlags = SER_MSG_FLAG_TIME;
        }

            // Date calibration
        else if(strncmp(input_line, "C01", 3) == 0)
        {
            serialMsgFlags = SER_MSG_FLAG_DATE;
        }

            // Pressure1 calibration coefA
        else if(strncmp(input_line, "C02A", 4) == 0)
        {
            serialMsgFlags = SER_MSG_FLAG_CAL_COEF;
            keyword[2] = "C02A";
        }
            // Pressure1 calibration coefB
        else if(strncmp(input_line, "C02B", 4) == 0)
        {
            serialMsgFlags = SER_MSG_FLAG_CAL_COEF;
            keyword[2] = "C02B";
        }
            // Pressure1 calibration coefA
        else if(strncmp(input_line, "C02C", 4) == 0)
        {
            serialMsgFlags = SER_MSG_FLAG_CAL_COEF;
            keyword[2] = "C02C";
        }
            // Pressure1 calibration coefD
        else if(strncmp(input_line, "C02D", 4) == 0)
        {
            serialMsgFlags = SER_MSG_FLAG_CAL_COEF;
            keyword[2] = "C02D";
        }

            // Temperature1 calibration coefA
        else if(strncmp(input_line, "C03A", 4) == 0)
        {
            serialMsgFlags = SER_MSG_FLAG_CAL_COEF;
            keyword[2] = "C03A";
        }
            // Temperature1 calibration coefB
        else if(strncmp(input_line, "C03B", 4) == 0)
        {
            serialMsgFlags = SER_MSG_FLAG_CAL_COEF;
            keyword[2] = "C03B";
        }
            // Temperature1 calibration coefC
        else if(strncmp(input_line, "C03C", 4) == 0)
        {
            serialMsgFlags = SER_MSG_FLAG_CAL_COEF;
            keyword[2] = "C03C";
        }
            // Temperature1 calibration coefD
        else if(strncmp(input_line, "C03D", 4) == 0)
        {
            serialMsgFlags = SER_MSG_FLAG_CAL_COEF;
            keyword[2] = "C03D";
        }

            // Pressure2 calibration coefA
        else if(strncmp(input_line, "C04A", 4) == 0)
        {
            serialMsgFlags = SER_MSG_FLAG_CAL_COEF;
            keyword[2] = "C04A";
        }
            // Pressure2 calibration coefB
        else if(strncmp(input_line, "C04B", 4) == 0)
        {
            serialMsgFlags = SER_MSG_FLAG_CAL_COEF;
            keyword[2] = "C04B";
        }
            // Pressure2 calibration coefC
        else if(strncmp(input_line, "C04C", 4) == 0)
        {
            serialMsgFlags = SER_MSG_FLAG_CAL_COEF;
            keyword[2] = "C04C";
        }
            // Pressure1 calibration coefD
        else if(strncmp(input_line, "C04D", 4) == 0)
        {
            serialMsgFlags = SER_MSG_FLAG_CAL_COEF;
            keyword[2] = "C04D";
        }

            // Temperature2 calibration coefA
        else if(strncmp(input_line, "C05A", 4) == 0)
        {
            serialMsgFlags = SER_MSG_FLAG_CAL_COEF;
            keyword[2] = "C05A";
        }
            // Temperature1 calibration coefB
        else if(strncmp(input_line, "C05B", 4) == 0)
        {
            serialMsgFlags = SER_MSG_FLAG_CAL_COEF;
            keyword[2] = "C05B";
        }
            // Temperature1 calibration coefC
        else if(strncmp(input_line, "C05C", 4) == 0)
        {
            serialMsgFlags = SER_MSG_FLAG_CAL_COEF;
            keyword[2] = "C05C";
        }
            // Temperature1 calibration coefD
        else if(strncmp(input_line, "C05D", 4) == 0)
        {
            serialMsgFlags = SER_MSG_FLAG_CAL_COEF;
            keyword[2] = "C05D";
        }

            // Pressure2 calibration
            /*else if(strncmp(input_line, "C04", 3) == 0)
            {
                serialMsgFlags = SER_MSG_FLAG_PRES2;
            }
        
            // Temperature2 calibration
            else if(strncmp(input_line, "C05", 3) == 0)
            {
                serialMsgFlags = SER_MSG_FLAG_TEMP2;
            }*/

            // Calibration mode enable
        else if(strncmp(input_line, "C06CalibrationY", 15) == 0) //C06
        {
            uart1_puts("Calibration mode enabled\n");
            calibrationEnableFlag = true;
        }

            // Calibration mode disable
        else if(strncmp(input_line, "C07CalibrationN", 15) == 0)
        {
            uart1_puts("Calibration mode disabled\n");
            calibrationEnableFlag = false;
        }

            // Start sending data
        else if(strncmp(input_line, "C08StartTX", 10) == 0)
        {
            txEnableFlag = true;
        }

            // Stop sending data
        else if(strncmp(input_line, "C09StopTX", 9) == 0)
        {
            txEnableFlag = false;
        }

            // Write 1 byte to EEPROM
        else if(strncmp(input_line, "C10", 3) == 0)
        {
            serialMsgFlags = SER_MSG_FLAG_EEPROMWr1;
        }

            // Read 1 byte from EEPROM
        else if(strncmp(input_line, "C11", 3) == 0)
        {
            serialMsgFlags = SER_MSG_FLAG_EEPROMRd1;
        }

            // Nothing to be process
        else
        {
            serialMsgFlags = SER_MSG_FLAG_IDLE;
        }

        if(calibrationEnableFlag == true)
        {
            process_data();
        }
    }
}

void ledBlink(void)
{
    LED_Toggle();
}

void ads1115SampleData(void)
{
    //    float voltage[4];
    //    static uint8_t count = 0;

    if(myADC.isPresent == 1)
    {

        for(uint8_t i = 0; i < 4; i++)
        {
            voltage[i] = ADS1115_readSingleEnded(&myADC, i);
            //            __delay_us(10);
        }

        /*sprintf(txBuffer, "\nADC# %d,  %0.3f  %0.3f  %0.3f  %0.3f\r\n\n",
                count, voltage[0] + pres1Factor, voltage[1] + temp1Factor, voltage[2], voltage[3]);
        uart1_puts(txBuffer);
        count++;
        uart1_newline();*/

        int adcData[4];
        float calArr[4] = {pres1Factor, temp1Factor, pres2Factor, temp2Factor};
        for(uint8_t i = 0; i < 4; i++)
        {
            adcData[i] = (voltage[i] * 1000) * calArr[i];
            itoa(adcData[i], txBuffer, 10);
            uart1_puts(txBuffer);
            uart1_puts(" ");
        }
        uart1_newline();


    }
}

void ds3232Timestamp(void)
{
    char *Day = "";
    char *MonthName = "";
    //    uint8_t ds3231_readBuff[7];
    DS3231_getTime(&ds3231_readBuff[0], &ds3231_readBuff[1], &ds3231_readBuff[2]);
    DS3231_getDate(&ds3231_readBuff[3], &ds3231_readBuff[4], &ds3231_readBuff[5], &ds3231_readBuff[6]);
    Day = setDayName(ds3231_readBuff[3]);
    MonthName = setMonthName(ds3231_readBuff[5]);

    //    sprintf(txBuffer, "\n%d:%d:%d %s %d-%s-%d\r\n\n", ds3231_readBuff[2], ds3231_readBuff[1], ds3231_readBuff[0], Day, ds3231_readBuff[4], MonthName, ds3231_readBuff[6]);
    //    uart1_puts(txBuffer);

    itoa(ds3231_readBuff[2], txBuffer, 10);
    uart1_puts(txBuffer);
    uart1_putc(':');
    itoa(ds3231_readBuff[1], txBuffer, 10);
    uart1_puts(txBuffer);
    uart1_putc(':');
    itoa(ds3231_readBuff[0], txBuffer, 10);
    uart1_puts(txBuffer);
    uart1_putc(' ');
    uart1_puts(Day);
    uart1_putc(',');
    itoa(ds3231_readBuff[4], txBuffer, 10);
    uart1_puts(txBuffer);
    uart1_putc('/');
    uart1_puts(MonthName);
    uart1_putc('/');
    itoa(ds3231_readBuff[6], txBuffer, 10);
    uart1_puts(txBuffer);
    uart1_newline();

    if(timeFlag == true)
    {
        timeFlag = false;
        DS3231_setTime(integerData[2], integerData[1], integerData[0]);
    }

    if(dateFlag == true)
    {
        dateFlag = false;
        DS3231_setDate(integerData[0], integerData[1], integerData[2], integerData[3]);
    }

    ////        LCDclear();
    //        LCDsetCursor(0, 1);
    //        LCD_Write_Str("P1=");
    //        adcData[0] = voltage[0] * 1000;
    //        itoa(adcData[0], txBuffer, 10);
    //        LCDsetCursor(3, 1);
    //        LCD_Write_Str(txBuffer);  


    //    uart1_puts(txBuffer); uart1_putc(' ');
    //    itoa(ds3231_readBuff[1], txBuffer, 10);
    //    uart1_puts(txBuffer); uart1_putc(' ');
    //    itoa(ds3231_readBuff[0], txBuffer, 10);
    //    uart1_puts(txBuffer); uart1_putc(' ');
    //    uart1_puts(Day); uart1_putc(' ');
    //    itoa(ds3231_readBuff[4], txBuffer, 10);
    //    uart1_puts(txBuffer); uart1_putc(' ');
    //    uart1_puts(MonthName); uart1_putc(' ');
    //    itoa(ds3231_readBuff[6], txBuffer, 10);
    //    uart1_puts(txBuffer); uart1_putc(' ');
    //    uart1_newline();


    //    LCDsetCursor(0, 3);
    //    // time write
    //    itoa(ds3231_readBuff[2], txBuffer, 10); LCD_Write_Str(txBuffer);
    //    LCDsetCursor(2, 3); LCD_Write_Str(":");
    //    LCDsetCursor(3, 3);
    //    itoa(ds3231_readBuff[1], txBuffer, 10); LCD_Write_Str(txBuffer);
    //    LCD_Write_Str(":");
    //    itoa(ds3231_readBuff[0], txBuffer, 10); LCD_Write_Str(txBuffer);
    //    
    //    LCDsetCursor(9, 3);
    //    // date write
    //    itoa(ds3231_readBuff[4], txBuffer, 10); LCD_Write_Str(txBuffer);
    //    LCDsetCursor(11, 3); LCD_Write_Str("-");
    //    LCDsetCursor(12, 3);
    //    itoa(ds3231_readBuff[5], txBuffer, 10); LCD_Write_Str(txBuffer);
    //    LCDsetCursor(15, 3); LCD_Write_Str("-");
    //    LCDsetCursor(16, 3);
    //    itoa(ds3231_readBuff[6], txBuffer, 10); LCD_Write_Str(txBuffer);
}

void process_data(void)
{
    uint16_t crc = 0;
    uint8_t crc1, crc2; // crc1 = highbyte, crc2 = lowbyte

    // <editor-fold defaultstate="collapsed" desc="Time calibration task">
    if(serialMsgFlags == SER_MSG_FLAG_TIME)
    {
        keyword[0] = "C00";
        keywordPointer = strstr(input_line, keyword[0]);
        if(keywordPointer != NULL)
        {
            dataPosition = (keywordPointer - input_line) + strlen(keyword[0]);
            dataCount = 0;

            char *token = strtok(&input_line[dataPosition], delimiter);
            if(token != NULL && strlen(token) < sizeof (parsedStrings[0]))
            {
                strncpy(parsedStrings[0], token, sizeof (parsedStrings[0]));
                dataCount++;
            }
            else
            {
                // uart_puts("token to big1\n");
                strcpy(parsedStrings[0], NULL);
            }

            for(uint8_t i = 1; i < timePacketLen; i++) // +2
            {
                token = strtok(NULL, delimiter);
                if(token != NULL && strlen(token) < sizeof (parsedStrings[i]))
                {
                    strncpy(parsedStrings[i], token, sizeof (parsedStrings[i]));
                    dataCount++;
                }
                else
                {
                    // uart_puts("token to big2\n");
                    strcpy(parsedStrings[i], NULL);
                }
            }

            // convert ascii data into integer data           
            if(dataCount == timePacketLen)
            {
                integerData[0] = (uint8_t) (atoi(parsedStrings[0])); // hour
                integerData[1] = (uint8_t) (atoi(parsedStrings[1])); // minute
                integerData[2] = (uint8_t) (atoi(parsedStrings[2])); // second
                integerData[3] = (uint8_t) (atoi(parsedStrings[3])); // CRC1
                integerData[4] = (uint8_t) (atoi(parsedStrings[4])); // CRC2
                for(uint8_t i = 5; i < NUM_OF_INT_Data; i++)
                {
                    integerData[i] = 0;
                }
                crc = checksumCalculator(integerData, 3);
                crc1 = (crc >> 8) & 0xff; // high byte
                crc2 = (crc >> 0) & 0xff; // low byte
                dataCount = 0;
                /*sprintf(txBuffer, "%d:%d:%d  %d  %d\n", integerData[0], integerData[1],
                        integerData[2], integerData[3], integerData[4]);
                uart1_puts(txBuffer);
                uart1_puts("--Time calib-----------------------------------------\n");*/
            }
            else if(dataCount < timePacketLen || dataCount > timePacketLen)
            {
                serialMsgErrorFlag = true; // uart1_puts("data no good\n");
                dataCount = 0;
                serialMsgFlags = 0;
            }
        }
        else
        {
            // uart1_puts("sorry no keyword\n");
            // dataCountTime = 0;
            // serialMsgFlags = 0;
        }

        if(crc1 == integerData[3] && crc2 == integerData[4])
        {
            timeFlag = true;
            ds3232Timestamp();
        }
    }
    // </editor-fold>

    // <editor-fold defaultstate="collapsed" desc="Date calibration task">
    if(serialMsgFlags == SER_MSG_FLAG_DATE)
    {
        keyword[1] = "C01";
        keywordPointer = strstr(input_line, keyword[1]);
        if(keywordPointer != NULL)
        {
            dataPosition = (keywordPointer - input_line) + strlen(keyword[1]);
            dataCount = 0;
            char *token = strtok(&input_line[dataPosition], delimiter);
            if(token != NULL && strlen(token) < sizeof (parsedStrings[0]))
            {
                strncpy(parsedStrings[0], token, sizeof (parsedStrings[0]));
                dataCount++;
            }
            else
            {
                // uart1_puts("token to big\n");
                strcpy(parsedStrings[0], NULL);
            }

            for(uint8_t i = 1; i < datePacketLen; i++)
            {
                token = strtok(NULL, delimiter);
                if(token != NULL && strlen(token) < sizeof (parsedStrings[i]))
                {
                    strncpy(parsedStrings[i], token, sizeof (parsedStrings[i]));
                    dataCount++;
                }
                else
                {
                    // uart1_puts("token to big\n");
                    strcpy(parsedStrings[i], NULL);
                }
            }

            // convert ascii data into integer data           
            if(dataCount == datePacketLen)
            {
                integerData[0] = (uint8_t) (atoi(parsedStrings[0])); // day
                integerData[1] = (uint8_t) (atoi(parsedStrings[1])); // date
                integerData[2] = (uint8_t) (atoi(parsedStrings[2])); // month
                integerData[3] = (uint8_t) (atoi(parsedStrings[3])); // year
                integerData[4] = (uint8_t) (atoi(parsedStrings[4])); // CRC1
                integerData[5] = (uint8_t) (atoi(parsedStrings[5])); // CRC2
                for(uint8_t i = 6; i < NUM_OF_INT_Data; i++)
                {
                    integerData[i] = 0;
                }
                crc = checksumCalculator(integerData, 4);
                crc1 = (crc >> 8) & 0xff; // high byte
                crc2 = (crc >> 0) & 0xff; // low byte
                dataCount = 0;
                /*sprintf(txBuffer, "%d, %d-%d-%d  %d %d\n", integerData[0], integerData[1], 
                        integerData[2], integerData[3], integerData[4], integerData[5]);
                uart1_puts(txBuffer);
                uart1_puts("--Date calib-----------------------------------------\n");*/
            }
            else if(dataCount < datePacketLen || dataCount > datePacketLen)
            {
                serialMsgErrorFlag = true; // uart1_puts("data no good\n");
                dataCount = 0;
                serialMsgFlags = 0;
            }
        }
        else
        {
            // uart1_puts("sorry no keyword\n");
            // dataCount = 0;
            // serialMsgFlags = 0;
        }
        if(crc1 == integerData[4] && crc2 == integerData[5])
        {
            dateFlag = true;
            ds3232Timestamp();
        }
    }
    // </editor-fold>

    // <editor-fold defaultstate="collapsed" desc="Pressure1 calibration task">
    //    if(serialMsgFlags == SER_MSG_FLAG_PRES1)
    //    {
    //        keyword[2] = "C02";
    //        keywordPointer = strstr(input_line, keyword[2]);
    //        if(keywordPointer != NULL)
    //        {
    //            dataPosition = (keywordPointer - input_line) + strlen(keyword[2]);
    //            dataCount = 0;
    //            char *token = strtok(&input_line[dataPosition], delimiter);
    //            if(token != NULL && strlen(token) < sizeof (parsedStrings[0]))
    //            {
    //                strncpy(parsedStrings[0], token, sizeof (parsedStrings[0]));
    //                dataCount++;
    //            }
    //            else
    //            {
    //                // uart1_puts("token to big\n");
    //                strcpy(parsedStrings[0], NULL);
    //            }
    //
    //            for(uint8_t i = 1; i < pres1PacketLen; i++)
    //            {
    //                token = strtok(NULL, delimiter);
    //                if(token != NULL && strlen(token) < sizeof (parsedStrings[i]))
    //                {
    //                    strncpy(parsedStrings[i], token, sizeof (parsedStrings[i]));
    //                    dataCount++;
    //                }
    //                else
    //                {
    //                    // uart1_puts("token to big\n");
    //                    strcpy(parsedStrings[i], NULL);
    //                }
    //            }
    //
    //            // convert ascii data into float data           
    //            if(dataCount == pres1PacketLen)
    //            {
    //                floatData[ = atof(parsedStrings[0]);      // pres1 value
    //                integerData[0] = (uint8_t)(atoi(parsedStrings[1]));    // CRC1
    //                integerData[1] = (uint8_t)(atoi(parsedStrings[2]));    // CRC2
    //                for(uint8_t i = 2; i < NUM_OF_INT_Data; i++)
    //                {
    //                    integerData[i] = 0; 
    //                }
    ////                crc = checksumCalculator(integerData, 4);
    ////                crc1 = (crc >> 8) & 0xff;         // high byte
    ////                crc2 = (crc >> 0) & 0xff;          // low byte
    //                dataCount = 0;
    //                /*sprintf(txBuffer, "%d, %d-%d-%d  %d %d\n", integerData[0], integerData[1], 
    //                        integerData[2], integerData[3], integerData[4], integerData[5]);
    //                uart1_puts(txBuffer);
    //                uart1_puts("--Date calib-----------------------------------------\n");*/
    //                
    ////                sprintf(txBuffer, "Float data = %.3f\n", floatData[0]);
    //                unsigned long floatToLong = floatData[0] * 1000;
    //                ltoa(floatToLong, txBuffer, 10);
    //                uart1_puts("Float = ");
    //                uart1_puts(txBuffer);
    //                uart1_newline();
    //            }
    //            else if(dataCount < pres1PacketLen || dataCount > pres1PacketLen)
    //            {
    //                serialMsgErrorFlag = true;  // uart1_puts("data no good\n");
    //                dataCount = 0;
    //                serialMsgFlags = 0;
    //            }
    //        }        
    //        else
    //        {
    //            // uart1_puts("sorry no keyword\n");
    //            // dataCount = 0;
    //            // serialMsgFlags = 0;
    //        }
    //        // if(crc1 == integerData[3] && crc2 == integerData[4])  // below line will replaced by this line
    //        if(integerData[0] == 1 && integerData[1] == 1)      // for now, not checking crc
    //        {
    //            pres1Flag = true;
    //            uart1_puts("Pres1 cal.\n");
    //            pres1Factor = floatData[0];
    //        }        
    //    } 
    // </editor-fold>

    // <editor-fold defaultstate="collapsed" desc="Calibration coefficients task">
    if(serialMsgFlags == SER_MSG_FLAG_CAL_COEF)
    {
        keywordPointer = strstr(input_line, keyword[2]);
        if(keywordPointer != NULL)
        {
            dataPosition = (keywordPointer - input_line) + strlen(keyword[2]);
            dataCount = 0;
            char *token = strtok(&input_line[dataPosition], delimiter);
            if(token != NULL && strlen(token) < sizeof (parsedStrings[0]))
            {
                strncpy(parsedStrings[0], token, sizeof (parsedStrings[0]));
                dataCount++;
            }
            else
            {
                // uart1_puts("token to big\n");
                strcpy(parsedStrings[0], NULL);
            }

            for(uint8_t i = 1; i < floatDataPacketLen; i++)
            {
                token = strtok(NULL, delimiter);
                if(token != NULL && strlen(token) < sizeof (parsedStrings[i]))
                {
                    strncpy(parsedStrings[i], token, sizeof (parsedStrings[i]));
                    dataCount++;
                }
                else
                {
                    // uart1_puts("token to big\n");
                    strcpy(parsedStrings[i], NULL);
                }
            }

            // convert ascii data into float data           
            if(dataCount == floatDataPacketLen)
            {
                integerData[0] = (uint8_t) (atoi(parsedStrings[0])); // byte1
                integerData[1] = (uint8_t) (atoi(parsedStrings[1])); // byte2
                integerData[2] = (uint8_t) (atoi(parsedStrings[2])); // byte3
                integerData[3] = (uint8_t) (atoi(parsedStrings[3])); // byte4
                integerData[4] = (uint8_t) (atoi(parsedStrings[4])); // CRC1
                integerData[5] = (uint8_t) (atoi(parsedStrings[5])); // CRC2
                for(uint8_t i = 6; i < NUM_OF_INT_Data; i++)
                {
                    integerData[i] = 0;
                }
                //                crc = checksumCalculator(integerData, 4);
                //                crc1 = (crc >> 8) & 0xff;         // high byte
                //                crc2 = (crc >> 0) & 0xff;          // low byte
                dataCount = 0;
                /*sprintf(txBuffer, "%d, %d-%d-%d  %d %d\n", integerData[0], integerData[1], 
                        integerData[2], integerData[3], integerData[4], integerData[5]);
                uart1_puts(txBuffer);
                uart1_puts("--Date calib-----------------------------------------\n");*/

                //                sprintf(txBuffer, "Float data = %.3f\n", floatData[0]);
                //                unsigned long floatToLong = floatData[0] * 1000;
                //                ltoa(floatToLong, txBuffer, 10);
                //                uart1_puts("Float = ");
                //                uart1_puts(txBuffer);
                //                uart1_newline();

                unsigned long floatToLong;
                floatData[0] = EEPROM_byteToFloat(integerData); // coef byte to float conversion
                // if(crc1 == integerData[3] && crc2 == integerData[4])  // below line will replaced by this line 
                if(integerData[4] == 1 && integerData[5] == 1) // for now, not checking crc
                {
                    if(keyword[2] == "C02A")
                    {
                        coefA_P1 = floatData[0];
                        floatToLong = floatData[0] * 1000;
                        ltoa(floatToLong, txBuffer, 10);
                        uart1_puts("CoefA_P1 = ");
                        uart1_puts(txBuffer);
                        uart1_newline();
                    }
                    else if(keyword[2] == "C02B")        
                    {
                        coefB_P1 = floatData[0];
                        floatToLong = floatData[0] * 1000;
                        ltoa(floatToLong, txBuffer, 10);
                        uart1_puts("CoefB_P1 = ");
                        uart1_puts(txBuffer);
                        uart1_newline();
                    }

                    else if(keyword[2] == "C02C")
                    {
                            coefC_P1 = floatData[0];
                            floatToLong = floatData[0] * 1000;
                            ltoa(floatToLong, txBuffer, 10);
                            uart1_puts("CoefC_P1 = ");
                            uart1_puts(txBuffer);
                            uart1_newline();
                    }

                    else if(keyword[2] == "C02D")
                    {
                            coefD_P1 = floatData[0];
                            floatToLong = floatData[0] * 1000;
                            ltoa(floatToLong, txBuffer, 10);
                            uart1_puts("CoefD_P1 = ");
                            uart1_puts(txBuffer);
                            uart1_newline();
                    }

                    else if(keyword[2] == "C03A")
                    {
                            coefA_T1 = floatData[0];
                            floatToLong = floatData[0] * 1000;
                            ltoa(floatToLong, txBuffer, 10);
                            uart1_puts("CoefA_T1 = ");
                            uart1_puts(txBuffer);
                            uart1_newline();
                    }

                    else if(keyword[2] == "C03B")
                    {
                            coefB_T1 = floatData[0];
                            floatToLong = floatData[0] * 1000;
                            ltoa(floatToLong, txBuffer, 10);
                            uart1_puts("CoefB_T1 = ");
                            uart1_puts(txBuffer);
                            uart1_newline();
                    }

                    else if(keyword[2] == "C03C")
                    {
                            coefC_T1 = floatData[0];
                            floatToLong = floatData[0] * 1000;
                            ltoa(floatToLong, txBuffer, 10);
                            uart1_puts("CoefC_T1 = ");
                            uart1_puts(txBuffer);
                            uart1_newline();
                    }

                    else if(keyword[2] == "C03D")
                    {
                            coefD_T1 = floatData[0];
                            floatToLong = floatData[0] * 1000;
                            ltoa(floatToLong, txBuffer, 10);
                            uart1_puts("CoefD_T1 = ");
                            uart1_puts(txBuffer);
                            uart1_newline();
                    }

                    else if(keyword[2] == "C04A")
                    {
                            coefA_P2 = floatData[0];
                            floatToLong = floatData[0] * 1000;
                            ltoa(floatToLong, txBuffer, 10);
                            uart1_puts("CoefA_P2 = ");
                            uart1_puts(txBuffer);
                            uart1_newline();
                    }

                    else if(keyword[2] == "C04B")
                    {
                            coefB_P2 = floatData[0];
                            floatToLong = floatData[0] * 1000;
                            ltoa(floatToLong, txBuffer, 10);
                            uart1_puts("CoefB_P2 = ");
                            uart1_puts(txBuffer);
                            uart1_newline();
                    }

                    else if(keyword[2] == "C04C")
                    {
                            coefC_P2 = floatData[0];
                            floatToLong = floatData[0] * 1000;
                            ltoa(floatToLong, txBuffer, 10);
                            uart1_puts("CoefC_P2 = ");
                            uart1_puts(txBuffer);
                            uart1_newline();
                    }

                    else if(keyword[2] == "C04D")
                    {
                            coefD_P2 = floatData[0];
                            floatToLong = floatData[0] * 1000;
                            ltoa(floatToLong, txBuffer, 10);
                            uart1_puts("CoefD_P2 = ");
                            uart1_puts(txBuffer);
                            uart1_newline();
                    }

                    else if(keyword[2] == "C05A")
                    {
                            coefA_T2 = floatData[0];
                            floatToLong = floatData[0] * 1000;
                            ltoa(floatToLong, txBuffer, 10);
                            uart1_puts("CoefA_T2 = ");
                            uart1_puts(txBuffer);
                            uart1_newline();
                    }

                    else if(keyword[2] == "C05B")
                    {
                            coefB_T2 = floatData[0];
                            floatToLong = floatData[0] * 1000;
                            ltoa(floatToLong, txBuffer, 10);
                            uart1_puts("CoefB_T2 = ");
                            uart1_puts(txBuffer);
                            uart1_newline();
                    }

                    else if(keyword[2] == "C05C")
                    {
                            coefC_T2 = floatData[0];
                            floatToLong = floatData[0] * 1000;
                            ltoa(floatToLong, txBuffer, 10);
                            uart1_puts("CoefC_T2 = ");
                            uart1_puts(txBuffer);
                            uart1_newline();
                    }

                    else if(keyword[2] == "C05D")
                    {
                            coefD_T2 = floatData[0];
                            floatToLong = floatData[0] * 1000;
                            ltoa(floatToLong, txBuffer, 10);
                            uart1_puts("CoefD_T2 = ");
                            uart1_puts(txBuffer);
                            uart1_newline();
                    }

                    //                    temp1Flag = true;
                    //                    uart1_puts("Temp1 cal.\n");
                    //                    temp1Factor = floatData[0];
                }
            }
            else if(dataCount < floatDataPacketLen || dataCount > floatDataPacketLen)
            {
                serialMsgErrorFlag = true; // uart1_puts("data no good\n");
                dataCount = 0;
                serialMsgFlags = 0;
            }
        }
        else
        {
            // uart1_puts("sorry no keyword\n");
            // dataCount = 0;
            // serialMsgFlags = 0;
        }
        //        if(integerData[0] == 1 && integerData[1] == 1)      // for now, not checking crc
        //        {
        //            pres1Flag = true;
        //            uart1_puts("Temp1 cal.\n");
        //            pres1Factor = floatData[0];
        //        }        
    }
    // </editor-fold>

    // <editor-fold defaultstate="collapsed" desc="EEPROM write 1 byte task">
    if(serialMsgFlags == SER_MSG_FLAG_EEPROMWr1)
    {
        keyword[4] = "C10";
        keywordPointer = strstr(input_line, keyword[4]);
        if(keywordPointer != NULL)
        {
            dataPosition = (keywordPointer - input_line) + strlen(keyword[4]);
            dataCount = 0;

            char *token = strtok(&input_line[dataPosition], delimiter);
            if(token != NULL && strlen(token) < sizeof (parsedStrings[0]))
            {
                strncpy(parsedStrings[0], token, sizeof (parsedStrings[0]));
                dataCount++;
            }
            else
            {
                // uart_puts("token to big1\n");
                strcpy(parsedStrings[0], NULL);
            }

            for(uint8_t i = 1; i < eepromWr1PacketLen; i++)
            {
                token = strtok(NULL, delimiter);
                if(token != NULL && strlen(token) < sizeof (parsedStrings[i]))
                {
                    strncpy(parsedStrings[i], token, sizeof (parsedStrings[i]));
                    dataCount++;
                }
                else
                {
                    // uart_puts("token to big2\n");
                    strcpy(parsedStrings[i], NULL);
                }
            }

            // convert ascii data into integer data           
            if(dataCount == eepromWr1PacketLen)
            {
                integerData[0] = (uint8_t) (atoi(parsedStrings[0])); // page
                integerData[1] = (uint8_t) (atoi(parsedStrings[1])); // offset
                integerData[2] = (uint8_t) (atoi(parsedStrings[2])); // data byte to write
                integerData[3] = (uint8_t) (atoi(parsedStrings[3])); // CRC1
                integerData[4] = (uint8_t) (atoi(parsedStrings[4])); // CRC2
                for(uint8_t i = 5; i < NUM_OF_INT_Data; i++)
                {
                    integerData[i] = 0;
                }
                crc = checksumCalculator(integerData, 3);
                crc1 = (crc >> 8) & 0xff; // high byte
                crc2 = (crc >> 0) & 0xff; // low byte
                dataCount = 0;
                /*sprintf(txBuffer, "%d:%d:%d  %d  %d\n", integerData[0], integerData[1],
                        integerData[2], integerData[3], integerData[4]);
                uart1_puts(txBuffer);
                uart1_puts("--Time calib-----------------------------------------\n");*/
            }
            else if(dataCount < eepromWr1PacketLen || dataCount > eepromWr1PacketLen)
            {
                serialMsgErrorFlag = true; // uart1_puts("data no good\n");
                dataCount = 0;
                serialMsgFlags = 0;
            }
        }
        else
        {
            // uart1_puts("sorry no keyword\n");
            // dataCountTime = 0;
            // serialMsgFlags = 0;
        }

        // if(crc1 == integerData[3] && crc2 == integerData[4])  // below line will replaced by this line
        if(integerData[3] == 1 && integerData[4] == 1) // for now, not checking crc
        {
            //            eepromFlag = true;
            //            eepromStateFlags = eepromWr1Byte;
            uart1_puts("EEPROM write done\n");
            EEPROM_write1ByteToPage(EEPROM_ADDR, integerData[0], integerData[1], integerData[2]);
        }
    }
    // </editor-fold>

    // <editor-fold defaultstate="collapsed" desc="EEPROM read 1 byte task">
    if(serialMsgFlags == SER_MSG_FLAG_EEPROMRd1)
    {
        keyword[5] = "C11";
        keywordPointer = strstr(input_line, keyword[5]);
        if(keywordPointer != NULL)
        {
            dataPosition = (keywordPointer - input_line) + strlen(keyword[5]);
            dataCount = 0;

            char *token = strtok(&input_line[dataPosition], delimiter);
            if(token != NULL && strlen(token) < sizeof (parsedStrings[0]))
            {
                strncpy(parsedStrings[0], token, sizeof (parsedStrings[0]));
                dataCount++;
            }
            else
            {
                // uart_puts("token to big1\n");
                strcpy(parsedStrings[0], NULL);
            }

            for(uint8_t i = 1; i < eepromRd1PacketLen; i++)
            {
                token = strtok(NULL, delimiter);
                if(token != NULL && strlen(token) < sizeof (parsedStrings[i]))
                {
                    strncpy(parsedStrings[i], token, sizeof (parsedStrings[i]));
                    dataCount++;
                }
                else
                {
                    // uart_puts("token to big2\n");
                    strcpy(parsedStrings[i], NULL);
                }
            }

            // convert ascii data into integer data           
            if(dataCount == eepromRd1PacketLen)
            {
                integerData[0] = (uint8_t) (atoi(parsedStrings[0])); // page
                integerData[1] = (uint8_t) (atoi(parsedStrings[1])); // offset
                integerData[2] = (uint8_t) (atoi(parsedStrings[2])); // CRC1
                integerData[3] = (uint8_t) (atoi(parsedStrings[3])); // CRC2
                for(uint8_t i = 4; i < NUM_OF_INT_Data; i++)
                {
                    integerData[i] = 0;
                }
                crc = checksumCalculator(integerData, 2);
                crc1 = (crc >> 8) & 0xff; // high byte
                crc2 = (crc >> 0) & 0xff; // low byte
                dataCount = 0;
                /*sprintf(txBuffer, "%d:%d:%d  %d  %d\n", integerData[0], integerData[1],
                        integerData[2], integerData[3], integerData[4]);
                uart1_puts(txBuffer);
                uart1_puts("--Time calib-----------------------------------------\n");*/
            }
            else if(dataCount < eepromRd1PacketLen || dataCount > eepromRd1PacketLen)
            {
                serialMsgErrorFlag = true; // uart1_puts("data no good\n");
                dataCount = 0;
                serialMsgFlags = 0;
            }
        }
        else
        {
            // uart1_puts("sorry no keyword\n");
            // dataCountTime = 0;
            // serialMsgFlags = 0;
        }

        // if(crc1 == integerData[2] && crc2 == integerData[3])  // below line will replaced by this line
        if(integerData[2] == 1 && integerData[3] == 1) // for now, not checking crc
        {
            //            eepromFlag = true;
            //            eepromStateFlags = eepromWr1Byte;
            uint8_t readByte = EEPROM_read1ByteFromPage(EEPROM_ADDR, integerData[0], integerData[1]);
            itoa(readByte, txBuffer, 10);
            uart1_puts(txBuffer);
            uart1_puts("    EEPROM read done\n");
        }
    }
        // </editor-fold>

        // <editor-fold defaultstate="collapsed" desc="No task">
    else
    {
        serialMsgFlags = 0;
        if(serialMsgErrorFlag == true)
        {
            serialMsgErrorFlag = false;
            uart1_puts("data no good\n");
        }
    }
    // </editor-fold>
    // end Date calibration task


}

// <editor-fold defaultstate="collapsed" desc="C functions">

uint16_t checksumCalculator(uint8_t *data, uint16_t length)
{
    uint16_t curr_crc = 0x0000;
    uint8_t sum1 = (uint8_t) curr_crc;
    uint8_t sum2 = (uint8_t) (curr_crc >> 8);
    uint8_t index;
    for(index = 0; index < length; index = index + 1)
    {
        sum1 = (sum1 + data[index]) % 255;
        sum2 = (sum2 + sum1) % 255;
    }
    return (uint16_t) ((sum2 << 8) | sum1);
}

// Function to swap two numbers

void swap(char *x, char *y)
{
    char t = *x;
    *x = *y;
    *y = t;
}

// Function to reverse `buffer[i?j]`

char* reverse(char *buffer, int i, int j)
{
    while(i < j)
    {
        swap(&buffer[i++], &buffer[j--]);
    }
    return buffer;
}

// Iterative function to implement `itoa()` function in C

char* itoa(int value, char* buffer, int numBase)
{
    // invalid input
    if(numBase < 2 || numBase > 32)
    {
        return buffer;
    }

    // consider the absolute value of the number
    int n = abs(value);
    int i = 0;
    while(n)
    {
        int r = n % numBase;

        if(r >= 10)
        {
            buffer[i++] = 65 + (r - 10);
        }
        else
        {
            buffer[i++] = 48 + r;
        }
        n = n / numBase;
    }

    // if the number is 0
    if(i == 0)
    {
        buffer[i++] = '0';
    }

    // If the base is 10 and the value is negative, the resulting string
    // is preceded with a minus sign (-)
    // With any other base, value is always considered unsigned
    if(value < 0 && numBase == 10)
    {
        buffer[i++] = '-';
    }
    buffer[i] = '\0'; // null terminate string

    // reverse the string and return it
    return reverse(buffer, 0, i - 1);
}

// Iterative function to implement `itoa()` function in C

char* ltoa(long value, char* buffer, int numBase)
{
    // invalid input
    if(numBase < 2 || numBase > 32)
    {
        return buffer;
    }

    // consider the absolute value of the number
    int n = abs(value);
    int i = 0;
    while(n)
    {
        int r = n % numBase;

        if(r >= 10)
        {
            buffer[i++] = 65 + (r - 10);
        }
        else
        {
            buffer[i++] = 48 + r;
        }
        n = n / numBase;
    }

    // if the number is 0
    if(i == 0)
    {
        buffer[i++] = '0';
    }

    // If the base is 10 and the value is negative, the resulting string
    // is preceded with a minus sign (-)
    // With any other base, value is always considered unsigned
    if(value < 0 && numBase == 10)
    {
        buffer[i++] = '-';
    }
    buffer[i] = '\0'; // null terminate string

    // reverse the string and return it
    return reverse(buffer, 0, i - 1);
}
// </editor-fold>