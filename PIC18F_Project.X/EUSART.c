#include "EUSART.h"
#include "GPIO.h"
#include "EEPROM.h"

// --Global variables-----------------------------------------------------------
uint8_t input_line[MAX_INPUT]; // serial msg buffer
//uint8_t dataBytes[DATA_PARSED_COUNT]; // number of parsed out bytes
//bool dataProcessed = false; // flag when parsing is done
char strBuff[30];
//uint8_t dataCount = 0;

// --Serial msg variables-------------------------------------------------------
uint8_t dataMode = 0;
uint8_t hour = 0;
uint8_t minute = 0;
uint8_t second = 0;
uint8_t day = 0;
uint8_t date = 0;
uint8_t month = 0;
uint8_t year = 0;
float serialTemp1Rx = 0.0f;
float serialTemp2Rx = 0.0f;
float serialPres1Rx = 0.0f;
float serialPres2Rx = 0.0f;
uint8_t CRC = 0;
bool calibrationRxFlag = false;
bool setDateFlag = false;
bool setTimeFlag = false;
bool setTemp1Flag = false;
bool setTemp2Flag = false;
bool setPres1Flag = false;
bool setPres2Flag = false;

// --Macro Declarations---------------------------------------------------------
#define EUSART1_TX_BUFFER_SIZE 50
#define EUSART1_RX_BUFFER_SIZE 8

// --Global Variables-----------------------------------------------------------
volatile uint8_t eusart1TxHead = 0;
volatile uint8_t eusart1TxTail = 0;
volatile uint8_t eusart1TxBuffer[EUSART1_TX_BUFFER_SIZE];
volatile uint8_t eusart1TxBufferRemaining;

volatile uint8_t eusart1RxHead = 0;
volatile uint8_t eusart1RxTail = 0;
volatile uint8_t eusart1RxBuffer[EUSART1_RX_BUFFER_SIZE];
volatile eusart1_status_t eusart1RxStatusBuffer[EUSART1_RX_BUFFER_SIZE];
volatile uint8_t eusart1RxCount;
volatile eusart1_status_t eusart1RxLastError;

// --EUSART1 API---------------------------------------------------------------
void (*EUSART1_TxDefaultInterruptHandler)(void);
void (*EUSART1_RxDefaultInterruptHandler)(void);

void (*EUSART1_FramingErrorHandler)(void);
void (*EUSART1_OverrunErrorHandler)(void);
void (*EUSART1_ErrorHandler)(void);

void EUSART1_DefaultFramingErrorHandler(void);
void EUSART1_DefaultOverrunErrorHandler(void);
void EUSART1_DefaultErrorHandler(void);

void EUSART1_Initialize(void)
{
    // disable interrupts before changing states
    PIE1bits.RC1IE = 0;
    EUSART1_SetRxInterruptHandler(EUSART1_Receive_ISR);
    PIE1bits.TX1IE = 0;
    EUSART1_SetTxInterruptHandler(EUSART1_Transmit_ISR);

    // --Configure EUSART1 module-----------------------------------------------
    //    BAUDCON1 = 0x08;
    BAUDCON1bits.ABDEN1 = 0; // disable auto-baud mode
    BAUDCON1bits.WUE1 = 0; // normal operation
    BAUDCON1bits.BRG161 = 1; // 16-bit baud rate generator
    BAUDCON1bits.CKTXP = 0; // idle state is low
    BAUDCON1bits.DTRXP1 = 0; // receive data not inverted
    BAUDCON1bits.RCIDL1 = 0; // detect start bit    
    BAUDCON1bits.ABDOVF1 = 0; // auto-baud no overflow 

    //    RCSTA1 = 0x90;
    RCSTA1bits.RX9D1 = 0; // 9th bit is parity
    RCSTA1bits.OERR1 = 0; // no overrun error
    RCSTA1bits.FERR1 = 0; // no framing error
    RCSTA1bits.ADDEN1 = 0; // disable address detection (9th bit is parity)
    RCSTA1bits.CREN1 = 1; // enable continous receiver
    //    RCSTA1bits.SREN1 = 0;   // dont care
    RCSTA1bits.RX91 = 0; // 8-bit operation
    RCSTA1bits.SPEN1 = 1; // enable serial port

    //    TXSTA1 = 0x24;
    TXSTA1bits.TX9D1 = 0; // 9th bit is parity bit
    TXSTA1bits.TRMT1 = 0; // TSR full
    TXSTA1bits.BRGH1 = 1; // highspeed
    TXSTA1bits.SENDB1 = 0; // sync break tx complete
    TXSTA1bits.SYNC1 = 0; // asynchronous mode
    TXSTA1bits.TXEN1 = 1; // transmit enable
    TXSTA1bits.TX91 = 0; // 8-bit transmission
    //    TXSTA1bits.CSRC1 = 0;   // dont care (slave mode when in sync mode)

    // Baudrate generator
    //    SPBRG1 = 0x8A;
    SPBRG1 = (uint8_t) ((FCY / BAUD_RATE) - 1U); // low-byte
    //    SPBRGH1 = 0x00;
    SPBRGH1 = (uint8_t) (((FCY / BAUD_RATE) - 1U) >> 8U); // high-byte

    EUSART1_SetFramingErrorHandler(EUSART1_DefaultFramingErrorHandler);
    EUSART1_SetOverrunErrorHandler(EUSART1_DefaultOverrunErrorHandler);
    EUSART1_SetErrorHandler(EUSART1_DefaultErrorHandler);

    eusart1RxLastError.status = 0;

    // initializing the driver state
    eusart1TxHead = 0;
    eusart1TxTail = 0;
    eusart1TxBufferRemaining = sizeof (eusart1TxBuffer);

    eusart1RxHead = 0;
    eusart1RxTail = 0;
    eusart1RxCount = 0;

    // enable receive interrupt
    PIE1bits.RC1IE = 1;
}

