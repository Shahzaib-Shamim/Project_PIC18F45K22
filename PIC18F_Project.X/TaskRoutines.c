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

volatile TIMER1_t timer1 = {
    .timer1_1msFlag = false,
    .timer1_1msCounter = 0
};

ADS1115_Object_t myADC;
char txBuffer[50] = {0};
char lcdBuff[8] = {0};
volatile ApplicationFlags_t sysFlags;
uint8_t serialMsgFlags = SER_MSG_FLAG_IDLE;
uint8_t serialMsgErrorFlag = false;
uint8_t integerData[NUM_OF_INT_Data];
char *keyword;
bool dateFlag = false;
bool timeFlag = false;
bool txEnableFlag = true; // false
uint8_t eepromStateFlags = 0;
bool calibrationEnableFlag = true; //false
uint8_t ds3231_readBuff[7];
uint32_t time_ccp = 0;
uint8_t lcdStates = 0;
bool systemStates = 0;
float voltage[4];
float calibData[4];
uint16_t *adcData;
char str[10] = {0};
char *p = str;

union SystemState_u
{
    struct
    {
        unsigned rtcTimeFlag    : 1;
        unsigned rtcDateFlag    : 1;
        unsigned eepromWrBytes  : 1;
        unsigned eepromRdBytes  : 1;
        unsigned eepromWrByte   : 1;
        unsigned eepromRdByte   : 1;
        unsigned fwWr           : 1;
        unsigned fwRd           : 1;
    };
    uint16_t systemAllStates;
};
union SystemState_u systemStatus;

union RelayState_u
{
    struct
    {
        unsigned button1 : 1;
        unsigned button2 : 1;
        unsigned button3 : 1;
        unsigned button4 : 1;
        unsigned button5 : 1;
        unsigned : 1;
        unsigned : 1;
        unsigned : 1;
    };
    uint8_t relay;
};
union RelayState_u relayStatus;

union sensorCalibrationCoef_u
{
    struct
    {
        unsigned pres1_a : 1;
        unsigned pres1_b : 1;
        unsigned pres1_c : 1;
        unsigned pres1_d : 1;
        unsigned temp1_a: 1;
        unsigned temp1_b: 1;
        unsigned temp1_c: 1;
        unsigned temp1_d: 1;
        unsigned pres2_a : 1;
        unsigned pres2_b : 1;
        unsigned pres2_c : 1;
        unsigned pres2_d : 1;
        unsigned temp2_a: 1;
        unsigned temp2_b: 1;
        unsigned temp2_c: 1;
        unsigned temp2_d: 1;
    };
    uint16_t sensorsCoef;
};
union sensorCalibrationCoef_u sensorsCoefStates;
    
typedef struct
{
    float coefA;
    float coefB;
    float coefC;
    float coefD;
} calFactors;
calFactors pres1 = {.coefA = 1.0f, .coefB = 1.0f, .coefC = 1.0f, .coefD = 1.0f},
           temp1 = {.coefA = 1.0f, .coefB = 1.0f, .coefC = 1.0f, .coefD = 1.0f},
           pres2 = {.coefA = 1.0f, .coefB = 1.0f, .coefC = 1.0f, .coefD = 1.0f},
           temp2 = {.coefA = 1.0f, .coefB = 1.0f, .coefC = 1.0f, .coefD = 1.0f};

