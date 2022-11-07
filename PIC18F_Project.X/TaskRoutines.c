#include "TaskRoutines.h"
#include "GPIO.h"
#include "UART.h"
#include "ADS1115.h"
#include "DS3231.h"
#include "EEPROM.h"
#include "LCD_HD44780_20x4.h"
#include "EXTI.h"
#include "TMR0.h"
#include "Capture.h"
#include "Functions.h"
#include "TMR.h"

// <editor-fold defaultstate="collapsed" desc="Variables">
// other variables
volatile TIMER1_t timer1 = {
    .timer1_1msFlag = false,
    .timer1_1msCounter = 0
};
volatile ApplicationFlags_t sysFlags;
bool systemStates = 0;
bool txEnableFlag = true;
bool calibrationEnableFlag = false;
unsigned long errorTimer = 0;

// CRC Variables
uint8_t crcBuff[35];
uint16_t crcUART = 0;
uint8_t crc1UARTHigh = 0;
uint8_t crc2UARTLow = 0;

// ADC variables
#define FILTER_SIZE     4
ADS1115_Object_t myADC;
uint8_t channelNumber = 0;
float voltage[4];
float voltageFiltered[4];
float calibData[4];
MAVFilter_f_t* adc_channel0_pres1;
MAVFilter_f_t* adc_channel1_temp1;
MAVFilter_f_t* adc_channel2_pres2;
MAVFilter_f_t* adc_channel3_temp2;

// DS3231 variables
uint8_t ds3231_readBuff[7];
bool dateFlag = false;
bool timeFlag = false;

// UART variables
char txBuffer[50] = {0};
uint8_t serialMsgFlags = SER_MSG_FLAG_IDLE;
uint8_t serialMsgErrorFlag = false;
uint8_t integerData[NUM_OF_INT_Data];
char *keyword;

// LCD variables
char lcdBuff[8] = {0};
uint8_t lcdStates = 0;
char lcdBuff2[20] = {0};
char str[10] = {0};
char *p = str;

// Pulse timer variables
uint8_t lowByte, highByte;
uint16_t pulsetime;
//uint32_t pulsetime;

union SystemState_u{

    struct{
        unsigned rtcTimeFlag : 1;
        unsigned rtcDateFlag : 1;
        unsigned eepromWrBytes : 1;
        unsigned eepromRdBytes : 1;
        unsigned eepromWrByte : 1;
        unsigned eepromRdByte : 1;
        unsigned fwWr : 1;
        unsigned fwRd : 1;
    };
    uint16_t systemAllStates;
};
union SystemState_u systemStatus;

union RelayState_u{

    struct{
        unsigned button1 : 1;
        unsigned button2 : 1;
        unsigned button3 : 1;
        unsigned button4 : 1;
        unsigned button5 : 1;
        unsigned pulsecounter : 1;
        unsigned : 1;
        unsigned : 1;
    };
    uint8_t relay;
};
union RelayState_u relayStatus;

union sensorCalibrationCoef_u{

    struct{
        unsigned pres1_a : 1;
        unsigned pres1_b : 1;
        unsigned pres1_c : 1;
        unsigned pres1_d : 1;
        unsigned temp1_a : 1;
        unsigned temp1_b : 1;
        unsigned temp1_c : 1;
        unsigned temp1_d : 1;
        unsigned pres2_a : 1;
        unsigned pres2_b : 1;
        unsigned pres2_c : 1;
        unsigned pres2_d : 1;
        unsigned temp2_a : 1;
        unsigned temp2_b : 1;
        unsigned temp2_c : 1;
        unsigned temp2_d : 1;
    };
    uint16_t sensorsCoef;
};
union sensorCalibrationCoef_u sensorsCoefStates;

typedef struct{
    float coefA;
    float coefB;
    float coefC;
    float coefD;
} calFactors;
calFactors pres1 = {.coefA = 1.0f, .coefB = 1.0f, .coefC = 1.0f, .coefD = 1.0f},
temp1 = {.coefA = 1.0f, .coefB = 1.0f, .coefC = 1.0f, .coefD = 1.0f},
pres2 = {.coefA = 1.0f, .coefB = 1.0f, .coefC = 1.0f, .coefD = 1.0f},
temp2 = {.coefA = 1.0f, .coefB = 1.0f, .coefC = 1.0f, .coefD = 1.0f};
// </editor-fold>

void initSysVariables(void)
{
    sysFlags.ledBlinkTask = false;
    sysFlags.uartTxTask = false;
    sysFlags.ads1115SampleTask = false;
    sysFlags.lcdTask = false;
    sysFlags.coefTask = false;
    sysFlags.pulseTask = false;

    // Initialize moving average filter
    adc_channel0_pres1 = Initialize_MAVFilter_f(FILTER_SIZE);
    adc_channel1_temp1 = Initialize_MAVFilter_f(FILTER_SIZE);
    adc_channel2_pres2 = Initialize_MAVFilter_f(FILTER_SIZE);
    adc_channel3_temp2 = Initialize_MAVFilter_f(FILTER_SIZE);

    // ADS1115 configuration
    myADC.address = ADS1115_ADDR;
    myADC.config.value = 0; // reset register
    myADC.config.mux = MUX_SINGLE_0;
    myADC.config.pga = PGA_FSR_6_144;
    myADC.config.mode = MODE_SINGLE_SHOT_OR_PWR_DOWN;
    myADC.config.dataRate = DR_128_SPS;
    myADC.config.compMode = COMP_MODE_TRADITIONAL;
    myADC.config.compPol = COMP_POL_ACT_LOW;
    myADC.config.compLat = COMP_LAT_NONLATCHING;
    myADC.config.compQue = COMP_QUEUE_ONE_CONVERSION;
    ADS1115_init(&myADC);
}

void load_Coef(void)
{
    uint8_t eepromReadBuff[4];

    EEPROM_readMultipleBytes(EEPROM_ADDR, 1, 0, eepromReadBuff, 4);
    EEPROM_byteToFloat(eepromReadBuff, &pres1.coefA);
    EEPROM_readMultipleBytes(EEPROM_ADDR, 1, 4, eepromReadBuff, 4);
    EEPROM_byteToFloat(eepromReadBuff, &pres1.coefB);
    EEPROM_readMultipleBytes(EEPROM_ADDR, 1, 8, eepromReadBuff, 4);
    EEPROM_byteToFloat(eepromReadBuff, &pres1.coefC);
    EEPROM_readMultipleBytes(EEPROM_ADDR, 1, 12, eepromReadBuff, 4);
    EEPROM_byteToFloat(eepromReadBuff, &pres1.coefD);

    EEPROM_readMultipleBytes(EEPROM_ADDR, 2, 0, eepromReadBuff, 4);
    EEPROM_byteToFloat(eepromReadBuff, &temp1.coefA);
    EEPROM_readMultipleBytes(EEPROM_ADDR, 2, 4, eepromReadBuff, 4);
    EEPROM_byteToFloat(eepromReadBuff, &temp1.coefB);
    EEPROM_readMultipleBytes(EEPROM_ADDR, 2, 8, eepromReadBuff, 4);
    EEPROM_byteToFloat(eepromReadBuff, &temp1.coefC);
    EEPROM_readMultipleBytes(EEPROM_ADDR, 2, 12, eepromReadBuff, 4);
    EEPROM_byteToFloat(eepromReadBuff, &temp1.coefD);

    EEPROM_readMultipleBytes(EEPROM_ADDR, 3, 0, eepromReadBuff, 4);
    EEPROM_byteToFloat(eepromReadBuff, &pres2.coefD);
    EEPROM_readMultipleBytes(EEPROM_ADDR, 3, 4, eepromReadBuff, 4);
    EEPROM_byteToFloat(eepromReadBuff, &pres2.coefD);
    EEPROM_readMultipleBytes(EEPROM_ADDR, 3, 8, eepromReadBuff, 4);
    EEPROM_byteToFloat(eepromReadBuff, &pres2.coefD);
    EEPROM_readMultipleBytes(EEPROM_ADDR, 3, 12, eepromReadBuff, 4);
    EEPROM_byteToFloat(eepromReadBuff, &pres2.coefD);

    EEPROM_readMultipleBytes(EEPROM_ADDR, 4, 0, eepromReadBuff, 4);
    EEPROM_byteToFloat(eepromReadBuff, &temp2.coefD);
    EEPROM_readMultipleBytes(EEPROM_ADDR, 4, 4, eepromReadBuff, 4);
    EEPROM_byteToFloat(eepromReadBuff, &temp2.coefD);
    EEPROM_readMultipleBytes(EEPROM_ADDR, 4, 8, eepromReadBuff, 4);
    EEPROM_byteToFloat(eepromReadBuff, &temp2.coefD);
    EEPROM_readMultipleBytes(EEPROM_ADDR, 4, 12, eepromReadBuff, 4);
    EEPROM_byteToFloat(eepromReadBuff, &temp2.coefD);
}

void storeLastCalibRTC(void)
{
    uint8_t readBuff[7];
    DS3231_getData(&readBuff[0], &readBuff[1], &readBuff[2], &readBuff[3], &readBuff[4], &readBuff[5], &readBuff[6]);
    EEPROM_writeMultipleBytes(EEPROM_ADDR, 0, 4, readBuff, 7);
    uart1_puts("\nRTC Stored\n");
}

void getLastCalibRTC(void)
{
    uint8_t readBuff[7];
    EEPROM_readMultipleBytes(EEPROM_ADDR, 0, 4, readBuff, 7);

    itoa(readBuff[2], txBuffer, 10);
    uart1_puts(txBuffer);
    uart1_putc(':');
    itoa(readBuff[1], txBuffer, 10);
    uart1_puts(txBuffer);
    uart1_putc(':');
    itoa(readBuff[0], txBuffer, 10);
    uart1_puts(txBuffer);
    uart1_putc(' ');

    itoa(readBuff[4], txBuffer, 10);
    uart1_puts(txBuffer);
    uart1_putc('/');
    itoa(readBuff[5], txBuffer, 10);
    uart1_puts(txBuffer); //MonthName
    uart1_putc('/');
    itoa(readBuff[6], txBuffer, 10);
    uart1_puts(txBuffer);
    uart1_puts(" ");
    uart1_puts("Last RTC\n");
    //    sprintf(txBuffer, "%d:%d:%d %d/%d/%d Last RTC\n", readBuff[2], readBuff[1], readBuff[0], readBuff[4], readBuff[5], readBuff[6]);
    //    uart1_puts(txBuffer);
}