bool EUSART1_is_tx_ready(void)
{
    return (eusart1TxBufferRemaining ? true : false);
}

bool EUSART1_is_rx_ready(void)
{
    return (eusart1RxCount ? true : false);
}

bool EUSART1_is_tx_done(void)
{
    return TXSTA1bits.TRMT;
}

eusart1_status_t EUSART1_get_last_status(void)
{
    return eusart1RxLastError;
}

uint8_t EUSART1_Read(void)
{
    uint8_t readValue = 0;

    while (0 == eusart1RxCount)
    {
    }

    eusart1RxLastError = eusart1RxStatusBuffer[eusart1RxTail];

    readValue = eusart1RxBuffer[eusart1RxTail++];
    if (sizeof (eusart1RxBuffer) <= eusart1RxTail)
    {
        eusart1RxTail = 0;
    }
    PIE1bits.RC1IE = 0;
    eusart1RxCount--;
    PIE1bits.RC1IE = 1;

    return readValue;
}

void EUSART1_Write(uint8_t txData)
{
    while (0 == eusart1TxBufferRemaining)
    {
    }

    if (0 == PIE1bits.TX1IE)
    {
        TXREG1 = txData;
    } else
    {
        PIE1bits.TX1IE = 0;
        eusart1TxBuffer[eusart1TxHead++] = txData;
        if (sizeof (eusart1TxBuffer) <= eusart1TxHead)
        {
            eusart1TxHead = 0;
        }
        eusart1TxBufferRemaining--;
    }
    PIE1bits.TX1IE = 1;
}

void EUSART1_Transmit_ISR(void)
{

    // add your EUSART1 interrupt custom code
    if (sizeof (eusart1TxBuffer) > eusart1TxBufferRemaining)
    {
        TXREG1 = eusart1TxBuffer[eusart1TxTail++];
        if (sizeof (eusart1TxBuffer) <= eusart1TxTail)
        {
            eusart1TxTail = 0;
        }
        eusart1TxBufferRemaining++;
    } else
    {
        PIE1bits.TX1IE = 0;
        //        PIR1bits.TX1IF = 0; // added
    }
}

void EUSART1_Receive_ISR(void)
{

    eusart1RxStatusBuffer[eusart1RxHead].status = 0;

    if (RCSTA1bits.FERR)
    {
        eusart1RxStatusBuffer[eusart1RxHead].ferr = 1;
        EUSART1_FramingErrorHandler();
    }

    if (RCSTA1bits.OERR)
    {
        eusart1RxStatusBuffer[eusart1RxHead].oerr = 1;
        EUSART1_OverrunErrorHandler();
    }

    if (eusart1RxStatusBuffer[eusart1RxHead].status)
    {
        EUSART1_ErrorHandler();
    } else
    {
        EUSART1_RxDataHandler();
    }

    // or set custom function using EUSART1_SetRxInterruptHandler()
}

