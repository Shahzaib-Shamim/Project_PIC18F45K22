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

// --UART variables-------------------------------------------------------------
#define charLen     10
#define numOfChar   8
#define timePacketLen   5
#define datePacketLen   6

char txBuff[40];
uint8_t serialMsgFlags = 0;
char parsedStrings[charLen][numOfChar];
uint8_t dataPosition = 0;
uint8_t dataCount = 0;
const char delimiter[] = ",";
char *keywordPointer;
uint8_t integerData[15];

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
    /*myADC.address = ADS1115_ADDR;
    myADC.config.value = 0; // reset
    myADC.config.mux = MUX_SINGLE_0;
    myADC.config.pga = PGA_FSR_6_144;
    myADC.config.mode = MODE_SINGLE_SHOT_OR_PWR_DOWN;
    myADC.config.dataRate = DR_128_SPS;
    myADC.config.compMode = COMP_MODE_TRADITIONAL;
    myADC.config.compPol = COMP_POL_ACT_LOW;
    myADC.config.compLat = COMP_LAT_NONLATCHING;
    myADC.config.compQue = COMP_QUEUE_DISABLE_COMPARATOR;
    ADS1115_init(&myADC);*/

    //    if(myADC.isPresent == 1)
    //    {
    //        sprintf((char *) txBuffer, "ADS1115 Found\r\n");
    //        EUSART1_send_string((uint8_t *) txBuffer);
    //    }
    //    if(myADC.isPresent == 0)
    //    {
    //        sprintf((char *) txBuffer, "ADS1115 Not found\r\n");
    //        EUSART1_send_string((uint8_t *) txBuffer);
    //    }
}

char str[20];
uint32_t counterIndex = 0;

void runApplication(void)
{
    if(sysFlags.ledBlinkTask == true)
    {
        sysFlags.ledBlinkTask = false;
        ledBlink();
    }

    if(sysFlags.uartTxTask == true)
    {
        static uint8_t counter = 0;
        sysFlags.uartTxTask = false;

        //        sprintf((char *) txBuffer, "\nCounter Value = %d\r\n", counter);
        //        EUSART1_send_string((uint8_t *) txBuffer);
        //        counter++;
        //        ads1115SampleData();
//                ds3232Timestamp();
    }

    if(sysFlags.ads1115SampleTask == true)
    {
        sysFlags.ads1115SampleTask = false;
        //        ads1115SampleData();
    }

    if(sysFlags.ds3231Task == true)
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

        /*LCDsetCursor(0, 0);
        LCD_Write_Str("Counter = ");
        LCDsetCursor(0, 1);
        sprintf(str, "%lu", counterIndex);
        counterIndex++;
        LCD_Write_Str(str);*/
    }

    // --Serial Msg process-----------------------------------------------------
    if(uartProcessFlag == true)
    {
        uartProcessFlag = false;

        // Time calibration
        if(input_line[0] == 'C' && input_line[1] == '0' && input_line[2] == '0')
        {
            serialMsgFlags = 1;
        }

            // Date calibration
        else if(input_line[0] == 'C' && input_line[1] == '0' && input_line[2] == '1')
        {
            serialMsgFlags = 2;
        }

            // Nothing to be process
        else
        {
            serialMsgFlags = 0;
        }

        process_data();
    }
}

void ledBlink(void)
{
    LED_Toggle();
}

void ads1115SampleData(void)
{
    float voltage[4];
    static uint8_t count = 0;

    if(myADC.isPresent == 1)
    {

        for(uint8_t i = 0; i < 4; i++)
        {
            voltage[i] = ADS1115_readSingleEnded(&myADC, i);
            //            __delay_us(10);
        }

        //        sprintf((char *) txBuffer, "--Start-------------------------------\r\n");
        //        EUSART1_send_string((uint8_t *) txBuffer);
        //
        //        sprintf(txBuffer, "\nADC# %d,  %0.2f  %0.2f  %0.2f  %0.2f\r\n\n",
        //                count, voltage[0], voltage[1], voltage[2], voltage[3]);
        //        EUSART1_send_string(txBuffer);
        //        count++;
        //        EUSART1_send_string("ADS1115\n\r\0");
    }
}

void ds3232Timestamp(void)
{
    char *Day = "";
    char *MonthName = "";
    uint8_t ds3231_readBuff[7];
    DS3231_getTime(&ds3231_readBuff[0], &ds3231_readBuff[1], &ds3231_readBuff[2]);
    DS3231_getDate(&ds3231_readBuff[3], &ds3231_readBuff[4], &ds3231_readBuff[5], &ds3231_readBuff[6]);
    Day = setDayName(ds3231_readBuff[3]);
    MonthName = setMonthName(ds3231_readBuff[5]);

    //    sprintf(txBuffer, "\n%d:%d:%d %s %d-%s-%d\r\n\n", ds3231_readBuff[2], ds3231_readBuff[1], ds3231_readBuff[0], Day, ds3231_readBuff[4], MonthName, ds3231_readBuff[6]);
    //    EUSART1_send_string(txBuffer);
    //    EUSART1_send_string("RTC\n\r\0");
}