void initSysVariables(void)
{
    sysFlags.ledBlinkTask = false;
    sysFlags.uartTxTask = false;
    sysFlags.ads1115SampleTask = false;
    sysFlags.ds3231Task = false;
    sysFlags.eepromTask = false;
    sysFlags.lcdTask = false;

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

void loadPres1_Coef(void)
{
    uint8_t eepromReadBuff[4];
    
    EEPROM_readMultipleBytes(EEPROM_ADDR, 1, 0, eepromReadBuff, 4);
//    pres1.coefA = EEPROM_byteToFloat(eepromReadBuff);
    EEPROM_byteToFloat(eepromReadBuff, &pres1.coefA);
    EEPROM_readMultipleBytes(EEPROM_ADDR, 1, 4, eepromReadBuff, 4);
//    pres1.coefB = EEPROM_byteToFloat(eepromReadBuff);
    EEPROM_byteToFloat(eepromReadBuff, &pres1.coefB);
    EEPROM_readMultipleBytes(EEPROM_ADDR, 1, 8, eepromReadBuff, 4);
//    pres1.coefC = EEPROM_byteToFloat(eepromReadBuff); 
    EEPROM_byteToFloat(eepromReadBuff, &pres1.coefC);
    EEPROM_readMultipleBytes(EEPROM_ADDR, 1, 12, eepromReadBuff, 4);
//    pres1.coefD = EEPROM_byteToFloat(eepromReadBuff);
    EEPROM_byteToFloat(eepromReadBuff, &pres1.coefD);
}

void loadTemp1_Coef(void)
{
    uint8_t eepromReadBuff[4];
    
    EEPROM_readMultipleBytes(EEPROM_ADDR, 2, 0, eepromReadBuff, 4);
//    temp1.coefA = EEPROM_byteToFloat(eepromReadBuff);
    EEPROM_byteToFloat(eepromReadBuff, &temp1.coefA);
    EEPROM_readMultipleBytes(EEPROM_ADDR, 2, 4, eepromReadBuff, 4);
//    temp1.coefB = EEPROM_byteToFloat(eepromReadBuff);
    EEPROM_byteToFloat(eepromReadBuff, &temp1.coefB);
    EEPROM_readMultipleBytes(EEPROM_ADDR, 2, 8, eepromReadBuff, 4);
//    temp1.coefC = EEPROM_byteToFloat(eepromReadBuff); 
    EEPROM_byteToFloat(eepromReadBuff, &temp1.coefC);
    EEPROM_readMultipleBytes(EEPROM_ADDR, 2, 12, eepromReadBuff, 4);
//    temp1.coefD = EEPROM_byteToFloat(eepromReadBuff);
    EEPROM_byteToFloat(eepromReadBuff, &temp1.coefD);
}

void loadPres2_Coef(void)
{
    uint8_t eepromReadBuff[4];
    
    EEPROM_readMultipleBytes(EEPROM_ADDR, 3, 0, eepromReadBuff, 4);
//    pres2.coefA = EEPROM_byteToFloat(eepromReadBuff);
    EEPROM_byteToFloat(eepromReadBuff, &pres2.coefD);
    EEPROM_readMultipleBytes(EEPROM_ADDR, 3, 4, eepromReadBuff, 4);
//    pres2.coefB = EEPROM_byteToFloat(eepromReadBuff);
    EEPROM_byteToFloat(eepromReadBuff, &pres2.coefD);
    EEPROM_readMultipleBytes(EEPROM_ADDR, 3, 8, eepromReadBuff, 4);
//    pres2.coefC = EEPROM_byteToFloat(eepromReadBuff); 
    EEPROM_byteToFloat(eepromReadBuff, &pres2.coefD);
    EEPROM_readMultipleBytes(EEPROM_ADDR, 3, 12, eepromReadBuff, 4);
//    pres2.coefD = EEPROM_byteToFloat(eepromReadBuff);
    EEPROM_byteToFloat(eepromReadBuff, &pres2.coefD);
}

void loadTemp2_Coef(void)
{
    uint8_t eepromReadBuff[4];
    
    EEPROM_readMultipleBytes(EEPROM_ADDR, 4, 0, eepromReadBuff, 4);
//    temp2.coefA = EEPROM_byteToFloat(eepromReadBuff);
    EEPROM_byteToFloat(eepromReadBuff, &temp2.coefD);
    EEPROM_readMultipleBytes(EEPROM_ADDR, 4, 4, eepromReadBuff, 4);
//    temp2.coefB = EEPROM_byteToFloat(eepromReadBuff);
    EEPROM_byteToFloat(eepromReadBuff, &temp2.coefD);
    EEPROM_readMultipleBytes(EEPROM_ADDR, 4, 8, eepromReadBuff, 4);
//    temp2.coefC = EEPROM_byteToFloat(eepromReadBuff);
    EEPROM_byteToFloat(eepromReadBuff, &temp2.coefD);
    EEPROM_readMultipleBytes(EEPROM_ADDR, 4, 12, eepromReadBuff, 4);
//    temp2.coefD = EEPROM_byteToFloat(eepromReadBuff);
    EEPROM_byteToFloat(eepromReadBuff, &temp2.coefD);
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
      
    DS3231_getData(&readBuff[0], &readBuff[1], &readBuff[2],
                   &readBuff[3], &readBuff[4], &readBuff[5], &readBuff[6]);
    EEPROM_writeMultipleBytes(EEPROM_ADDR, 0, 4, readBuff, 7);
//    uart1_puts("To EEPROM\n");
}

void getLastCalibRTC(void)
{
    uint8_t readBuff[7];
//    DS3231_getData(&readBuff[0], &readBuff[1], &readBuff[2],
//                   &readBuff[3], &readBuff[4], &readBuff[5], &readBuff[6]);
    
    EEPROM_readMultipleBytes(EEPROM_ADDR, 0, 4, readBuff, 7);
    
    /*itoa(readBuff[2], txBuffer, 10);
    uart1_puts(txBuffer);
    uart1_putc(':');
    itoa(readBuff[1], txBuffer, 10);
    uart1_puts(txBuffer);
    uart1_putc(':');
    itoa(readBuff[0], txBuffer, 10);
    uart1_puts(txBuffer);
    uart1_putc(' ');
    
    itoa(readBuff[3], txBuffer, 10);
    uart1_puts(txBuffer);     //Day
    uart1_putc(',');
    itoa(readBuff[4], txBuffer, 10);
    uart1_puts(txBuffer);
    uart1_putc('/');
    itoa(readBuff[5], txBuffer, 10);
    uart1_puts(txBuffer);   //MonthName
    uart1_putc('/');
    itoa(readBuff[6], txBuffer, 10);
    uart1_puts(txBuffer);
    uart1_newline();
    uart1_puts("From EEPROM\n");*/
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

void startInitialization(void)
{
//    loadPres1_Coef();
//    loadTemp1_Coef();
//    loadPres2_Coef();
//    loadTemp2_Coef();
    
    load_Coef();
}

void runApplication(void)
{
    if(sysFlags.ledBlinkTask == true)
    {
        sysFlags.ledBlinkTask = false;
        ledBlink();
    }

    if(sysFlags.uartTxTask == true && txEnableFlag == true)
    {
        sysFlags.uartTxTask = false;
        
        /*static uint8_t i = 0;
        i++;
        if(i == 40)
        {
            i = 0;
//            ds3232Timestamp();
//            ads1115SampleData();
        }*/       
    }

//    if(sysFlags.ads1115SampleTask == true && txEnableFlag == true)
//    {
//        sysFlags.ads1115SampleTask = false;
//        //        ads1115SampleData();
//    }

    //    if(sysFlags.ds3231Task == true && serialMsgFlags == txEnableFlag == true)
//    if(sysFlags.ds3231Task == true && txEnableFlag == true)
//    {
//        sysFlags.ds3231Task = false;
////                ds3232Timestamp();
//    }

//    if(sysFlags.eepromTask == true)
//    {
//        sysFlags.eepromTask = false;
//    }

    if(sysFlags.lcdTask == true)
    {
        sysFlags.lcdTask = false;
//        uint8_t readBuffRTC[7];
        char *MonthName = "";
        
        // <editor-fold defaultstate="collapsed" desc="LCD Idle mode">
        if(lcdStates == LCD_IDLE)
        {
            // readBuffTRTC --> ds3231_readBuff
            DS3231_getTime(&ds3231_readBuff[0], &ds3231_readBuff[1], &ds3231_readBuff[2]);
            DS3231_getDate(&ds3231_readBuff[3], &ds3231_readBuff[4], &ds3231_readBuff[5], &ds3231_readBuff[6]);
            
            LCDsetCursor(0, 0);
            LCD_Write_Str("Idle...");
            LCDsetCursor(0, 1);
            LCD_Write_Str("Waiting for command");
                    
            LCDsetCursor(0, 3);
            itoa(ds3231_readBuff[2], lcdBuff, 10); LCD_Write_Str(lcdBuff); LCD_Write_Str(":");
            itoa(ds3231_readBuff[1], lcdBuff, 10); LCD_Write_Str(lcdBuff); LCD_Write_Str(":");
            itoa(ds3231_readBuff[0], lcdBuff, 10); LCD_Write_Str(lcdBuff);        
            LCDsetCursor(9, 3);
            itoa(ds3231_readBuff[4], lcdBuff, 10); LCD_Write_Str(lcdBuff); LCD_Write_Str("-");
            MonthName = setMonthName(ds3231_readBuff[5]);
            /*itoa(readBuffRTC[5], lcdBuff, 10);*/ LCD_Write_Str(MonthName); LCD_Write_Str("-");
//            itoa(ds3231_readBuff[5], lcdBuff, 10); LCD_Write_Str(lcdBuff); LCD_Write_Str("-");
            itoa(ds3231_readBuff[6], lcdBuff, 10); LCD_Write_Str(lcdBuff);
        }
        // </editor-fold>
        
        // <editor-fold defaultstate="collapsed" desc="LCD Normal mode">
        if(lcdStates == LCD_NORMAL_MODE)
        {
            LCDclear();
            
            LCDsetCursor(0, 0);
            LCD_Write_Str("Pulse=");
//            ltoa(time_ccp / 1000, lcdBuff, 10); LCD_Write_Str(lcdBuff);
            //added start
            uint8_t lowByte, highByte;
            uint16_t pulsetime;
//            if(1 == 1)
//            {
//                int0Flag = false;
                lowByte = LCD_i2cRead(0x3B);
                highByte = LCD_i2cRead(0x3D);
                pulsetime = (uint16_t)((highByte << 8) | (lowByte & 0xff));
                LCDsetCursor(0, 6);
                ltoa(pulsetime, lcdBuff, 10);    // ltoa, / 1000
                LCD_Write_Str(lcdBuff);
                
                
                
                
//                uart1_puts(lcdBuff);
//                LCD_Write_Str(".");
//                uart1_puts(".");
//                ltoa(pulsetime % 1000, lcdBuff, 10);    // ltoa
//                LCD_Write_Str(lcdBuff);
//                uart1_puts(lcdBuff);
//                uart1_puts("sec");
//                uart1_newline();
//                ltoa(pulsetime, lcdBuff, 10);    // ltoa
//                LCD_Write_Str(lcdBuff);
                
//                LCDsetCursor(0, 6);
//                LCD_Write_Str("       ");
//                LCDsetCursor(0, 6);
//                ltoa(120, lcdBuff, 10);    // ltoa
//                LCD_Write_Str(lcdBuff);
//                uart1_puts(lcdBuff);
//                uart1_newline();
                
//            }
            
            //added stop
            LCDsetCursor(16, 0);
            LCD_Write_Str("sec");
//            ltoa(time_ccp / 1000, lcdBuff, 10);
//            uart1_puts(lcdBuff);
//            uart1_puts("ms\n");
//            
            ads1115SampleData();
            *adcData = calibData;
            
            // ok code
            LCDsetCursor(0, 1);
            LCD_Write_Str("P1=");
             FloatToStringNew(p, calibData[0], 3);
            LCD_Write_Str(str);
            LCDsetCursor(16, 1);
            LCD_Write_Str("PSI");
            
//            LCDsetCursor(0, 2);
//            LCD_Write_Str("T1=");
//            ltoa(voltage[1] * 1000, lcdBuff, 10); LCD_Write_Str(lcdBuff);
//            LCDsetCursor(16, 2);
//            LCD_Write_Str("degC");
            LCDsetCursor(0, 2);
            LCD_Write_Str("T1=");
            FloatToStringNew(p, calibData[1], 3);
            LCD_Write_Str(str);
            LCDsetCursor(16, 2);
            LCD_Write_Str("degC");
            
            
            /*LCDsetCursor(0, 1);
            LCD_Write_Str("P1=");
            ltoa(1 * 1000, lcdBuff, 10); LCD_Write_Str(lcdBuff);
            LCDsetCursor(16, 1);
            LCD_Write_Str("PSI");
            
            LCDsetCursor(0, 2);
            LCD_Write_Str("T1=");
            ltoa(2 * 1000, lcdBuff, 10); LCD_Write_Str(lcdBuff);
            LCDsetCursor(16, 2);
            LCD_Write_Str("degC");*/
            
            // this code is ok to send over uart
            /*for(uint8_t i = 0; i < 4; i++)
            {
                uint16_t adcData;
                adcData = calibData[i] * 1000;
                itoa(adcData, txBuffer, 10);
                uart1_puts(txBuffer);
                uart1_puts("    ");
                adcData = voltage[i] * 1000;
                itoa(adcData, txBuffer, 10);
                uart1_puts(txBuffer);
                uart1_newline();            
            }
            uart1_puts("--------------------------");
            uart1_newline();*/
            
            LCDsetCursor(0, 3);
            LCD_Write_Str("Relay#12345=");
            
            LCDsetCursor(19, 3);
            itoa(relayStatus.button1, lcdBuff, 2); LCD_Write_Str(lcdBuff);
            
            LCDsetCursor(18, 3);
            itoa(relayStatus.button2, lcdBuff, 2); LCD_Write_Str(lcdBuff);
            
            LCDsetCursor(17, 3);
            itoa(relayStatus.button3, lcdBuff, 2); LCD_Write_Str(lcdBuff);
            
            LCDsetCursor(16, 3);
            itoa(relayStatus.button4, lcdBuff, 2); LCD_Write_Str(lcdBuff);
            
            LCDsetCursor(15, 3);
            itoa(relayStatus.button5, lcdBuff, 2); LCD_Write_Str(lcdBuff);
            
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
//            sysFlags.ledBlinkTask = false;
//            sysFlags.uartTxTask = false;
//            sysFlags.ads1115SampleTask = false;
//            sysFlags.ds3231Task = false;
//            sysFlags.eepromTask = false;
//            sysFlags.lcdTask = false;
            
        }
        // <</editor-fold>
    }

    // --Serial Msg process-----------------------------------------------------
    if(uartProcessFlag == true)
    {
        uartProcessFlag = false;

        // <editor-fold defaultstate="collapsed" desc="Serial msg states">
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
        else if(strncmp(input_line, "C06CalibrationY", 15) == 0) //C06
        {
            uart1_puts("Calibration mode enabled\n");
            calibrationEnableFlag = true;
            lcdStates = LCD_CAL_MODE_Y;
            LCDclear();   // comment abhi
        }

        // Calibration mode disable
        else if(strncmp(input_line, "C07CalibrationN", 15) == 0)
        {
            uart1_puts("Calibration mode disabled\n");
            calibrationEnableFlag = false;
            lcdStates = LCD_CAL_MODE_N;
            LCDclear();
//            LCDsetCursor(0, 0);
//            LCD_Write_Str("Calibration mode:");
//            LCDsetCursor(0, 1);
//            LCD_Write_Str("Done");
            lcdStates = LCD_NORMAL_MODE;
//            LCDclear();

            /*LCDclear();
            LCDsetCursor(0, 0);
            LCD_Write_Str("Pulse=");
            LCDsetCursor(16, 0);
            LCD_Write_Str("sec");

            LCDsetCursor(0, 1);
            LCD_Write_Str("P1=");
            LCDsetCursor(16, 1);
            LCD_Write_Str("PSI");

            LCDsetCursor(0, 2);
            LCD_Write_Str("T1=");
            LCDsetCursor(16, 2);
            LCD_Write_Str("degC");

            LCDsetCursor(0, 3);
            LCD_Write_Str("Relay#12345=");*/
            
        }

        // Start sending data
        else if(strncmp(input_line, "C08StartTX", 10) == 0)
        {
            txEnableFlag = true;
//            lcdStates = LCD_NORMAL_MODE;
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
            serialMsgFlags = SER_MSG_FLAG_IDLE;
            lcdStates = LCD_IDLE;
            LCDclear();
        }
        
        // LCD Normal mode
        else if(strncmp(input_line, "C20LCDRun", 9) == 0)
        {
            serialMsgFlags = SER_MSG_FLAG_IDLE;
            lcdStates = LCD_NORMAL_MODE;
            LCDclear();
            /*LCDsetCursor(0, 0);
            LCD_Write_Str("Pulse=");
            LCDsetCursor(16, 0);
            LCD_Write_Str("sec");

            LCDsetCursor(0, 1);
            LCD_Write_Str("P1=");
            LCDsetCursor(16, 1);
            LCD_Write_Str("PSI");

            LCDsetCursor(0, 2);
            LCD_Write_Str("T1=");
            LCDsetCursor(16, 2);
            LCD_Write_Str("degC");

            LCDsetCursor(0, 3);
            LCD_Write_Str("Relay#12345=");*/
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
        // </editor-fold>
    }
    
    // <editor-fold defaultstate="collapsed" desc="CCP2 task">
    if(ccp2Flag == true)
    {
        ccp2Flag = false;

//        time_ccp = time_after_capture(cap_overflow_tmr3, &capture_value);
//        ltoa(time_ccp/1000, txBuffer, 10);
//        uart1_puts("Pulsetime=");
//        uart1_puts(txBuffer);
//        uart1_newline();

        // not correct
//        ltoa(time_ccp, txBuffer, 10);
//        uart1_puts("Pulsetime=");
//        uart1_puts(txBuffer);
//        uart1_newline();

//            cap_overflow_tmr3 = 0;
//            capture_value.ccpL = 0;
//            capture_value.ccpH = 0;

    }
    // </editor-fold>
    
    // <editor-fold defaultstate="collapsed" desc="Button reading task">
    if(int1Flag == true && tmr0Flag == true)
    {
        tmr0Flag = false;
        int1Flag = false;
        if(PORTBbits.RB1 == PRESSED)
        {
            relayStatus.button1 ^= 1;
//            itoa(relayStatus.relay, txBuffer, 2);
//            uart1_puts(txBuffer);
//            uart1_newline();
        }
    }
    else if(int2Flag == true && tmr0Flag == true)
    {
        tmr0Flag = false;
        int2Flag = false;
        if(PORTBbits.RB2 == PRESSED)
        {
            relayStatus.button2 ^= 1;
//            itoa(relayStatus.relay, txBuffer, 2);
//            uart1_puts(txBuffer);
//            uart1_newline();
        }
    }
    else if(rb4Flag == true && tmr0Flag == true)
    {
        tmr0Flag = false;
        rb4Flag = false;
        if(PORTBbits.RB4 == PRESSED)
        {
            relayStatus.button3 ^= 1;
//            itoa(relayStatus.relay, txBuffer, 2);
//            uart1_puts(txBuffer);
//            uart1_newline();
        }

    }
    else if(rb5Flag == true && tmr0Flag == true)
    {
        tmr0Flag = false;
        rb5Flag = false;
        if(PORTBbits.RB5 == PRESSED)
        {
            relayStatus.button4 ^= 1;
//            itoa(relayStatus.relay, txBuffer, 2);
//            uart1_puts(txBuffer);
//            uart1_newline();
        }
    }
    else if(rb6Flag == true && tmr0Flag == true)
    {
        tmr0Flag = false;
        rb6Flag = false;
        if(PORTBbits.RB6 == PRESSED)
        {
            relayStatus.button5 ^= 1;
//            itoa(relayStatus.relay, txBuffer, 2);
//            uart1_puts(txBuffer);
//            uart1_newline();
        }
    }
    // </editor-fold>
}

void ledBlink(void)
{
    LED_Toggle();
}

void ads1115SampleData(void)
{
//    float *pfloatData = voltage;
    
    if(myADC.isPresent == 1)
    {
        for(uint8_t i = 0; i < 4; i++)
        {
            voltage[i] = ADS1115_readSingleEnded(&myADC, i);
//            ADS1115_readSingleEnded(&myADC, i, &voltage[i]);
            // __delay_us(10);
        }

//        int adcData;
        
//        uint8_t i;
          
        // send calibrated data and raw data
//        if(calibrationEnableFlag == true)       
//        {
            /**
             * calibration equation using 4 polynomials
             * y = ax3 + bx2 + cx + d       (x = adc value in float (0.000 to 5.000V)
             */
            calibData[0] = (pres1.coefA * voltage[0] * voltage[0] * voltage[0]) +
                    (pres1.coefB * voltage[0] * voltage[0]) + (pres1.coefC * voltage[0]) + pres1.coefD;
            calibData[1] = (temp1.coefA * voltage[1] * voltage[1] * voltage[1]) +
                    (temp1.coefB * voltage[1] * voltage[1]) + (temp1.coefC * voltage[1]) + temp1.coefD;
            calibData[2] = (pres2.coefA * voltage[2] * voltage[2] * voltage[2]) +
                    (pres2.coefB * voltage[2] * voltage[2]) + (pres2.coefC * voltage[2]) + pres2.coefD;
            calibData[3] = (temp2.coefA * voltage[3] * voltage[3] * voltage[3]) +
                    (temp2.coefB * voltage[3] * voltage[3]) + (temp2.coefC * voltage[3]) + temp2.coefD;            
//        }
        
//        for(i = 0; i < 4; i++)
//        {
//            adcData[i] = calibData[i] * 1000;
//            itoa(adcData[i], txBuffer, 10);
//            uart1_puts(txBuffer);
//            uart1_puts("    ");
//            adcData[i] = voltage[i] * 1000;
//            itoa(adcData[i], txBuffer, 10);
//            uart1_puts(txBuffer);
//            uart1_newline();            
//        }
//        uart1_puts("--------------------------");
//        uart1_newline();
    }
}

void ds3232Timestamp(void)
{
//    char *Day = "";
//    char *MonthName = "";
    //    uint8_t ds3231_readBuff[7];
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
    uint8_t crc1, crc2;                     // crc1 = highbyte, crc2 = lowbyte
//    char parsedStrings[charLen][numOfChar];
    char parsedStrings[numOfChar][charLen];
    uint8_t dataPosition = 0;
    uint8_t dataCount = 0;
    const char delimiter[] = ",";
    char *keywordPointer;
    uint8_t i;
//    uint8_t serialMsgErrorFlag = false;

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
                // uart_puts("token to big1\n");
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
//                    uart1_puts("RTC calib done\n");
//                    lcdStates = LCD_CAL_RTC_T;
                    systemStatus.systemAllStates = 0;
                    systemStatus.rtcTimeFlag = true;
                }
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

//        if(crc1 == integerData[3] && crc2 == integerData[4])  // disable for testing
        /*if(integerData[3] == 1 && integerData[4] == 1)
        {
            timeFlag = true;
            ds3232Timestamp();
        }*/
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
                // uart1_puts("token to big\n");
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
                    systemStatus.systemAllStates = 0;
                    systemStatus.rtcDateFlag = true;
                }
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
//        if(crc1 == integerData[4] && crc2 == integerData[5])  // disable for testing
        /*if(integerData[4] == 1 && integerData[5] == 1)
        {
            dateFlag = true;
            ds3232Timestamp();
        }*/
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
                // uart1_puts("token to big\n");
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
                for(i = 6; i < NUM_OF_INT_Data; i++)
                {
                    integerData[i] = 0;
                }
                crc = checksumCalculator(integerData, 4);
                crc1 = (crc >> 8) & 0xff;         // high byte
                crc2 = (crc >> 0) & 0xff;          // low byte
                dataCount = 0;

                
                // if(crc1 == integerData[3] && crc2 == integerData[4])  // below line will replaced by this line 
                if(integerData[4] == 1 && integerData[5] == 1) // for now, not checking crc
                {
                    unsigned long floatToLong;
                    float floatData;
//                    uint8_t eepromWriteBuff[4];
//                    floatData = EEPROM_byteToFloat(integerData); // coef byte to float conversion
                    EEPROM_byteToFloat(integerData, &floatData);
//                    EEPROM_floatToByte(floatData, eepromWriteBuff);
//                    EEPROM_floatToByte(floatData, integerData);
                    
                    // <editor-fold defaultstate="collapsed" desc="Pressure1 coef">
                    if(keyword == "C02A")
                    {
                        pres1.coefA = floatData;
//                        EEPROM_writeMultipleBytes(EEPROM_ADDR, 1, 0, eepromWriteBuff, 4);
                        EEPROM_writeMultipleBytes(EEPROM_ADDR, 1, 0, integerData, 4);
                        floatToLong = floatData * 10000;
                        ltoa(floatToLong, txBuffer, 10);
//                        uart1_puts("CoefA_P1 = ");
//                        uart1_puts(txBuffer);
//                        uart1_newline();
//                        pres1Flag = true;
                        sensorsCoefStates.sensorsCoef = 0;
                        sensorsCoefStates.pres1_a = 1;
                    }
                    else if(keyword == "C02B")        
                    {
                        pres1.coefB = floatData;
//                        EEPROM_writeMultipleBytes(EEPROM_ADDR, 1, 4, eepromWriteBuff, 4);
                        EEPROM_writeMultipleBytes(EEPROM_ADDR, 1, 4, integerData, 4);
                        floatToLong = floatData * 10000;
                        ltoa(floatToLong, txBuffer, 10);
//                        uart1_puts("CoefB_P1 = ");
//                        uart1_puts(txBuffer);
//                        uart1_newline();
//                        pres1Flag = true;
                        sensorsCoefStates.sensorsCoef = 0;
                        sensorsCoefStates.pres1_b = 1;
                    }

                    else if(keyword == "C02C")
                    {
                        pres1.coefC = floatData;
//                        EEPROM_writeMultipleBytes(EEPROM_ADDR, 1, 8, eepromWriteBuff, 4);
                        EEPROM_writeMultipleBytes(EEPROM_ADDR, 1, 8, integerData, 4);
                        floatToLong = floatData * 10000;
                        ltoa(floatToLong, txBuffer, 10);
//                        uart1_puts("CoefC_P1 = ");
//                        uart1_puts(txBuffer);
//                        uart1_newline();
//                            pres1Flag = true;
                        sensorsCoefStates.sensorsCoef = 0;
                        sensorsCoefStates.pres1_c = 1;
                    }

                    else if(keyword == "C02D")
                    {
                        pres1.coefD = floatData;
//                        EEPROM_writeMultipleBytes(EEPROM_ADDR, 1, 12, eepromWriteBuff, 4);
                        EEPROM_writeMultipleBytes(EEPROM_ADDR, 1, 12, integerData, 4);
                        floatToLong = floatData * 10000;
                        ltoa(floatToLong, txBuffer, 10);
//                        uart1_puts("CoefD_P1 = ");
//                        uart1_puts(txBuffer);
//                        uart1_newline();
//                            pres1Flag = true;
                        sensorsCoefStates.sensorsCoef = 0;
                        sensorsCoefStates.pres1_d = 1;
                    }
                    // </editor-fold>

                    // <editor-fold defaultstate="collapsed" desc="Temperature1 coef">
                    else if(keyword == "C03A")
                    {
                        temp1.coefA = floatData;
//                        EEPROM_writeMultipleBytes(EEPROM_ADDR, 2, 0, eepromWriteBuff, 4);
                        EEPROM_writeMultipleBytes(EEPROM_ADDR, 2, 0, integerData, 4);
                        floatToLong = floatData * 10000;
                        ltoa(floatToLong, txBuffer, 10);
//                        uart1_puts("CoefA_T1 = ");
//                        uart1_puts(txBuffer);
//                        uart1_newline();
//                            temp1Flag = true;
                        sensorsCoefStates.sensorsCoef = 0;
                        sensorsCoefStates.temp1_a = 1;
                    }

                    else if(keyword == "C03B")
                    {
                        temp1.coefB = floatData;
//                        EEPROM_writeMultipleBytes(EEPROM_ADDR, 2, 4, eepromWriteBuff, 4);
                        EEPROM_writeMultipleBytes(EEPROM_ADDR, 2, 4, integerData, 4);
                        floatToLong = floatData * 10000;
                        ltoa(floatToLong, txBuffer, 10);
//                        uart1_puts("CoefB_T1 = ");
//                        uart1_puts(txBuffer);
//                        uart1_newline();
//                            temp1Flag = true;
                        sensorsCoefStates.sensorsCoef = 0;
                        sensorsCoefStates.temp1_b = 1;
                    }

                    else if(keyword == "C03C")
                    {
                        temp1.coefC = floatData;
//                        EEPROM_writeMultipleBytes(EEPROM_ADDR, 2, 8, eepromWriteBuff, 4);
                        EEPROM_writeMultipleBytes(EEPROM_ADDR, 2, 8, integerData, 4);
                        floatToLong = floatData * 10000;
                        ltoa(floatToLong, txBuffer, 10);
//                        uart1_puts("CoefC_T1 = ");
//                        uart1_puts(txBuffer);
//                        uart1_newline();
//                            temp1Flag = true;
                        sensorsCoefStates.sensorsCoef = 0;
                        sensorsCoefStates.temp1_c = 1; 
                    }
                    
                    else if(keyword == "C03D")
                    {
                        temp1.coefD = floatData;
//                        EEPROM_writeMultipleBytes(EEPROM_ADDR, 2, 12, eepromWriteBuff, 4);
                        EEPROM_writeMultipleBytes(EEPROM_ADDR, 2, 12, integerData, 4);
                        floatToLong = floatData * 10000;
                        ltoa(floatToLong, txBuffer, 10);
//                        uart1_puts("CoefD_T1 = ");
//                        uart1_puts(txBuffer);
//                        uart1_newline();
//                            temp1Flag = true;
                        sensorsCoefStates.sensorsCoef = 0;
                        sensorsCoefStates.temp1_d = 1;
                    }
                    // </editor-fold>
                    
                    // <editor-fold defaultstate="collapsed" desc="Pressure2 coef">
                    else if(keyword == "C04A")
                    {
                        pres2.coefA = floatData;
//                        EEPROM_writeMultipleBytes(EEPROM_ADDR, 3, 0, eepromWriteBuff, 4);
                        EEPROM_writeMultipleBytes(EEPROM_ADDR, 3, 0, integerData, 4);
                        floatToLong = floatData * 10000;
                        ltoa(floatToLong, txBuffer, 10);
//                        uart1_puts("CoefA_P2 = ");
//                        uart1_puts(txBuffer);
//                        uart1_newline();
//                            pres2Flag = true;
                        sensorsCoefStates.sensorsCoef = 0;
                        sensorsCoefStates.pres2_a = 1;
                    }

                    else if(keyword == "C04B")
                    {
                        pres2.coefB = floatData;
//                        EEPROM_writeMultipleBytes(EEPROM_ADDR, 3, 4, eepromWriteBuff, 4);
                        EEPROM_writeMultipleBytes(EEPROM_ADDR, 3, 4, integerData, 4);
                        floatToLong = floatData * 10000;
                        ltoa(floatToLong, txBuffer, 10);
//                        uart1_puts("CoefB_P2 = ");
//                        uart1_puts(txBuffer);
//                        uart1_newline();
//                            pres2Flag = true;
                        sensorsCoefStates.sensorsCoef = 0;
                        sensorsCoefStates.pres2_b = 1;
                    }

                    else if(keyword == "C04C")
                    {
                        pres2.coefC = floatData;
//                        EEPROM_writeMultipleBytes(EEPROM_ADDR, 3, 8, eepromWriteBuff, 4);
                        EEPROM_writeMultipleBytes(EEPROM_ADDR, 3, 8, integerData, 4);
                        floatToLong = floatData * 10000;
                        ltoa(floatToLong, txBuffer, 10);
//                        uart1_puts("CoefC_P2 = ");
//                        uart1_puts(txBuffer);
//                        uart1_newline();
//                            pres2Flag = true;
                        sensorsCoefStates.sensorsCoef = 0;
                        sensorsCoefStates.pres2_c = 1;
                    }

                    else if(keyword == "C04D")
                    {
                        pres2.coefD = floatData;
//                        EEPROM_writeMultipleBytes(EEPROM_ADDR, 3, 12, eepromWriteBuff, 4);
                        EEPROM_writeMultipleBytes(EEPROM_ADDR, 3, 12, integerData, 4);
                        floatToLong = floatData * 10000;
                        ltoa(floatToLong, txBuffer, 10);
//                        uart1_puts("CoefD_P2 = ");
//                        uart1_puts(txBuffer);
//                        uart1_newline();
//                            pres2Flag = true;
                        sensorsCoefStates.sensorsCoef = 0;
                        sensorsCoefStates.pres2_d = 1;
                    }
                    // </editor-fold>

                    // <editor-fold defaultstate="collapsed" desc="Temperature2 coef">
                    else if(keyword == "C05A")
                    {
                        temp2.coefA = floatData;
//                        EEPROM_writeMultipleBytes(EEPROM_ADDR, 4, 0, eepromWriteBuff, 4);
                        EEPROM_writeMultipleBytes(EEPROM_ADDR, 4, 0, integerData, 4);
                        floatToLong = floatData * 10000;
                        ltoa(floatToLong, txBuffer, 10);
//                        uart1_puts("CoefA_T2 = ");
//                        uart1_puts(txBuffer);
//                        uart1_newline();
//                            temp2Flag = true;
                        sensorsCoefStates.sensorsCoef = 0;
                        sensorsCoefStates.temp2_a = 1;
                    }

                    else if(keyword == "C05B")
                    {
                        temp2.coefB = floatData;
//                        EEPROM_writeMultipleBytes(EEPROM_ADDR, 4, 4, eepromWriteBuff, 4);
                        EEPROM_writeMultipleBytes(EEPROM_ADDR, 4, 4, integerData, 4);
                        floatToLong = floatData * 10000;
                        ltoa(floatToLong, txBuffer, 10);
//                        uart1_puts("CoefB_T2 = ");
//                        uart1_puts(txBuffer);
//                        uart1_newline();
//                            temp2Flag = true;
                        sensorsCoefStates.sensorsCoef = 0;
                        sensorsCoefStates.temp2_b = 1;
                    }

                    else if(keyword == "C05C")
                    {
                        temp2.coefC = floatData;
//                        EEPROM_writeMultipleBytes(EEPROM_ADDR, 4, 8, eepromWriteBuff, 4);
                        EEPROM_writeMultipleBytes(EEPROM_ADDR, 4, 8, integerData, 4);
                        floatToLong = floatData * 10000;
                        ltoa(floatToLong, txBuffer, 10);
//                        uart1_puts("CoefC_T2 = ");
//                        uart1_puts(txBuffer);
//                        uart1_newline();
//                            temp2Flag = true;
                        sensorsCoefStates.sensorsCoef = 0;
                        sensorsCoefStates.temp2_c = 1;
                    }

                    else if(keyword == "C05D")
                    {
                        temp2.coefD = floatData;
//                        EEPROM_writeMultipleBytes(EEPROM_ADDR, 4, 12, eepromWriteBuff, 4);
                        EEPROM_writeMultipleBytes(EEPROM_ADDR, 4, 12, integerData, 4);
                        floatToLong = floatData * 10000;
                        ltoa(floatToLong, txBuffer, 10);
//                        uart1_puts("CoefD_T2 = ");
//                        uart1_puts(txBuffer);
//                        uart1_newline();
//                            temp2Flag = true;
                        sensorsCoefStates.sensorsCoef = 0;
                        sensorsCoefStates.temp2_d = 1;
                    }
                    // </editor-fold>
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
        //            pres1Factor = floatData;
        //        }        
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
                // uart_puts("token to big1\n");
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
                    //            eepromFlag = true;
                    //            eepromStateFlags = eepromWr1Byte;
//                    uart1_puts("EEPROM write done\n");
                    EEPROM_write1ByteToPage(EEPROM_ADDR, integerData[0], integerData[1], integerData[2]);
                    systemStatus.systemAllStates = 0;
                    systemStatus.eepromWrBytes = true;
                }
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
                // uart_puts("token to big1\n");
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
                    // uart_puts("token to big2\n");
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
//                itoa(readByte, txBuffer, 10);
//                uart1_puts(txBuffer);
//                uart1_puts("  EEPROM read done\n");
                systemStatus.systemAllStates = 0;
                systemStatus.eepromRdBytes = true;
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
                // uart_puts("token to big1\n");
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
                    // uart_puts("token to big2\n");
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
                    //            eepromFlag = true;
                    //            eepromStateFlags = eepromWr1Byte;
                    EEPROM_writeMultipleBytes(EEPROM_ADDR, integerData[0], integerData[1], &integerData[2], 4);
                    systemStatus.systemAllStates = 0;
                    systemStatus.eepromWrByte = true;
                }
            }
            else if(dataCount < eepromWrNPacketLen || dataCount > eepromWrNPacketLen)
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
                // uart_puts("token to big1\n");
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
                    // uart_puts("token to big2\n");
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