void EUSART1_RxDataHandler(void)
{
    // use this default receive interrupt handler code
    eusart1RxBuffer[eusart1RxHead++] = RCREG1;
    if (sizeof (eusart1RxBuffer) <= eusart1RxHead)
    {
        eusart1RxHead = 0;
    }
    eusart1RxCount++;

    // my custom code added here
    // TXREG1 = RCREG;   // comment for readline function

    processIncomingByte(RCREG1);

    //    EUSART1_Write(RCREG);
    PIR1bits.RC1IF = 0;
}

// <editor-fold defaultstate="collapsed" desc="EUSART1 handler functions">

void EUSART1_DefaultFramingErrorHandler(void)
{
    // added from example code
    //    RCREG1;
}

void EUSART1_DefaultOverrunErrorHandler(void)
{
    // EUSART1 error - restart
    RCSTA1bits.CREN = 0;
    RCSTA1bits.CREN = 1;
}

void EUSART1_DefaultErrorHandler(void)
{
    EUSART1_RxDataHandler();
}

void EUSART1_SetFramingErrorHandler(void (* interruptHandler)(void))
{
    EUSART1_FramingErrorHandler = interruptHandler;
}

void EUSART1_SetOverrunErrorHandler(void (* interruptHandler)(void))
{
    EUSART1_OverrunErrorHandler = interruptHandler;
}

void EUSART1_SetErrorHandler(void (* interruptHandler)(void))
{
    EUSART1_ErrorHandler = interruptHandler;
}

void EUSART1_SetTxInterruptHandler(void (* interruptHandler)(void))
{
    EUSART1_TxDefaultInterruptHandler = interruptHandler;
}

void EUSART1_SetRxInterruptHandler(void (* interruptHandler)(void))
{
    EUSART1_RxDefaultInterruptHandler = interruptHandler;
}
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="EUSART1 user functions">

void EUSART1_send(uint8_t c) // char old
{
    EUSART1_Write(c);
}

void EUSART1_send_array(uint8_t *c, uint8_t len) // char old
{
    for (uint8_t i = 0; i < len; i++)
    {
        EUSART1_send(c[i]);
    }
}

void EUSART1_send_string(uint8_t *c) // char old 
{
    uint8_t i = 0;
    while (c[i] != '\0')
    {
        EUSART1_send(c[i]);
        i++;
    }
    //    EUSART1_send('\0');
}

//void EUSART1_Read_Text(uint8_t *Output, uint8_t length)
//{
//    for(int i = 0; i < length; i++)
//        Output[i] = EUSART1_Read();
//}
//
//void EUSART1_Read_Line(uint8_t *Output)
//{
//    char temp = 0;
//    char i;
//    for(i = 0; temp != '\n' && temp != '\r'; i++)
//    {
//        while(!PIR1bits.RC1IF);
//        temp = EUSART1_Read();
//        Output[i] = temp;
//    }
//    Output[i - 1] = '\0';
//}
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="Parsing functions">