void getFWInfo(void)
{
    uint8_t readBuff;
    readBuff = EEPROM_read1ByteFromPage(EEPROM_ADDR, 0, 0);
    uart1_puts("FW info = ");
    itoa(readBuff, txBuffer, 10);
    uart1_puts(txBuffer);
    uart1_newline();
}

void gpioInitialize(void)
{
    // reset all relay outputs
    relayStatus.relay = 0;
    LATAbits.LATA0 = relayStatus.button1;
    LATAbits.LATA1 = relayStatus.button2;
    LATAbits.LATA2 = relayStatus.button3;
    LATAbits.LATA3 = relayStatus.button4;
    LATAbits.LATA4 = relayStatus.button5;

    // reset pulsecounter
    LATAbits.LATA5 = HIGH;
    __delay_ms(10);
    LATAbits.LATA5 = LOW;
}

void startInitialization(void)
{
    gpioInitialize();
    load_Coef();
    ADS1115_setReadSingleEnded(&myADC, channelNumber);
}

void runApplication(void)
{
    // <editor-fold defaultstate="collapsed" desc="LED Blink Task">
    if(sysFlags.ledBlinkTask == true)
    {
        sysFlags.ledBlinkTask = false;
        ledBlink();
    }
    // </editor-fold>

    // <editor-fold defaultstate="collapsed" desc="UART Task">
    if(sysFlags.uartTxTask == true && txEnableFlag == true)
    {
        sysFlags.uartTxTask = false;
        
        // reset CRCbuffer to 0
        for(uint8_t i = 0; i < 35; i++)
            crcBuff[i] = 0;
        uint8_t dataBytes[4];
        
        // for ASCII format (no need when using byte format)
        floatToByte(calibData[0], &crcBuff[0]);
        floatToByte(calibData[1], &crcBuff[4]);
        floatToByte(calibData[2], &crcBuff[8]);
        floatToByte(calibData[3], &crcBuff[12]);
            
        if(calibrationEnableFlag == true)
        {
            // Data in ASCII format
            floatToByte(voltageFiltered[0], &crcBuff[16]);
            floatToByte(voltageFiltered[1], &crcBuff[20]);
            floatToByte(voltageFiltered[2], &crcBuff[24]);
            floatToByte(voltageFiltered[3], &crcBuff[28]);        
           /*crcUART = checksumCalculator(crcBuff, 32);
            crc1UARTHigh = (crcUART >> 8) & 0xff;
            crc2UARTLow = (crcUART >> 0) & 0xff;

            sprintf(txBuffer, "%0.3f  %0.3f  %0.3f  %0.3f----", calibData[0], calibData[1], calibData[2], calibData[3]);
            uart1_puts(txBuffer);
            sprintf(txBuffer, "%0.3f  %0.3f  %0.3f  %0.3f  ", voltageFiltered[0], voltageFiltered[1], voltageFiltered[2], voltageFiltered[3]);
            uart1_puts(txBuffer);
            
            itoa(crc1UARTHigh, txBuffer, 10); uart1_puts(txBuffer);
            uart1_puts(",");
            itoa(crc2UARTLow, txBuffer, 10); uart1_puts(txBuffer);
            uart1_newline();*/
            
            // Data in byte format
            uart1_putc(crcBuff[0]);     // calibData[0]
            uart1_putc(crcBuff[1]);
            uart1_putc(crcBuff[2]);
            uart1_putc(crcBuff[3]);
            uart1_putc(crcBuff[4]);     // calibData[1]
            uart1_putc(crcBuff[5]);
            uart1_putc(crcBuff[6]);
            uart1_putc(crcBuff[7]);
            uart1_putc(crcBuff[8]);     // calibData[2]
            uart1_putc(crcBuff[9]);
            uart1_putc(crcBuff[10]);
            uart1_putc(crcBuff[11]);
            uart1_putc(crcBuff[12]);    // calibData[3]
            uart1_putc(crcBuff[13]);
            uart1_putc(crcBuff[14]);
            uart1_putc(crcBuff[15]);
            
            uart1_putc(crcBuff[16]);    // voltageFiltered[0]
            uart1_putc(crcBuff[17]);
            uart1_putc(crcBuff[18]);
            uart1_putc(crcBuff[19]);
            uart1_putc(crcBuff[20]);    // voltageFiltered[1]
            uart1_putc(crcBuff[21]);
            uart1_putc(crcBuff[22]);
            uart1_putc(crcBuff[23]);
            uart1_putc(crcBuff[24]);    // voltageFiltered[2]
            uart1_putc(crcBuff[25]);
            uart1_putc(crcBuff[26]);
            uart1_putc(crcBuff[27]);
            uart1_putc(crcBuff[28]);    // voltageFiltered[3]
            uart1_putc(crcBuff[29]);
            uart1_putc(crcBuff[30]);
            uart1_putc(crcBuff[31]);
            
            crcUART = checksumCalculator(&crcBuff[0], 32);
            crc1UARTHigh = (crcUART >> 8) & 0xff;
            crc2UARTLow = (crcUART >> 0) & 0xff;
            
            uart1_putc(crc1UARTHigh);
            uart1_putc(crc2UARTLow);
            uart1_newline();
        }

        else if(calibrationEnableFlag == false)
        {
            static char *monthName = "";

            DS3231_getTime(&ds3231_readBuff[0], &ds3231_readBuff[1], &ds3231_readBuff[2]);
            DS3231_getDate(&ds3231_readBuff[3], &ds3231_readBuff[4], &ds3231_readBuff[5], &ds3231_readBuff[6]);

            // Data in ASCII format
            /*itoa(ds3231_readBuff[2], lcdBuff, 10);
            uart1_puts(lcdBuff);
            uart1_puts(":"); // hour
            itoa(ds3231_readBuff[1], lcdBuff, 10);
            uart1_puts(lcdBuff);
            uart1_puts(":"); // minute
            itoa(ds3231_readBuff[0], lcdBuff, 10);
            uart1_puts(lcdBuff);
            uart1_puts(" "); // second
            itoa(ds3231_readBuff[3], lcdBuff, 10);
            uart1_puts(lcdBuff);
            uart1_puts(","); // day of week
            itoa(ds3231_readBuff[4], lcdBuff, 10);
            uart1_puts(lcdBuff);
            uart1_puts("/"); // day of month
            monthName = setMonthName(ds3231_readBuff[5]);
            uart1_puts(monthName);
            uart1_puts("/"); // month
            itoa(ds3231_readBuff[6], lcdBuff, 10);
            uart1_puts(lcdBuff); // year
            uart1_puts("  ");

            itoa(relayStatus.relay, txBuffer, 10);
            uart1_puts(txBuffer);
            uart1_puts("  ");

            ltoa(pulsetime, txBuffer, 10);
            uart1_puts(txBuffer);
            uart1_puts("  ");

            sprintf(txBuffer, "%0.3f  %0.3f  %0.3f  %0.3f  ", calibData[0], calibData[1], calibData[2], calibData[3]);
            uart1_puts(txBuffer);

            crcBuff[16] = ds3231_readBuff[2];
            crcBuff[17] = ds3231_readBuff[1];
            crcBuff[18] = ds3231_readBuff[0];
            crcBuff[19] = ds3231_readBuff[4];
            crcBuff[20] = ds3231_readBuff[5];
            crcBuff[21] = ds3231_readBuff[6];
            crcBuff[22] = relayStatus.relay;
            ulongToByte(pulsetime, &crcBuff[23]);

            crcUART = checksumCalculator(&crcBuff[0], 27);
            crc1UARTHigh = (crcUART >> 8) & 0xff;
            crc2UARTLow = (crcUART >> 0) & 0xff;
            
            itoa(crc1UARTHigh, txBuffer, 10); uart1_puts(txBuffer);
            uart1_puts(",");
            itoa(crc2UARTLow, txBuffer, 10); uart1_puts(txBuffer);
            uart1_newline();*/
            
            
            // Data in byte format
//            uint8_t dataBytes[4];
            uart1_putc(ds3231_readBuff[2]);     // hour
            uart1_putc(ds3231_readBuff[1]);     // minute
            uart1_putc(ds3231_readBuff[0]);     // second
            uart1_putc(ds3231_readBuff[4]);     // date
            uart1_putc(ds3231_readBuff[5]);     // month
            uart1_putc(ds3231_readBuff[6]);     // year
            uart1_putc(relayStatus.relay);
            uart1_putc(highByte);               // pulse high byte
            uart1_putc(lowByte);                // pulse low byte
//            floatToByte(calibData[0], &dataBytes[0]);
//            uart1_putc(dataBytes[0]);
//            uart1_putc(dataBytes[1]);
//            uart1_putc(dataBytes[2]);
//            uart1_putc(dataBytes[3]);
//            floatToByte(calibData[1], &dataBytes[0]);
//            uart1_putc(dataBytes[0]);
//            uart1_putc(dataBytes[1]);
//            uart1_putc(dataBytes[2]);
//            uart1_putc(dataBytes[3]);
//            floatToByte(calibData[2], &dataBytes[0]);
//            uart1_putc(dataBytes[0]);
//            uart1_putc(dataBytes[1]);
//            uart1_putc(dataBytes[2]);
//            uart1_putc(dataBytes[3]);
//            floatToByte(calibData[3], &dataBytes[0]);
//            uart1_putc(dataBytes[0]);
//            uart1_putc(dataBytes[1]);
//            uart1_putc(dataBytes[2]);
//            uart1_putc(dataBytes[3]);
            uart1_putc(crcBuff[0]);     // calibData[0]
            uart1_putc(crcBuff[1]);
            uart1_putc(crcBuff[2]);
            uart1_putc(crcBuff[3]);
            uart1_putc(crcBuff[4]);     // calibData[1]
            uart1_putc(crcBuff[5]);
            uart1_putc(crcBuff[6]);
            uart1_putc(crcBuff[7]);
            uart1_putc(crcBuff[8]);     // calibData[2]
            uart1_putc(crcBuff[9]);
            uart1_putc(crcBuff[10]);
            uart1_putc(crcBuff[11]);
            uart1_putc(crcBuff[12]);    // calibData[3]
            uart1_putc(crcBuff[13]);
            uart1_putc(crcBuff[14]);
            uart1_putc(crcBuff[15]);
            
            crcBuff[16] = ds3231_readBuff[2];
            crcBuff[17] = ds3231_readBuff[1];
            crcBuff[18] = ds3231_readBuff[0];
            crcBuff[19] = ds3231_readBuff[4];
            crcBuff[20] = ds3231_readBuff[5];
            crcBuff[21] = ds3231_readBuff[6];
            crcBuff[22] = relayStatus.relay;
            ulongToByte(pulsetime, &crcBuff[23]);

            crcUART = checksumCalculator(&crcBuff[0], 27);
            crc1UARTHigh = (crcUART >> 8) & 0xff;
            crc2UARTLow = (crcUART >> 0) & 0xff;
            
            uart1_putc(crc1UARTHigh);
            uart1_putc(crc2UARTLow);
            uart1_newline();
            
        }        
    }
    // </editor-fold>

    // <editor-fold defaultstate="collapsed" desc="ADS1115 Task">
    if(sysFlags.ads1115SampleTask == true)
    {
        sysFlags.ads1115SampleTask = false;

        // increment ADC channel for next conversion
        if(channelNumber++ == 4)
        {
            channelNumber = 0;

            voltageFiltered[0] = calculateMovingAverage_f(adc_channel0_pres1, voltage[0]);
            voltageFiltered[1] = calculateMovingAverage_f(adc_channel1_temp1, voltage[1]);
            voltageFiltered[2] = calculateMovingAverage_f(adc_channel2_pres2, voltage[2]);
            voltageFiltered[3] = calculateMovingAverage_f(adc_channel3_temp2, voltage[3]);

            calibData[0] = (pres1.coefA * voltageFiltered[0] * voltageFiltered[0] * voltageFiltered[0]) +
                    (pres1.coefB * voltageFiltered[0] * voltageFiltered[0]) + (pres1.coefC * voltageFiltered[0]) + pres1.coefD;

            calibData[1] = (temp1.coefA * voltageFiltered[1] * voltageFiltered[1] * voltageFiltered[1]) +
                    (temp1.coefB * voltageFiltered[1] * voltageFiltered[1]) + (temp1.coefC * voltageFiltered[1]) + temp1.coefD;

            calibData[2] = (pres2.coefA * voltageFiltered[2] * voltageFiltered[2] * voltageFiltered[2]) +
                    (pres2.coefB * voltageFiltered[2] * voltageFiltered[2]) + (pres2.coefC * voltageFiltered[2]) + pres2.coefD;

            calibData[3] = (temp2.coefA * voltageFiltered[3] * voltageFiltered[3] * voltageFiltered[3]) +
                    (temp2.coefB * voltageFiltered[3] * voltageFiltered[3]) + (temp2.coefC * voltageFiltered[3]) + temp2.coefD;
        }
        ADS1115_setReadSingleEnded(&myADC, channelNumber);
    }
    // </editor-fold>

    // <editor-fold defaultstate="collapsed" desc="ADS1115 interrupt callback (continue)">
    if(int0Flag == true)
    {
        int0Flag = false;

        if(channelNumber == 0)
        {
            voltage[0] = ADS1115_getVoltage(&myADC, 0);
        }
        else if(channelNumber == 1)
        {
            voltage[1] = ADS1115_getVoltage(&myADC, 1);
        }
        else if(channelNumber == 2)
        {
            voltage[2] = ADS1115_getVoltage(&myADC, 2);
        }
        else if(channelNumber == 3)
        {
            voltage[3] = ADS1115_getVoltage(&myADC, 3);
        }
    }
    // </editor-fold>

    // <editor-fold defaultstate="collapsed" desc="Coefficient Task (commented)">
    /*if(sysFlags.coefTask == true)
    {
        sysFlags.coefTask = false;

        calibData[0] = (pres1.coefA * voltageFiltered[0] * voltageFiltered[0] * voltageFiltered[0]) +
                        (pres1.coefB * voltageFiltered[0] * voltageFiltered[0]) + (pres1.coefC * voltageFiltered[0]) + pres1.coefD;
        calibData[1] = (temp1.coefA * voltageFiltered[1] * voltageFiltered[1] * voltageFiltered[1]) +
                        (temp1.coefB * voltageFiltered[1] * voltageFiltered[1]) + (temp1.coefC * voltageFiltered[1]) + temp1.coefD;
        calibData[2] = (pres2.coefA * voltageFiltered[2] * voltageFiltered[2] * voltageFiltered[2]) +
                        (pres2.coefB * voltageFiltered[2] * voltageFiltered[2]) + (pres2.coefC * voltageFiltered[2]) + pres2.coefD;
        calibData[3] = (temp2.coefA * voltageFiltered[3] * voltageFiltered[3] * voltageFiltered[3]) +
                        (temp2.coefB * voltageFiltered[3] * voltageFiltered[3]) + (temp2.coefC * voltageFiltered[3]) + temp2.coefD;
    
    }*/
    // </editor-fold>

    // <editor-fold defaultstate="collapsed" desc="LCD Task">
    if(sysFlags.lcdTask == true)
    {
        sysFlags.lcdTask = false;
        char *MonthName = "";
        static bool blinkingSymbol = false;
        blinkingSymbol ^= 1;

        // <editor-fold defaultstate="collapsed" desc="LCD Idle mode">
        if(lcdStates == LCD_IDLE)
        {
            DS3231_getTime(&ds3231_readBuff[0], &ds3231_readBuff[1], &ds3231_readBuff[2]);
            DS3231_getDate(&ds3231_readBuff[3], &ds3231_readBuff[4], &ds3231_readBuff[5], &ds3231_readBuff[6]);

            LCDsetCursor(0, 0);
            LCD_Write_Str("Idle...");
            LCDsetCursor(0, 1);
            LCD_Write_Str("Waiting for command");

            LCDsetCursor(0, 3);
            itoa(ds3231_readBuff[2], lcdBuff, 10);
            LCD_Write_Str(lcdBuff);
            LCD_Write_Str(":");
            itoa(ds3231_readBuff[1], lcdBuff, 10);
            LCD_Write_Str(lcdBuff);
            LCD_Write_Str(":");
            itoa(ds3231_readBuff[0], lcdBuff, 10);
            LCD_Write_Str(lcdBuff);
            LCDsetCursor(9, 3);
            itoa(ds3231_readBuff[4], lcdBuff, 10);
            LCD_Write_Str(lcdBuff);
            LCD_Write_Str("-");
            MonthName = setMonthName(ds3231_readBuff[5]);
            LCD_Write_Str(MonthName);
            LCD_Write_Str("-");
            itoa(ds3231_readBuff[6], lcdBuff, 10);
            LCD_Write_Str(lcdBuff);
        }
        // </editor-fold>

        // <editor-fold defaultstate="collapsed" desc="LCD Normal mode">
        if(lcdStates == LCD_NORMAL_MODE)
        {
            if(blinkingSymbol == true)
            {
                LCDsetCursor(19, 0);
                LCD_Write_Char('*');
            }
            else if(blinkingSymbol == false)
            {
                LCDsetCursor(19, 0);
                LCD_Write_Char(' ');
            }

            LCDsetCursor(0, 0);
            LCD_Write_Str("Pulse=");
            ltoa(pulsetime, lcdBuff, 10);
            LCDsetCursor(6, 0);
            LCD_Write_Str("        ");
            LCDsetCursor(6, 0);
            LCD_Write_Str(lcdBuff);
            LCDsetCursor(16, 0);
            LCD_Write_Str("ms");

            LCDsetCursor(0, 1);
            LCD_Write_Str("P1=");
            //            FloatToStringNew(p, calibData[0], 3);
            sprintf(lcdBuff, "%0.3f", calibData[0]);
            LCDsetCursor(3, 1);
            LCD_Write_Str("        ");
            LCDsetCursor(3, 1);
            //            LCD_Write_Str(str);
            LCD_Write_Str(lcdBuff);
            LCDsetCursor(16, 1);
            LCD_Write_Str("Pa");

            LCDsetCursor(0, 2);
            LCD_Write_Str("T1=");
            //            FloatToStringNew(p, calibData[1], 3);
            sprintf(lcdBuff, "%0.3f", calibData[1]);
            LCDsetCursor(3, 2);
            LCD_Write_Str("        ");
            LCDsetCursor(3, 2);
            //            LCD_Write_Str(str);
            LCD_Write_Str(lcdBuff);
            LCDsetCursor(16, 2);
            LCD_Write_Char(0b11011111); //degree symbol
            LCD_Write_Char('C');

            /*errorTimer++;
            LCDsetCursor(0, 3);
            LCD_Write_Str("Error->");
            ltoa(errorTimer, lcdBuff2, 10);
            LCD_Write_Str(lcdBuff2);*/
            LCDsetCursor(0, 3);
            LCD_Write_Str("Relay#12345=");
            LCDsetCursor(15, 3);
            LCD_Write_Str("     ");
            LCDsetCursor(19, 3);
            itoa(relayStatus.button1, lcdBuff, 2);
            LCD_Write_Str(lcdBuff);
            LCDsetCursor(18, 3);
            itoa(relayStatus.button2, lcdBuff, 2);
            LCD_Write_Str(lcdBuff);
            LCDsetCursor(17, 3);
            itoa(relayStatus.button3, lcdBuff, 2);
            LCD_Write_Str(lcdBuff);
            LCDsetCursor(16, 3);
            itoa(relayStatus.button4, lcdBuff, 2);
            LCD_Write_Str(lcdBuff);
            LCDsetCursor(15, 3);
            itoa(relayStatus.button5, lcdBuff, 2);
            LCD_Write_Str(lcdBuff);
        }
        // </editor-fold>

        // <editor-fold defaultstate="collapsed" desc="LCD Calibration mode">
        if(lcdStates == LCD_CAL_MODE_Y)
        {
            LCDsetCursor(0, 0);
            LCD_Write_Str("Calibration mode:");

            // <editor-fold defaultstate="collapsed" desc="RTC Time">
            if((strncmp(input_line, "C00", 3) == 0) && systemStatus.rtcTimeFlag == 1)
            {
                systemStatus.rtcTimeFlag = 0;
                LCDsetCursor(0, 1);
                LCD_Write_Str("Time set done");
            }
                // </editor-fold>

                // <editor-fold defaultstate="collapsed" desc="RTC Date">
            else if((strncmp(input_line, "C01", 3) == 0))
            {
                systemStatus.rtcDateFlag = 0;
                LCDsetCursor(0, 1);
                LCD_Write_Str("Date set done");
            }
                // </editor-fold>

                // <editor-fold defaultstate="collapsed" desc="RTC Time">
            else if((strncmp(input_line, "C02", 3) == 0))
            {
                LCDsetCursor(0, 1);
                LCD_Write_Str("P1 Coef  done");
                if(sensorsCoefStates.pres1_a == 1)
                {
                    LCDsetCursor(7, 1);
                    LCD_Write_Str("A");
                }
                else if(sensorsCoefStates.pres1_b == 1)
                {
                    LCDsetCursor(7, 1);
                    LCD_Write_Str("B");
                }
                if(sensorsCoefStates.pres1_c == 1)
                {
                    LCDsetCursor(7, 1);
                    LCD_Write_Str("C");
                }
                if(sensorsCoefStates.pres1_d == 1)
                {
                    LCDsetCursor(7, 1);
                    LCD_Write_Str("D");
                }
            }
                // </editor-fold>

                // <editor-fold defaultstate="collapsed" desc="Pressure1 coef">
            else if((strncmp(input_line, "C02", 3) == 0))
            {
                LCDsetCursor(0, 1);
                LCD_Write_Str("P1 Coef  done");
                if(sensorsCoefStates.pres1_a == 1)
                {
                    sensorsCoefStates.pres1_a = 0;
                    LCDsetCursor(7, 1);
                    LCD_Write_Str("A");
                }
                else if(sensorsCoefStates.pres1_b == 1)
                {
                    sensorsCoefStates.pres1_b = 0;
                    LCDsetCursor(7, 1);
                    LCD_Write_Str("B");
                }
                if(sensorsCoefStates.pres1_c == 1)
                {
                    sensorsCoefStates.pres1_c = 0;
                    LCDsetCursor(7, 1);
                    LCD_Write_Str("C");
                }
                if(sensorsCoefStates.pres1_d == 1)
                {
                    sensorsCoefStates.pres1_d = 0;
                    LCDsetCursor(7, 1);
                    LCD_Write_Str("D");
                }
            }
                // </editor-fold>

                // <editor-fold defaultstate="collapsed" desc="Temperature1 coef">
            else if((strncmp(input_line, "C03", 3) == 0))
            {
                LCDsetCursor(0, 1);
                LCD_Write_Str("T1 Coef  done");
                if(sensorsCoefStates.temp1_a == 1)
                {
                    sensorsCoefStates.temp1_a = 0;
                    LCDsetCursor(7, 1);
                    LCD_Write_Str("A");
                }
                else if(sensorsCoefStates.temp1_b == 1)
                {
                    sensorsCoefStates.temp1_b = 0;
                    LCDsetCursor(7, 1);
                    LCD_Write_Str("B");
                }
                if(sensorsCoefStates.temp1_c == 1)
                {
                    sensorsCoefStates.temp1_c = 0;
                    LCDsetCursor(7, 1);
                    LCD_Write_Str("C");
                }
                if(sensorsCoefStates.temp1_d == 1)
                {
                    sensorsCoefStates.temp1_d = 0;
                    LCDsetCursor(7, 1);
                    LCD_Write_Str("D");
                }

            }
                // </editor-fold>

                // <editor-fold defaultstate="collapsed" desc="Pressure2 coef">
            else if((strncmp(input_line, "C04", 3) == 0))
            {
                LCDsetCursor(0, 1);
                LCD_Write_Str("P2 Coef  done");
                if(sensorsCoefStates.pres2_a == 1)
                {
                    sensorsCoefStates.pres2_a = 0;
                    LCDsetCursor(7, 1);
                    LCD_Write_Str("A");
                }
                else if(sensorsCoefStates.pres2_b == 1)
                {
                    sensorsCoefStates.pres2_b = 0;
                    LCDsetCursor(7, 1);
                    LCD_Write_Str("B");
                }
                if(sensorsCoefStates.pres2_c == 1)
                {
                    sensorsCoefStates.pres2_c = 0;
                    LCDsetCursor(7, 1);
                    LCD_Write_Str("C");
                }
                if(sensorsCoefStates.pres2_d == 1)
                {
                    sensorsCoefStates.pres2_d = 0;
                    LCDsetCursor(7, 1);
                    LCD_Write_Str("D");
                }
            }
                // </editor-fold>

                // <editor-fold defaultstate="collapsed" desc="Temperature2 coef">
            else if((strncmp(input_line, "C05", 3) == 0))
            {
                LCDsetCursor(0, 1);
                LCD_Write_Str("T2 Coef  done");
                if(sensorsCoefStates.temp2_a == 1)
                {
                    sensorsCoefStates.temp2_a = 0;
                    LCDsetCursor(7, 1);
                    LCD_Write_Str("A");
                }
                else if(sensorsCoefStates.temp2_b == 1)
                {
                    sensorsCoefStates.temp2_b = 0;
                    LCDsetCursor(7, 1);
                    LCD_Write_Str("B");
                }
                if(sensorsCoefStates.temp2_c == 1)
                {
                    sensorsCoefStates.temp2_c = 0;
                    LCDsetCursor(7, 1);
                    LCD_Write_Str("C");
                }
                if(sensorsCoefStates.temp2_d == 1)
                {
                    sensorsCoefStates.temp2_d = 0;
                    LCDsetCursor(7, 1);
                    LCD_Write_Str("D");
                }
            }
            // </editor-fold> 
        }
        // </editor-fold>

        // <editor-fold defaultstate="collapsed" desc="LCD Shutdown mode">
        if(lcdStates == LCD_SHUTDOWN)
        {
            LCDclear();
            LCDsetCursor(0, 0);
            LCD_Write_Str("Shutting down...");

            sysFlags.ledBlinkTask = false;
            sysFlags.uartTxTask = false;
            sysFlags.ads1115SampleTask = false;
            sysFlags.lcdTask = false;
            sysFlags.coefTask = false;
            sysFlags.pulseTask = false;
            txEnableFlag = false;
            TMR1_StopTimer();
            systemStatus.systemAllStates = 0;
            relayStatus.relay = 0;
            pulsetime = 0;
            __delay_ms(1000);
            LCDclear();
        }
        // <</editor-fold>
    }
    // </editor-fold>

    // <editor-fold defaultstate="collapsed" desc="Pulsetime Task">
    if(sysFlags.pulseTask == true)
    {
        sysFlags.pulseTask = false;
        lowByte = LCD_i2cRead(0x3B);
        highByte = LCD_i2cRead(0x3D);
        pulsetime = (uint16_t) ((highByte << 8) | (lowByte & 0xff));
    }
    // </editor-fold>

    // <editor-fold defaultstate="collapsed" desc="Serial MSG Processes">
    if(uartProcessFlag == true)
    {
        uartProcessFlag = false;

        // Time calibration
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
            keyword = "C02A";
        }

            // Pressure1 calibration coefB
        else if(strncmp(input_line, "C02B", 4) == 0)
        {
            serialMsgFlags = SER_MSG_FLAG_CAL_COEF;
            keyword = "C02B";
        }

            // Pressure1 calibration coefA
        else if(strncmp(input_line, "C02C", 4) == 0)
        {
            serialMsgFlags = SER_MSG_FLAG_CAL_COEF;
            keyword = "C02C";
        }

            // Pressure1 calibration coefD
        else if(strncmp(input_line, "C02D", 4) == 0)
        {
            serialMsgFlags = SER_MSG_FLAG_CAL_COEF;
            keyword = "C02D";
        }

            // Temperature1 calibration coefA
        else if(strncmp(input_line, "C03A", 4) == 0)
        {
            serialMsgFlags = SER_MSG_FLAG_CAL_COEF;
            keyword = "C03A";
        }

            // Temperature1 calibration coefB
        else if(strncmp(input_line, "C03B", 4) == 0)
        {
            serialMsgFlags = SER_MSG_FLAG_CAL_COEF;
            keyword = "C03B";
        }

            // Temperature1 calibration coefC
        else if(strncmp(input_line, "C03C", 4) == 0)
        {
            serialMsgFlags = SER_MSG_FLAG_CAL_COEF;
            keyword = "C03C";
        }

            // Temperature1 calibration coefD
        else if(strncmp(input_line, "C03D", 4) == 0)
        {
            serialMsgFlags = SER_MSG_FLAG_CAL_COEF;
            keyword = "C03D";
        }


            // Pressure2 calibration coefA
        else if(strncmp(input_line, "C04A", 4) == 0)
        {
            serialMsgFlags = SER_MSG_FLAG_CAL_COEF;
            keyword = "C04A";
        }

            // Pressure2 calibration coefB
        else if(strncmp(input_line, "C04B", 4) == 0)
        {
            serialMsgFlags = SER_MSG_FLAG_CAL_COEF;
            keyword = "C04B";
        }

            // Pressure2 calibration coefC
        else if(strncmp(input_line, "C04C", 4) == 0)
        {
            serialMsgFlags = SER_MSG_FLAG_CAL_COEF;
            keyword = "C04C";
        }

            // Pressure1 calibration coefD
        else if(strncmp(input_line, "C04D", 4) == 0)
        {
            serialMsgFlags = SER_MSG_FLAG_CAL_COEF;
            keyword = "C04D";
        }

            // Temperature2 calibration coefA
        else if(strncmp(input_line, "C05A", 4) == 0)
        {
            serialMsgFlags = SER_MSG_FLAG_CAL_COEF;
            keyword = "C05A";
        }

            // Temperature1 calibration coefB
        else if(strncmp(input_line, "C05B", 4) == 0)
        {
            serialMsgFlags = SER_MSG_FLAG_CAL_COEF;
            keyword = "C05B";
        }

            // Temperature1 calibration coefC
        else if(strncmp(input_line, "C05C", 4) == 0)
        {
            serialMsgFlags = SER_MSG_FLAG_CAL_COEF;
            keyword = "C05C";
        }

            // Temperature1 calibration coefD
        else if(strncmp(input_line, "C05D", 4) == 0)
        {
            serialMsgFlags = SER_MSG_FLAG_CAL_COEF;
            keyword = "C05D";
        }

            // Calibration mode enable
        else if(strncmp(input_line, "C06CalibrationY", 15) == 0)
        {
            uart1_puts("Calibration mode enabled\n");
            calibrationEnableFlag = true;
            lcdStates = LCD_CAL_MODE_Y;
            LCDclear();
        }

            // Calibration mode disable
        else if(strncmp(input_line, "C07CalibrationN", 15) == 0)
        {
            uart1_puts("Calibration mode disabled\n");
            calibrationEnableFlag = false;
            lcdStates = LCD_NORMAL_MODE;
            LCDclear();
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
            lcdStates = LCD_IDLE;
            LCDclear();
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

            // Write 4 bytes to EEPROM
        else if(strncmp(input_line, "C12", 3) == 0)
        {
            serialMsgFlags = SER_MSG_FLAG_EEPROMWrN;
        }

            // Read 4 bytes from EEPROM
        else if(strncmp(input_line, "C13", 3) == 0)
        {
            serialMsgFlags = SER_MSG_FLAG_EEPROMRdN;
        }

            // Store Time & Date to EEPROM
        else if(strncmp(input_line, "C14StoreRTC", 11) == 0)
        {
            serialMsgFlags = SER_MSG_FLAG_RTC_W;
            if(calibrationEnableFlag == true)
            {
                storeLastCalibRTC();
            }
        }

            // Get Time & Date from EEPROM (last calib time and date)
        else if(strncmp(input_line, "C15GetRTC", 9) == 0)
        {
            serialMsgFlags = SER_MSG_FLAG_RTC_R;
            getLastCalibRTC();
        }

            // Store FW info to EEPROM
        else if(strncmp(input_line, "C16", 3) == 0)
        {
            serialMsgFlags = SER_MSG_FLAG_FW_W;
        }

            // Get FW info from EEPROM
        else if(strncmp(input_line, "C17GetFW", 8) == 0)
        {
            getFWInfo();
        }

            // Start shutdown process
        else if(strncmp(input_line, "C18Shutdown", 11) == 0)
        {
            serialMsgFlags = SER_MSG_FLAG_IDLE;
            lcdStates = LCD_SHUTDOWN;
        }

            // LCD Idle mode
        else if(strncmp(input_line, "C19LCDIdle", 10) == 0)
        {
            uart1_puts("LCD Idle\n");
            serialMsgFlags = SER_MSG_FLAG_IDLE;
            lcdStates = LCD_IDLE;
            LCDclear();
        }

            // LCD Normal mode
        else if(strncmp(input_line, "C20LCDRun", 9) == 0)
        {
            uart1_puts("LCD Run\n");
            serialMsgFlags = SER_MSG_FLAG_IDLE;
            lcdStates = LCD_NORMAL_MODE;
            LCDclear();
        }

        else if(strncmp(input_line, "C21ResetCount", 13) == 0)
        {
            LATAbits.LATA5 = HIGH;
            relayStatus.pulsecounter = 1;
            TMR0_WriteTimer(TMR0_5MS);
            INTCONbits.TMR0IF = 0;
            INTCONbits.TMR0IE = 1;
            TMR0_StartTimer();
            serialMsgFlags = SER_MSG_FLAG_IDLE;
            lcdStates = LCD_NORMAL_MODE;
        }

        else if(strncmp(input_line, "C22Relay", 8) == 0)
        {
            serialMsgFlags = SER_MSG_FLAG_RELAY;
            process_data();
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
    // </editor-fold>

    // <editor-fold defaultstate="collapsed" desc="Buttons and PulseReset Task">
    if(tmr0Flag == true)
    {
        tmr0Flag = false;
        if(int1Flag == true)
        {
            int1Flag = false;
            if(PORTBbits.RB1 == PRESSED)
            {
                relayStatus.button1 ^= 1;
                LATAbits.LATA0 = relayStatus.button1;
            }
        }
        else if(int2Flag == true)
        {
            int2Flag = false;
            if(PORTBbits.RB2 == PRESSED)
            {
                relayStatus.button2 ^= 1;
                LATAbits.LATA1 = relayStatus.button2;
            }
        }
        else if(rb4Flag == true)
        {
            rb4Flag = false;
            if(PORTBbits.RB4 == PRESSED)
            {
                relayStatus.button3 ^= 1;
                LATAbits.LATA2 = relayStatus.button3;
            }
        }
        else if(rb5Flag == true)
        {
            rb5Flag = false;
            if(PORTBbits.RB5 == PRESSED)
            {
                relayStatus.button4 ^= 1;
                LATAbits.LATA3 = relayStatus.button4;
            }
        }
        else if(rb6Flag == true)
        {
            rb6Flag = false;
            if(PORTBbits.RB6 == PRESSED)
            {
                relayStatus.button5 ^= 1;
                LATAbits.LATA4 = relayStatus.button5;
            }
        }
        else if(relayStatus.pulsecounter == 1)
        {
            tmr0Flag = false;
            relayStatus.pulsecounter = 0;
            LATAbits.LATA5 = LOW;
            uart1_puts("Pulsetime Reset\n");
        }
    }
    // </editor-fold>
}

void ledBlink(void)
{
    LED_Toggle();
}

/*void ads1115SampleData(void)
{
    if(myADC.isPresent == 1)
    {
        if(channelNumber == 0)
        {
            voltage[0] = ADS1115_readSingleEnded(&myADC, 0);
        }
        if(channelNumber == 1)
        {
            voltage[1] = ADS1115_readSingleEnded(&myADC, 1);
        }
        if(channelNumber == 2)
        {
            voltage[2] = ADS1115_readSingleEnded(&myADC, 2);
        }
        if(channelNumber == 3)
        {
            voltage[3] = ADS1115_readSingleEnded(&myADC, 3);
        }
    }
}*/

void ds3232Timestamp(void)
{
    //    char *Day = "";
    //    char *MonthName = "";
    DS3231_getTime(&ds3231_readBuff[0], &ds3231_readBuff[1], &ds3231_readBuff[2]);
    DS3231_getDate(&ds3231_readBuff[3], &ds3231_readBuff[4], &ds3231_readBuff[5], &ds3231_readBuff[6]);
    //    Day = setDayName(ds3231_readBuff[3]);
    //    MonthName = setMonthName(ds3231_readBuff[5]);

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
}

void process_data(void)
{
    uint16_t crc = 0;
    uint8_t crc1 = 0; // crc1 = highbyte
    uint8_t crc2 = 0; // crc2 = lowbyte
    char parsedStrings[numOfChar][charLen];
    uint8_t dataPosition = 0;
    uint8_t dataCount = 0;
    const char delimiter[] = ",";
    char *keywordPointer;
    uint8_t i;

    // <editor-fold defaultstate="collapsed" desc="Time calibration task">
    if(serialMsgFlags == SER_MSG_FLAG_TIME)
    {
        keyword = "C00";
        keywordPointer = strstr(input_line, keyword);
        if(keywordPointer != NULL)
        {
            dataPosition = (keywordPointer - input_line) + strlen(keyword);
            dataCount = 0;

            char *token = strtok(&input_line[dataPosition], delimiter);
            if(token != NULL && strlen(token) < sizeof (parsedStrings[0]))
            {
                strncpy(parsedStrings[0], token, sizeof (parsedStrings[0]));
                dataCount++;
            }
            else
            {
                uart1_puts("Token to big\n");
                strcpy(parsedStrings[0], NULL);
            }

            for(i = 1; i < timePacketLen; i++)
            {
                token = strtok(NULL, delimiter);
                if(token != NULL && strlen(token) < sizeof (parsedStrings[i]))
                {
                    strncpy(parsedStrings[i], token, sizeof (parsedStrings[i]));
                    dataCount++;
                }
                else
                {
                    uart1_puts("Token to big\n");
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
                for(i = 5; i < NUM_OF_INT_Data; i++)
                {
                    integerData[i] = 0;
                }
                crc = checksumCalculator(integerData, 3);
                crc1 = (crc >> 8) & 0xff; // high byte
                crc2 = (crc >> 0) & 0xff; // low byte
                dataCount = 0;

                if(integerData[3] == 1 && integerData[4] == 1)
                {
                    timeFlag = true;
                    ds3232Timestamp();
                    uart1_puts("Time stored\n");
                    systemStatus.systemAllStates = 0;
                    systemStatus.rtcTimeFlag = true;
                }
                else
                {
                    uart1_puts("Invalid command\n");
                    serialMsgErrorFlag = true;
                    serialMsgFlags = 0;
                }
            }
            else if(dataCount < timePacketLen || dataCount > timePacketLen)
            {
                uart1_puts("Invalid command\n");
                serialMsgErrorFlag = true; // uart1_puts("data no good\n");
                dataCount = 0;
                serialMsgFlags = 0;
            }
        }
        else
        {
            uart1_puts("Invalid command\n");
            dataCount = 0;
            serialMsgFlags = 0;
        }
    }
    // </editor-fold>

    // <editor-fold defaultstate="collapsed" desc="Date calibration task">
    if(serialMsgFlags == SER_MSG_FLAG_DATE)
    {
        keyword = "C01";
        keywordPointer = strstr(input_line, keyword);
        if(keywordPointer != NULL)
        {
            dataPosition = (keywordPointer - input_line) + strlen(keyword);
            dataCount = 0;
            char *token = strtok(&input_line[dataPosition], delimiter);
            if(token != NULL && strlen(token) < sizeof (parsedStrings[0]))
            {
                strncpy(parsedStrings[0], token, sizeof (parsedStrings[0]));
                dataCount++;
            }
            else
            {
                uart1_puts("Token to big\n");
                strcpy(parsedStrings[0], NULL);
            }

            for(i = 1; i < datePacketLen; i++)
            {
                token = strtok(NULL, delimiter);
                if(token != NULL && strlen(token) < sizeof (parsedStrings[i]))
                {
                    strncpy(parsedStrings[i], token, sizeof (parsedStrings[i]));
                    dataCount++;
                }
                else
                {
                    uart1_puts("Token to big\n");
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
                for(i = 6; i < NUM_OF_INT_Data; i++)
                {
                    integerData[i] = 0;
                }
                crc = checksumCalculator(integerData, 4);
                crc1 = (crc >> 8) & 0xff; // high byte
                crc2 = (crc >> 0) & 0xff; // low byte
                dataCount = 0;

                if(integerData[4] == 1 && integerData[5] == 1)
                {
                    dateFlag = true;
                    ds3232Timestamp();
                    //                    lcdStates = LCD_CAL_RTC_D;
                    uart1_puts("Date stored\n");
                    systemStatus.systemAllStates = 0;
                    systemStatus.rtcDateFlag = true;
                }
                else
                {
                    uart1_puts("Invalid command\n");
                    serialMsgErrorFlag = true;
                    serialMsgFlags = 0;
                }
            }
            else if(dataCount < datePacketLen || dataCount > datePacketLen)
            {
                uart1_puts("Invalid command\n");
                serialMsgErrorFlag = true; // uart1_puts("data no good\n");
                dataCount = 0;
                serialMsgFlags = 0;
            }
        }
        else
        {
            uart1_puts("Invalid command\n");
            dataCount = 0;
            serialMsgFlags = 0;
        }
    }
    // </editor-fold>

    // <editor-fold defaultstate="collapsed" desc="Calibration coefficients task">
    if(serialMsgFlags == SER_MSG_FLAG_CAL_COEF)
    {
        keywordPointer = strstr(input_line, keyword);
        if(keywordPointer != NULL)
        {
            dataPosition = (keywordPointer - input_line) + strlen(keyword);
            dataCount = 0;
            char *token = strtok(&input_line[dataPosition], delimiter);
            if(token != NULL && strlen(token) < sizeof (parsedStrings[0]))
            {
                strncpy(parsedStrings[0], token, sizeof (parsedStrings[0]));
                dataCount++;
            }
            else
            {
                uart1_puts("Token to big\n");
                strcpy(parsedStrings[0], NULL);
            }

            for(i = 1; i < floatDataPacketLen; i++)
            {
                token = strtok(NULL, delimiter);
                if(token != NULL && strlen(token) < sizeof (parsedStrings[i]))
                {
                    strncpy(parsedStrings[i], token, sizeof (parsedStrings[i]));
                    dataCount++;
                }
                else
                {
                    uart1_puts("Token to big\n");
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
                for(i = 6; i < NUM_OF_INT_Data; i++)
                {
                    integerData[i] = 0;
                }
                crc = checksumCalculator(integerData, 4);
                crc1 = (crc >> 8) & 0xff; // high byte
                crc2 = (crc >> 0) & 0xff; // low byte
                dataCount = 0;


                // if(crc1 == integerData[3] && crc2 == integerData[4])  // below line will replaced by this line 
                if(integerData[4] == 1 && integerData[5] == 1) // for now, not checking crc
                {
                    unsigned long floatToLong;
                    float floatData;
                    EEPROM_byteToFloat(integerData, &floatData);

                    // <editor-fold defaultstate="collapsed" desc="Pressure1 coef">
                    if(keyword == "C02A")
                    {
                        pres1.coefA = floatData;
                        EEPROM_writeMultipleBytes(EEPROM_ADDR, 1, 0, &integerData[0], 4); // integerData
                        uart1_puts("CoefA_P1 done\n");
                        sensorsCoefStates.sensorsCoef = 0;
                        sensorsCoefStates.pres1_a = 1;
                    }
                    else if(keyword == "C02B")
                    {
                        pres1.coefB = floatData;
                        EEPROM_writeMultipleBytes(EEPROM_ADDR, 1, 4, &integerData[0], 4);
                        uart1_puts("CoefB_P1 done\n");
                        sensorsCoefStates.sensorsCoef = 0;
                        sensorsCoefStates.pres1_b = 1;
                    }
                    else if(keyword == "C02C")
                    {
                        pres1.coefC = floatData;
                        EEPROM_writeMultipleBytes(EEPROM_ADDR, 1, 8, &integerData[0], 4);
                        uart1_puts("CoefC_P1 done\n");
                        sensorsCoefStates.sensorsCoef = 0;
                        sensorsCoefStates.pres1_c = 1;
                    }
                    else if(keyword == "C02D")
                    {
                        pres1.coefD = floatData;
                        EEPROM_writeMultipleBytes(EEPROM_ADDR, 1, 12, &integerData[0], 4);
                        uart1_puts("CoefD_P1 done\n");
                        sensorsCoefStates.sensorsCoef = 0;
                        sensorsCoefStates.pres1_d = 1;
                    }
                        // </editor-fold>

                        // <editor-fold defaultstate="collapsed" desc="Temperature1 coef">
                    else if(keyword == "C03A")
                    {
                        temp1.coefA = floatData;
                        EEPROM_writeMultipleBytes(EEPROM_ADDR, 2, 0, &integerData[0], 4);
                        uart1_puts("CoefA_T1 done\n");
                        sensorsCoefStates.sensorsCoef = 0;
                        sensorsCoefStates.temp1_a = 1;
                    }
                    else if(keyword == "C03B")
                    {
                        temp1.coefB = floatData;
                        EEPROM_writeMultipleBytes(EEPROM_ADDR, 2, 4, &integerData[0], 4);
                        uart1_puts("CoefB_T1 done\n");
                        sensorsCoefStates.sensorsCoef = 0;
                        sensorsCoefStates.temp1_b = 1;
                    }
                    else if(keyword == "C03C")
                    {
                        temp1.coefC = floatData;
                        EEPROM_writeMultipleBytes(EEPROM_ADDR, 2, 8, &integerData[0], 4);
                        uart1_puts("CoefC_T1 done\n");
                        sensorsCoefStates.sensorsCoef = 0;
                        sensorsCoefStates.temp1_c = 1;
                    }
                    else if(keyword == "C03D")
                    {
                        temp1.coefD = floatData;
                        EEPROM_writeMultipleBytes(EEPROM_ADDR, 2, 12, &integerData[0], 4);
                        uart1_puts("CoefD_T1 done\n");
                        sensorsCoefStates.sensorsCoef = 0;
                        sensorsCoefStates.temp1_d = 1;
                    }
                        // </editor-fold>

                        // <editor-fold defaultstate="collapsed" desc="Pressure2 coef">
                    else if(keyword == "C04A")
                    {
                        pres2.coefA = floatData;
                        EEPROM_writeMultipleBytes(EEPROM_ADDR, 3, 0, integerData, 4);
                        uart1_puts("CoefA_P2 done\n");
                        sensorsCoefStates.sensorsCoef = 0;
                        sensorsCoefStates.pres2_a = 1;
                    }
                    else if(keyword == "C04B")
                    {
                        pres2.coefB = floatData;
                        EEPROM_writeMultipleBytes(EEPROM_ADDR, 3, 4, integerData, 4);
                        uart1_puts("CoefB_P2 done\n");
                        sensorsCoefStates.sensorsCoef = 0;
                        sensorsCoefStates.pres2_b = 1;
                    }
                    else if(keyword == "C04C")
                    {
                        pres2.coefC = floatData;
                        EEPROM_writeMultipleBytes(EEPROM_ADDR, 3, 8, integerData, 4);
                        uart1_puts("CoefC_P2 done\n");
                        sensorsCoefStates.sensorsCoef = 0;
                        sensorsCoefStates.pres2_c = 1;
                    }
                    else if(keyword == "C04D")
                    {
                        pres2.coefD = floatData;
                        EEPROM_writeMultipleBytes(EEPROM_ADDR, 3, 12, integerData, 4);
                        uart1_puts("CoefD_P2 done\n");
                        sensorsCoefStates.sensorsCoef = 0;
                        sensorsCoefStates.pres2_d = 1;
                    }
                        // </editor-fold>

                        // <editor-fold defaultstate="collapsed" desc="Temperature2 coef">
                    else if(keyword == "C05A")
                    {
                        temp2.coefA = floatData;
                        EEPROM_writeMultipleBytes(EEPROM_ADDR, 4, 0, integerData, 4);
                        uart1_puts("CoefA_T2 done\n");
                        sensorsCoefStates.sensorsCoef = 0;
                        sensorsCoefStates.temp2_a = 1;
                    }
                    else if(keyword == "C05B")
                    {
                        temp2.coefB = floatData;
                        EEPROM_writeMultipleBytes(EEPROM_ADDR, 4, 4, integerData, 4);
                        uart1_puts("CoefB_T2 done\n");
                        sensorsCoefStates.sensorsCoef = 0;
                        sensorsCoefStates.temp2_b = 1;
                    }
                    else if(keyword == "C05C")
                    {
                        temp2.coefC = floatData;
                        EEPROM_writeMultipleBytes(EEPROM_ADDR, 4, 8, integerData, 4);
                        uart1_puts("CoefC_T2 done\n");
                        sensorsCoefStates.sensorsCoef = 0;
                        sensorsCoefStates.temp2_c = 1;
                    }
                    else if(keyword == "C05D")
                    {
                        temp2.coefD = floatData;
                        EEPROM_writeMultipleBytes(EEPROM_ADDR, 4, 12, integerData, 4);
                        uart1_puts("CoefD_T2 done\n");
                        sensorsCoefStates.sensorsCoef = 0;
                        sensorsCoefStates.temp2_d = 1;
                    }
                    // </editor-fold>
                }
                else
                {
                    uart1_puts("Invalid command\n");
                    serialMsgErrorFlag = true;
                    serialMsgFlags = 0;
                }
            }
            else if(dataCount < floatDataPacketLen || dataCount > floatDataPacketLen)
            {
                uart1_puts("Invalid command\n");
                serialMsgErrorFlag = true; // uart1_puts("data no good\n");
                dataCount = 0;
                serialMsgFlags = 0;
            }
        }
        else
        {
            uart1_puts("Invalid command\n");
            dataCount = 0;
            serialMsgFlags = 0;
        }
    }
    // </editor-fold>

    // <editor-fold defaultstate="collapsed" desc="EEPROM write 1 byte task">
    if(serialMsgFlags == SER_MSG_FLAG_EEPROMWr1)
    {
        keyword = "C10";
        keywordPointer = strstr(input_line, keyword);
        if(keywordPointer != NULL)
        {
            dataPosition = (keywordPointer - input_line) + strlen(keyword);
            dataCount = 0;

            char *token = strtok(&input_line[dataPosition], delimiter);
            if(token != NULL && strlen(token) < sizeof (parsedStrings[0]))
            {
                strncpy(parsedStrings[0], token, sizeof (parsedStrings[0]));
                dataCount++;
            }
            else
            {
                uart1_puts("Token to big\n");
                strcpy(parsedStrings[0], NULL);
            }

            for(i = 1; i < eepromWr1PacketLen; i++)
            {
                token = strtok(NULL, delimiter);
                if(token != NULL && strlen(token) < sizeof (parsedStrings[i]))
                {
                    strncpy(parsedStrings[i], token, sizeof (parsedStrings[i]));
                    dataCount++;
                }
                else
                {
                    uart1_puts("Token to big\n");
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
                for(i = 5; i < NUM_OF_INT_Data; i++)
                {
                    integerData[i] = 0;
                }
                crc = checksumCalculator(integerData, 3);
                crc1 = (crc >> 8) & 0xff; // high byte
                crc2 = (crc >> 0) & 0xff; // low byte
                dataCount = 0;

                // if(crc1 == integerData[3] && crc2 == integerData[4])  // below line will replaced by this line
                if(integerData[3] == 1 && integerData[4] == 1) // for now, not checking crc
                {
                    EEPROM_write1ByteToPage(EEPROM_ADDR, integerData[0], integerData[1], integerData[2]);
                    systemStatus.systemAllStates = 0;
                    systemStatus.eepromWrBytes = true;
                    uart1_puts("EEPROM Wr done\n");
                }
                else
                {
                    uart1_puts("Invalid command\n");
                    serialMsgErrorFlag = true;
                    serialMsgFlags = 0;
                }
            }
            else if(dataCount < eepromWr1PacketLen || dataCount > eepromWr1PacketLen)
            {
                uart1_puts("Invalid command\n");
                serialMsgErrorFlag = true; // uart1_puts("data no good\n");
                dataCount = 0;
                serialMsgFlags = 0;
            }
        }
        else
        {
            uart1_puts("Invalid command\n");
            dataCount = 0;
            serialMsgFlags = 0;
        }
    }
    // </editor-fold>

    // <editor-fold defaultstate="collapsed" desc="EEPROM read 1 byte task">
    if(serialMsgFlags == SER_MSG_FLAG_EEPROMRd1)
    {
        keyword = "C11";
        keywordPointer = strstr(input_line, keyword);
        if(keywordPointer != NULL)
        {
            dataPosition = (keywordPointer - input_line) + strlen(keyword);
            dataCount = 0;

            char *token = strtok(&input_line[dataPosition], delimiter);
            if(token != NULL && strlen(token) < sizeof (parsedStrings[0]))
            {
                strncpy(parsedStrings[0], token, sizeof (parsedStrings[0]));
                dataCount++;
            }
            else
            {
                uart1_puts("Token to big\n");
                strcpy(parsedStrings[0], NULL);
            }

            for(i = 1; i < eepromRd1PacketLen; i++)
            {
                token = strtok(NULL, delimiter);
                if(token != NULL && strlen(token) < sizeof (parsedStrings[i]))
                {
                    strncpy(parsedStrings[i], token, sizeof (parsedStrings[i]));
                    dataCount++;
                }
                else
                {
                    uart1_puts("Token to big\n");
                    strcpy(parsedStrings[i], NULL);
                }
            }

            // convert ascii data into integer data           
            if(dataCount == eepromRd1PacketLen)
            {
                integerData[0] = (uint8_t) (atoi(parsedStrings[0])); // page
                integerData[1] = (uint8_t) (atoi(parsedStrings[1])); // offset
                for(i = 2; i < NUM_OF_INT_Data; i++)
                {
                    integerData[i] = 0;
                }
                //                crc = checksumCalculator(integerData, 2);
                //                crc1 = (crc >> 8) & 0xff; // high byte
                //                crc2 = (crc >> 0) & 0xff; // low byte
                dataCount = 0;
                uint8_t readByte = EEPROM_read1ByteFromPage(EEPROM_ADDR, integerData[0], integerData[1]);
                itoa(readByte, txBuffer, 10);
                uart1_puts(txBuffer);
                uart1_puts("\nEEPROM Rd done\n");
                systemStatus.systemAllStates = 0;
                systemStatus.eepromRdBytes = true;
            }
            else if(dataCount < eepromRd1PacketLen || dataCount > eepromRd1PacketLen)
            {
                uart1_puts("Invalid command\n");
                serialMsgErrorFlag = true;
                dataCount = 0;
                serialMsgFlags = 0;
            }
        }
        else
        {
            uart1_puts("Invalid command\n");
            dataCount = 0;
            serialMsgFlags = 0;
        }
    }
    // </editor-fold>

    // <editor-fold defaultstate="collapsed" desc="EEPROM write 4 bytes task">
    if(serialMsgFlags == SER_MSG_FLAG_EEPROMWrN)
    {
        keyword = "C12";
        keywordPointer = strstr(input_line, keyword);
        if(keywordPointer != NULL)
        {
            dataPosition = (keywordPointer - input_line) + strlen(keyword);
            dataCount = 0;

            char *token = strtok(&input_line[dataPosition], delimiter);
            if(token != NULL && strlen(token) < sizeof (parsedStrings[0]))
            {
                strncpy(parsedStrings[0], token, sizeof (parsedStrings[0]));
                dataCount++;
            }
            else
            {
                uart1_puts("Token to big\n");
                strcpy(parsedStrings[0], NULL);
            }

            for(i = 1; i < eepromWrNPacketLen; i++)
            {
                token = strtok(NULL, delimiter);
                if(token != NULL && strlen(token) < sizeof (parsedStrings[i]))
                {
                    strncpy(parsedStrings[i], token, sizeof (parsedStrings[i]));
                    dataCount++;
                }
                else
                {
                    uart1_puts("Token to big\n");
                    strcpy(parsedStrings[i], NULL);
                }
            }

            // convert ascii data into integer data           
            if(dataCount == eepromWrNPacketLen)
            {
                integerData[0] = (uint8_t) (atoi(parsedStrings[0])); // page
                integerData[1] = (uint8_t) (atoi(parsedStrings[1])); // offset
                integerData[2] = (uint8_t) (atoi(parsedStrings[2])); // data byte1 to write
                integerData[3] = (uint8_t) (atoi(parsedStrings[3])); // data byte2 to write
                integerData[4] = (uint8_t) (atoi(parsedStrings[4])); // data byte3 to write
                integerData[5] = (uint8_t) (atoi(parsedStrings[5])); // data byte4 to write
                integerData[6] = (uint8_t) (atoi(parsedStrings[6])); // CRC1
                integerData[7] = (uint8_t) (atoi(parsedStrings[7])); // CRC2
                for(i = 8; i < NUM_OF_INT_Data; i++)
                {
                    integerData[i] = 0;
                }
                crc = checksumCalculator(integerData, 6);
                crc1 = (crc >> 8) & 0xff;
                crc2 = (crc >> 0) & 0xff;
                dataCount = 0;

                // if(crc1 == integerData[3] && crc2 == integerData[4])  // below line will replaced by this line
                if(integerData[6] == 1 && integerData[7] == 1) // for now, not checking crc
                {
                    EEPROM_writeMultipleBytes(EEPROM_ADDR, integerData[0], integerData[1], &integerData[2], 4);
                    systemStatus.systemAllStates = 0;
                    systemStatus.eepromWrByte = true;
                    uart1_puts("EEPROM WrN done\n");
                }
                else
                {
                    uart1_puts("Invalid command\n");
                    serialMsgErrorFlag = true;
                    serialMsgFlags = 0;
                }
            }
            else if(dataCount < eepromWrNPacketLen || dataCount > eepromWrNPacketLen)
            {
                uart1_puts("Invalid command\n");
                serialMsgErrorFlag = true;
                dataCount = 0;
                serialMsgFlags = 0;
            }
        }
        else
        {
            uart1_puts("Invalid command\n");
            dataCount = 0;
            serialMsgFlags = 0;
        }
    }
    // </editor-fold>

    // <editor-fold defaultstate="collapsed" desc="EEPROM read 4 byte task">
    if(serialMsgFlags == SER_MSG_FLAG_EEPROMRdN)
    {
        keyword = "C13";
        keywordPointer = strstr(input_line, keyword);
        if(keywordPointer != NULL)
        {
            dataPosition = (keywordPointer - input_line) + strlen(keyword);
            dataCount = 0;

            char *token = strtok(&input_line[dataPosition], delimiter);
            if(token != NULL && strlen(token) < sizeof (parsedStrings[0]))
            {
                strncpy(parsedStrings[0], token, sizeof (parsedStrings[0]));
                dataCount++;
            }
            else
            {
                uart1_puts("Token to big\n");
                strcpy(parsedStrings[0], NULL);
            }

            for(uint8_t i = 1; i < eepromRdNPacketLen; i++)
            {
                token = strtok(NULL, delimiter);
                if(token != NULL && strlen(token) < sizeof (parsedStrings[i]))
                {
                    strncpy(parsedStrings[i], token, sizeof (parsedStrings[i]));
                    dataCount++;
                }
                else
                {
                    uart1_puts("Token to big\n");
                    strcpy(parsedStrings[i], NULL);
                }
            }

            // convert ascii data into integer data           
            if(dataCount == eepromRdNPacketLen)
            {
                integerData[0] = (uint8_t) (atoi(parsedStrings[0])); // page
                integerData[1] = (uint8_t) (atoi(parsedStrings[1])); // offset
                for(i = 2; i < NUM_OF_INT_Data; i++)
                {
                    integerData[i] = 0;
                }
                dataCount = 0;

                uint8_t readBytes[4];
                EEPROM_readMultipleBytes(EEPROM_ADDR, integerData[0], integerData[1], readBytes, 4);

                for(i = 0; i < 4; i++)
                {
                    itoa(readBytes[i], txBuffer, 10);
                    uart1_puts(txBuffer);
                    uart1_puts(" ");
                }
                uart1_puts("\nEEPROM RdN done\n");
            }
            else if(dataCount < eepromRdNPacketLen || dataCount > eepromRdNPacketLen)
            {
                uart1_puts("Invalid command\n");
                serialMsgErrorFlag = true;
                dataCount = 0;
                serialMsgFlags = 0;
            }
        }
        else
        {
            uart1_puts("Invalid command\n");
            dataCount = 0;
            serialMsgFlags = 0;
        }
    }
    // </editor-fold>

    // <editor-fold defaultstate="collapsed" desc="FW info write task">
    if(serialMsgFlags == SER_MSG_FLAG_FW_W)
    {
        uint8_t writeBytes[4];
        keyword = "C16";
        keywordPointer = strstr(input_line, keyword);
        if(keywordPointer != NULL)
        {
            dataPosition = (keywordPointer - input_line) + strlen(keyword);
            dataCount = 0;

            char *token = strtok(&input_line[dataPosition], delimiter);
            if(token != NULL && strlen(token) < sizeof (parsedStrings[0]))
            {
                strncpy(parsedStrings[0], token, sizeof (parsedStrings[0]));
                dataCount++;
            }
            else
            {
                uart1_puts("Token to big\n");
                strcpy(parsedStrings[0], NULL);
            }

            for(uint8_t i = 1; i < fwInfoWePacketLen; i++)
            {
                token = strtok(NULL, delimiter);
                if(token != NULL && strlen(token) < sizeof (parsedStrings[i]))
                {
                    strncpy(parsedStrings[i], token, sizeof (parsedStrings[i]));
                    dataCount++;
                }
                else
                {
                    uart1_puts("Token to big\n");
                    strcpy(parsedStrings[i], NULL);
                }
            }

            // convert ascii data into integer data           
            if(dataCount == fwInfoWePacketLen)
            {
                integerData[0] = (uint8_t) (atoi(parsedStrings[0])); // FW info
                integerData[1] = (uint8_t) (atoi(parsedStrings[1])); // CRC1
                integerData[2] = (uint8_t) (atoi(parsedStrings[2])); // CRC2
                for(i = 3; i < NUM_OF_INT_Data; i++)
                {
                    integerData[i] = 0;
                }
                dataCount = 0;

                crc = checksumCalculator(integerData[0], 1);
                crc1 = (crc >> 8) & 0xff;
                crc2 = (crc >> 0) & 0xff;

                //if(crc1 == integerData[0] && crc2 == integerData[1])
                if(integerData[1] == 1 && integerData[2] == 1)
                {
                    //                    EEPROM_writeMultipleBytes(EEPROM_ADDR, 0, 0, integerData[0], 1);
                    EEPROM_write1ByteToPage(EEPROM_ADDR, 0, 0, integerData[0]);
                    uart1_puts("FW stored\n");
                    systemStatus.systemAllStates = 0;
                    systemStatus.fwWr = true;
                }
                else
                {
                    uart1_puts("Invalid command\n");
                    serialMsgErrorFlag = true;
                    serialMsgFlags = 0;
                }
            }
            else if(dataCount < fwInfoWePacketLen || dataCount > fwInfoWePacketLen)
            {
                uart1_puts("Invalid command\n");
                serialMsgErrorFlag = true;
                dataCount = 0;
                serialMsgFlags = 0;
            }
        }
        else
        {
            uart1_puts("Invalid command\n");
            dataCount = 0;
            serialMsgFlags = 0;

        }
    }
    // </editor-fold>

    // <editor-fold defaultstate="collapsed" desc="Relay control task">
    if(serialMsgFlags == SER_MSG_FLAG_RELAY)
    {
        uint8_t writeBytes[4];
        keyword = "C22Relay";
        keywordPointer = strstr(input_line, keyword);
        if(keywordPointer != NULL)
        {
            dataPosition = (keywordPointer - input_line) + strlen(keyword);
            dataCount = 0;

            char *token = strtok(&input_line[dataPosition], delimiter);
            if(token != NULL && strlen(token) < sizeof (parsedStrings[0]))
            {
                strncpy(parsedStrings[0], token, sizeof (parsedStrings[0]));
                dataCount++;
            }
            else
            {
                uart1_puts("Token to big1\n");
                strcpy(parsedStrings[0], NULL);
            }

            for(uint8_t i = 1; i < relayPacketLen; i++)
            {
                token = strtok(NULL, delimiter);
                if(token != NULL && strlen(token) < sizeof (parsedStrings[i]))
                {
                    strncpy(parsedStrings[i], token, sizeof (parsedStrings[i]));
                    dataCount++;
                }
                else
                {
                    uart1_puts("Token to big2\n");
                    strcpy(parsedStrings[i], NULL);
                }
            }

            // convert ascii data into integer data           
            if(dataCount == relayPacketLen)
            {
                integerData[0] = (uint8_t) (atoi(parsedStrings[0])); // relay number with on/off state
                integerData[1] = (uint8_t) (atoi(parsedStrings[1])); // CRC1
                integerData[2] = (uint8_t) (atoi(parsedStrings[2])); // CRC2
                for(i = 3; i < NUM_OF_INT_Data; i++)
                {
                    integerData[i] = 0;
                }
                dataCount = 0;

                crc = checksumCalculator(integerData[0], 1);
                crc1 = (crc >> 8) & 0xff;
                crc2 = (crc >> 0) & 0xff;

                //if(crc1 == integerData[0] && crc2 == integerData[1])
                if(integerData[1] == 1 && integerData[2] == 1)
                {
                    uint8_t relayNumber = (uint8_t) (integerData[0] / 10);

                    switch(relayNumber)
                    {
                        case 1:
                            relayStatus.button1 = (integerData[0] % 10 == 0) ? LOW : HIGH;
                            LATAbits.LATA0 = relayStatus.button1;
                            break;
                        case 2:
                            relayStatus.button2 = (integerData[0] % 10 == 0) ? LOW : HIGH;
                            LATAbits.LATA1 = relayStatus.button2;
                            break;

                        case 3:
                            relayStatus.button3 = (integerData[0] % 10 == 0) ? LOW : HIGH;
                            LATAbits.LATA2 = relayStatus.button3;
                            break;
                        case 4:
                            relayStatus.button4 = (integerData[0] % 10 == 0) ? LOW : HIGH;
                            LATAbits.LATA3 = relayStatus.button4;
                            break;
                        case 5:
                            relayStatus.button5 = (integerData[0] % 10 == 0) ? LOW : HIGH;
                            LATAbits.LATA4 = relayStatus.button5;
                            break;
                        default:
                            break;
                    }
                    uart1_puts("RelayCommand\n");
                }
                else
                {
                    uart1_puts("Invalid command\n");
                    serialMsgErrorFlag = true;
                    serialMsgFlags = 0;
                }
            }
            else if(dataCount < relayPacketLen || dataCount > relayPacketLen)
            {
                uart1_puts("Invalid command\n");
                serialMsgErrorFlag = true;
                dataCount = 0;
                serialMsgFlags = 0;
            }
        }
        else
        {
            uart1_puts("Invalid command\n");
            dataCount = 0;
            serialMsgFlags = 0;
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
            uart1_puts("Invalid command\n");
            lcdStates = LCD_INVALID_INPUT;
        }
    }
    // </editor-fold>
    // end Date calibration task
}

// <editor-fold defaultstate="collapsed" desc="Functions: CRC, itoa, ltoa, float to string">

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

// convert float to string one decimal digit at a time
// assumes float is < 65536 and ARRAYSIZE is big enough
// problem: it truncates numbers at size without rounding
// str is a char array to hold the result, float is the number to convert
// size is the number of decimal digits you want

void FloatToStringNew(char *str, float f, char size)
{
    char pos; // position in string
    char len; // length of decimal part of result
    char* curr; // temp holder for next digit
    int value; // decimal digit(s) to convert

    pos = 0; // initialize pos, just to be sure
    value = (int) f; // truncate the floating point number
    itoa(value, str, 10); // this is kinda dangerous depending on the length of str
    // now str array has the digits before the decimal

    if(f < 0) // handle negative numbers
    {
        f *= -1;
        value *= -1;
    }

    len = strlen(str); // find out how big the integer part was
    pos = len; // position the pointer to the end of the integer part
    str[pos++] = '.'; // add decimal point to string

    while(pos < (size + len + 1)) // process remaining digits
    {
        f = f - (float) value; // hack off the whole part of the number
        f *= 10; // move next digit over
        value = (int) f; // get next digit
        itoa(value, curr, 10); // convert digit to string
        str[pos++] = *curr; // add digit to result string and increment pointer
    }
}

void ulongToByte(uint32_t ulongData, uint8_t *pByteData)
{
    uint8_t *pointer_to_ulong;
    pointer_to_ulong = (uint8_t *) & ulongData;
    pByteData[0] = *pointer_to_ulong++;
    pByteData[1] = *pointer_to_ulong++;
    pByteData[2] = *pointer_to_ulong++;
    pByteData[3] = *pointer_to_ulong;
}

void floatToByte(float floatData, uint8_t *pByteData)
{
    uint8_t *pointer_to_float;
    pointer_to_float = (uint8_t *) & floatData;
    pByteData[0] = *pointer_to_float++;
    pByteData[1] = *pointer_to_float++;
    pByteData[2] = *pointer_to_float++;
    pByteData[3] = *pointer_to_float;
}
// </editor-fold>