//                for(i = 0; i < 4; i++)
//                {
//                    itoa(readBytes, txBuffer, 10);
//                    uart1_puts(txBuffer);
////                    uart1_puts("  EEPROM readN done\n");
//                }
                systemStatus.systemAllStates = 0;
                systemStatus.eepromRdByte = true;
            }
            else if(dataCount < eepromRdNPacketLen || dataCount > eepromRdNPacketLen)
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
                // uart_puts("token to big1\n");
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
                    // uart_puts("token to big2\n");
                    strcpy(parsedStrings[i], NULL);
                }
            }

            // convert ascii data into integer data           
            if(dataCount == fwInfoWePacketLen)
            {
                integerData[0] = (uint8_t)(atoi(parsedStrings[0])); // FW info
                integerData[1] = (uint8_t)(atoi(parsedStrings[1])); // CRC1
                integerData[2] = (uint8_t)(atoi(parsedStrings[2])); // CRC2
                for(i = 3; i < NUM_OF_INT_Data; i++)
                {
                    integerData[i] = 0;
                }
                dataCount = 0;
                
                crc = checksumCalculator(integerData[0], 1);
                crc1 = (crc >> 8) & 0xff;
                crc2 = (crc >> 0) & 0xff;
                
//                if(crc1 == integerData[0] && crc2 == integerData[1])
                if(integerData[1] == 1 && integerData[2] == 2)
                {
//                    EEPROM_writeMultipleBytes(EEPROM_ADDR, 0, 0, integerData[0], 1);
                    EEPROM_write1ByteToPage(EEPROM_ADDR, 0, 0, integerData[0]);
//                    uart1_puts("FW stored\n");
                    systemStatus.systemAllStates = 0;
                    systemStatus.fwWr = true;
                }
                else
                {
                    serialMsgErrorFlag = true; // uart1_puts("data no good\n");
                    serialMsgFlags = 0;
                }
            }
            else if(dataCount < fwInfoWePacketLen || dataCount > fwInfoWePacketLen)
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

// <editor-fold defaultstate="collapsed" desc="Functions: CRC, itoa, ltoa">
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
// </editor-fold>