void parseData(uint8_t *data)
{
    static uint8_t dataCount = 0;

    EUSART1_send_string("Raw data ");
    EUSART1_send_string((char *) data);
    dataCount = strlen(input_line);
    sprintf(strBuff, "  %d\n", dataCount);
    EUSART1_send_string(strBuff);
    
    // Hex input, normal output
    // *#$hour,minute,second,day,date,month,year,\n\
    // example: 11:23:49  tuesday, 27 sep, 2022 
    // 2A,23,24,hour,minute,second,day,date,month,year,\n
    // 2A,23,24,0B,17,31,03,1B,09,16,0A         in hex
    // OR OR OR OR OR OR OR OR OR OR OR
    // 42,35,36,23,39,00,03,27,9,22,10          in decimal
    /*if(input_line[0] == 42 && input_line[1] == 35 && input_line[2] == 36)  //42 = 2Ah, 35 = 23h, 36 = 24h
    {
        hour = input_line[3];       // enter hex in serial
        minute = input_line[4];     // hex in serial
        second = input_line[5];     // hex in serial
        day = input_line[6];
        date = input_line[7];
        month = input_line[8];
        year = input_line[9];
        
        EUSART1_send_string("RTC calibration\n");
        calibrationRxFlag = true;
    }*/
    
//    hour = (uint8_t)atoi(input_line[0]);
//    minute = (uint8_t)atoi(input_line[1]);
//    second = (uint8_t)atoi(input_line[2]);
    
//    EUSART1_send_array(num, 4);
    
    
    
    /*if(dataCount == CALIB_MSG_DATE_LEN) //7
    {
        // check start character
        if(input_line[0] == '<' || input_line[0] == 60)
        {
            if(input_line[1] == 'D' || input_line[1] == 68)
            {
                day = input_line[2];
                date = input_line[3];
                month = input_line[4];
                year = input_line[5];
                CRC = input_line[6];
                setDateFlag = true;
                calibrationRxFlag = true;
            }
        }
        dataCount = 0;
    }
    if(dataCount == CALIB_MSG_TIME_LEN)     //6
    {
        if(input_line[0] == '<' || input_line[0] == 60)
        {
            if(input_line[1] == 'T' || input_line[1] == 84)
            {
                hour = input_line[2];
                minute = input_line[3];
                second = input_line[4];
                year = input_line[5];
                CRC = input_line[6];
                setDateFlag = true;
                calibrationRxFlag = true;
            }
        }
        dataCount = 0;
    }
    else
    {
        calibrationRxFlag = false;
        dataCount = 0;
    }*/



    /*if (input_line[0] == '<' || input_line[0] == 60)    // in ascii or decimal (3C = hex)
    {
        if (input_line[1] == 'C' || input_line[1] == 67)    // in ascii or decimal (43 = hex)
        {
            hour = input_line[2];
            minute = input_line[3];
            second = input_line[4];
            day = input_line[5];
            date = input_line[6];
            month = input_line[7];
            year = input_line[8];
//            CRC = input_line[6];
            
//            setDateFlag = true;
            calibrationRxFlag = true;
        }
    }
    dataCount = 0;*/
    
    // sahi code for extracting keyword
/*uint8_t keyword1[] = "C001#";
uint8_t *pointerToFoundData = strstr(input_line, keyword1);    //go find keyword

if (pointerToFoundData != NULL)
{ 
	//found it
	uint8_t positionInString = pointerToFoundData - input_line;
//        sprintf(strBuff, "Keyword Starts at %d\n", positionInString);
//        EUSART1_send_string(strBuff);

	//now strip out keyword and junk
	uint8_t goodData[30];
	strncpy(goodData, &input_line[positionInString + strlen(keyword1)], sizeof (goodData));
	EUSART1_send_string(goodData);
} 
else
{
	EUSART1_send_string("NO KEYWORD");
}*/



    // sahi code improved atoi
    /*
    uint8_t numbers[5];
    uint8_t keyword1[] = "C001#";
    uint8_t *pointerToFoundData = strstr(input_line, keyword1); //go find keyword

    if (pointerToFoundData != NULL)
    {
        //found it
        uint8_t positionInString = pointerToFoundData - input_line;
        //        sprintf(strBuff, "Keyword Starts at %d\n", positionInString);
        //        EUSART1_send_string(strBuff);

        //now strip out keyword and junk
        uint8_t goodData[30];
        strncpy(goodData, &input_line[positionInString + strlen(keyword1)], sizeof (goodData));
        EUSART1_send_string("\n\0");
        EUSART1_send_string(goodData);

        // PARSE **  PARSE **  PARSE **  PARSE **  PARSE **  PARSE **  PARSE **
        const char delimiter[] = ",";
        char parsedStrings[5][5];
        char *token = strtok(goodData, delimiter);
        strncpy(parsedStrings[0], token, sizeof (parsedStrings[0])); //first one
        for (uint8_t i = 1; i < 5; i++)
        {
            token = strtok(NULL, delimiter);
            strncpy(parsedStrings[i], token, sizeof (parsedStrings[i]));
        }
        EUSART1_send_string("\n");

        for (uint8_t i = 0; i < 5; i++)
        {
            numbers[i] = (uint8_t)(atoi(parsedStrings[i]));
//            EUSART1_send_string(parsedStrings[i]); //  should have the 5 data strings parsed out
//            EUSART1_send_string("\n\0");
            
            sprintf(strBuff, "%d\n", numbers[i]);
            EUSART1_send_string(strBuff);
        }    

    } 
    else
    {
        EUSART1_send_string("\nNO KEYWORD\n");
    }*/



    // not working
    /*uint8_t numbers[5];
    uint8_t keyword1[] = "C00$#";
    uint8_t *pointerToFoundData = strstr(input_line, keyword1); //go find keyword

    if (pointerToFoundData != NULL)
    {
        //found it
        uint8_t positionInString = pointerToFoundData - input_line;
        //        sprintf(strBuff, "Keyword Starts at %d\n", positionInString);
        //        EUSART1_send_string(strBuff);

        //now strip out keyword and junk
        uint8_t goodData[30];
        strncpy(goodData, &input_line[positionInString + strlen(keyword1)], sizeof (goodData));
        EUSART1_send_string("\n\0");
        EUSART1_send_string(goodData);

        // PARSE **  PARSE **  PARSE **  PARSE **  PARSE **  PARSE **  PARSE **
        const char delimiter[] = ",";
        char parsedStrings[5][5];
        uint8_t dataCount = 0;
        char *token = strtok(goodData, delimiter);
        strncpy(parsedStrings[0], token, sizeof (parsedStrings[0])); //first one
        dataCount++;
        for (uint8_t i = 1; i < 5; i++)
        {
            token = strtok(NULL, delimiter);
            strncpy(parsedStrings[i], token, sizeof (parsedStrings[i]));
            dataCount++;
        }
        EUSART1_send_string("\n\0");

        
        if(dataCount == 5)
        {
            EUSART1_send_string("\n5 data\n\0");
            for (uint8_t i = 0; i < 5; i++)
            {
                numbers[i] = (uint8_t)(atoi(parsedStrings[i]));
    //            EUSART1_send_string(parsedStrings[i]); //  should have the 5 data strings parsed out
    //            EUSART1_send_string("\n\0");

                sprintf(strBuff, "%d\n", numbers[i]);
                EUSART1_send_string(strBuff);
            }    
        }
        else
        {
            EUSART1_send_string("\nNO 5 data\n\0");
        }

    } 
    else
    {
        EUSART1_send_string("\nNO KEYWORD\n\0");
    }*/
    
    
    
    
    if(input_line[0] == 42 && input_line[1] == 35 && input_line[2] == 36)  //42 = 2Ah, 35 = 23h, 36 = 24h
    {
        if(dataCount == 6)
        {
            hour = input_line[3];       // enter hex in serial
            minute = input_line[4];     // hex in serial
            second = input_line[5];     // hex in serial
            EUSART1_send_string("Time calib\n");
            setTimeFlag = true;
        }
    }
    if(input_line[0] == 42 && input_line[1] == 35 && input_line[2] == 37)  //42 = 2Ah, 35 = 23h, 36 = 24h
    {
        if(dataCount == 7)
        {
            day = input_line[3];       // enter hex in serial
            date = input_line[4];     // hex in serial
            month = input_line[5];     // hex in serial
            year = input_line[6];     // hex in serial
            EUSART1_send_string("Date calib\n");
            setDateFlag = true;
        }
    }
    if(input_line[0] == 42 && input_line[1] == 35 && input_line[2] == 38)
    {
        if(dataCount == 3)
        {
            EUSART1_send_string("Temp1 calib\n");
            setTemp1Flag = true;
        }
    }
    if(input_line[0] == 42 && input_line[1] == 35 && input_line[2] == 39)
    {
        if(dataCount == 3)
        {
            EUSART1_send_string("Pres1 calib\n");
            setPres1Flag = true;
        }
    }
}

void processIncomingByte(const uint8_t inByte)
{
    static uint8_t input_pos = 0;

    switch (inByte)
    {
        case '\n': // end of text
            //        case '0x0A':
            input_line[input_pos] = 0; // terminating null byte  

            // terminator reached! process input_line here ...=
            parseData(input_line);

            // reset buffer for next time
            input_pos = 0;
            break;

        case '\r': // discard carriage return
            break;

        default:
            // keep adding if not full ... allow for terminating null byte
            if (input_pos < (MAX_INPUT - 1))
            {
                input_line[input_pos++] = inByte;
            }
            break;
    }
}

//void float2Bytes(uint8_t bytes_temp[4], float float_variable)
//{
//    memcpy(bytes_temp, (uint8_t*) (&float_variable), 4);
//}
// </editor-fold>