void process_data(void)
{
    //    uart1_puts("In ProcessRoutine\n");

    // <editor-fold defaultstate="collapsed" desc="Parsing data and make seperate variable value">
    // Time calibration task
    if(serialMsgFlags == 1)
    {
        char keyword[] = "C00";
        //        char *keywordPointer = strstr(input_line, keyword);
        keywordPointer = strstr(input_line, keyword);
        if(keywordPointer != NULL)
        {
            //            uint8_t dataPosition = (keywordPointer - input_line) + strlen(keyword);
            dataPosition = (keywordPointer - input_line) + strlen(keyword);
            //            const char delimiter[] = ",";
            //            char parsedStrings[5][8];
            //            uint8_t dataCount = 0;

            char *token = strtok(&input_line[dataPosition], delimiter);
            if(token != NULL && strlen(token) < sizeof (parsedStrings[0]))
            {
                strncpy(parsedStrings[0], token, sizeof (parsedStrings[0]));
                dataCount++;
            }
            else
            {
                //                uart_puts("token to big1\n");
                strcpy(parsedStrings[0], NULL);
            }

            for(uint8_t i = 1; i < timePacketLen; i++)
            {
                token = strtok(NULL, delimiter);
                if(token != NULL && strlen(token) < sizeof (parsedStrings[i]))
                {
                    strncpy(parsedStrings[i], token, sizeof (parsedStrings[i]));
                    dataCount++;
                }
                else
                {
                    //                    uart_puts("token to big2\n");
                    strcpy(parsedStrings[i], NULL);
                }
            }
            //            sprintf(txBuff, "Found %d strings\n", dataCount);
            //            uart_puts(txBuff);

            //            for(uint8_t i = 0; i < timePacketLen; i++)
            //            {
            //                uart_puts(parsedStrings[i]);
            //                uart_puts("\n");
            //            }
            // convert ascii data into integer data           
            if(dataCount == timePacketLen)
            {
                integerData[0] = (uint8_t)(atoi(parsedStrings[0])); // hour
                integerData[1] = (uint8_t)(atoi(parsedStrings[1])); // minute
                integerData[2] = (uint8_t)(atoi(parsedStrings[2])); // second
                integerData[3] = (uint8_t)(atoi(parsedStrings[3])); // len
                integerData[4] = (uint8_t)(atoi(parsedStrings[4])); // CRC
                for(uint8_t i = 5; i < 15; i++)
                {
                    integerData[i] = 0;
                }
                dataCount = 0;
                sprintf(txBuff, "%d:%d:%d  %d  %d\n", integerData[0], integerData[1],
                        integerData[2], integerData[3], integerData[4]);
                uart1_puts(txBuff);
                uart1_puts("--Time calib-----------------------------------------\n");
            }
            else if(dataCount < timePacketLen || dataCount > timePacketLen)
            {
                uart1_puts("data no good\n");
                dataCount = 0;
                serialMsgFlags = 0;
            }
        }
        else
        {
            uart1_puts("sorry no keyword\n");
            //            dataCount = 0;
            //            serialMsgFlags = 0;
        }
    }

    // Date calibration task
    if(serialMsgFlags == 2)
    {
//        static uint8_t dataCountTemp = 0;
        char keyword[] = "C01";
        //        char *keywordPointer = strstr(input_line, keyword);
        keywordPointer = strstr(input_line, keyword);
        if(keywordPointer != NULL)
        {
            //            uint8_t dataPosition = (keywordPointer - input_line) + strlen(keyword);
            dataPosition = (keywordPointer - input_line) + strlen(keyword);
            //            const char delimiter[] = ",";
            //            char parsedStrings[5][8];
            //            uint8_t dataCount = 0;

            char *token = strtok(&input_line[dataPosition], delimiter);
            if(token != NULL && strlen(token) < sizeof (parsedStrings[0]))
            {
                strncpy(parsedStrings[0], token, sizeof (parsedStrings[0]));
                dataCount++;
            }
            else
            {
                //                uart_puts("token to big1\n");
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
                    //                    uart_puts("token to big2\n");
                    strcpy(parsedStrings[i], NULL);
                }
            }
            //            sprintf(txBuff, "Found %d strings\n", dataCount);
            //            uart_puts(txBuff);

            //            for(uint8_t i = 0; i < timePacketLen; i++)
            //            {
            //                uart_puts(parsedStrings[i]);
            //                uart_puts("\n");
            //            }
            // convert ascii data into integer data           
            if(dataCount == datePacketLen)
            {
                integerData[0] = (uint8_t)(atoi(parsedStrings[0]));    // day
                integerData[1] = (uint8_t)(atoi(parsedStrings[1]));    // date
                integerData[2] = (uint8_t)(atoi(parsedStrings[2]));    // month
                integerData[3] = (uint8_t)(atoi(parsedStrings[3]));    // year
                integerData[4] = (uint8_t)(atoi(parsedStrings[4]));    // len
                integerData[5] = (uint8_t)(atoi(parsedStrings[5]));    // CRC
                for(uint8_t i = 6; i < 15; i++)
                {
                    integerData[i] = 0; 
                }
                dataCount = 0;
                sprintf(txBuff, "%d, %d-%d-%d  %d %d\n", integerData[0], integerData[1], 
                        integerData[2], integerData[3], integerData[4], integerData[5]);
                uart1_puts(txBuff);
                uart1_puts("--Date calib-----------------------------------------\n");
            }
            else if(dataCount < datePacketLen || dataCount > datePacketLen)
            {
                uart1_puts("data no good date\n");
                dataCount = 0;
                serialMsgFlags = 0;
            }
        }
        
        else
        {
            uart1_puts("sorry no keyword\n");
            //            dataCount = 0;
            //            serialMsgFlags = 0;
        }
    }    




    else
    {
        serialMsgFlags = 0;
    }

    // </editor-fold>
}