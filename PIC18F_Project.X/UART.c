#include "UART.h"

#define charLen     10
#define numOfChar   8

#define timePacketLen   5
#define datePacketLen   6

char input_line[MAX_INPUT];
char txBuff[40];
uint8_t serialMsgFlags = 0;
char parsedStrings[charLen][numOfChar];
uint8_t dataPosition = 0;
uint8_t dataCount = 0;
const char delimiter[] = ",";
char *keywordPointer;
uint8_t integerData[15];

void uart_init(void)
{
    PIE1bits.TX1IE = 0; // disable transmitter interrupts
    PIE1bits.RC1IE = 0; // disable receiver interrupts
    BAUDCON1 = 0; // stop UART1
    RCSTA1 = 0;
    TXSTA1 = 0;

    //  _delay((FCY / BAUD_RATE)*20); /* wait 20 bit times with UART1 TX output at stop bit state */

    TXSTA1bits.SYNC = 0; // use asynchronous mode
    TXSTA1bits.BRGH = 1; // use high speed BRG mode, see errata
    BAUDCON1bits.BRG16 = 1; // use 16-bit baud rate divisor

    // Baudrate
    SPBRGH1 = (unsigned char) (((FCY / BAUD_RATE) - 1U) >> 8U);
    SPBRG1 = (unsigned char) ((FCY / BAUD_RATE) - 1U);

    TXSTA1bits.TXEN = 1; // enable transmitter
    RCSTA1bits.CREN = 1; // enable receiver
    RCSTA1bits.SPEN = 1; // enable UART1

    IPR1bits.RC1IP = 1; // set receiver interrupt for high priority
    IPR1bits.TX1IP = 0; // set transmitter interrupt for low priority
    PIE1bits.RC1IE = 1; // enable receiver interrupts
    PIE1bits.TX1IE = 0; // disable transmitter interrupts
}

//void uart_send(char *c)

void uart_send(uint8_t c)
{
    //    TXREG1 = *c;
    TXREG1 = c;
    while(TXSTA1bits.TRMT1 == 0);
    //  {
    //    Nop();
    //  }
}

void uart_puts(char *pStr)
{
    while(*pStr)
    {
        //        uart_send(pStr);
        //        pStr++;

        uart_send(*pStr++);
    }
}

void uart_receiver(void)
{
    if(RCSTA1bits.FERR1)
    {
        uint8_t er = RCREG1;
    } else if(RCSTA1bits.OERR1)
    {
        RCSTA1bits.CREN1 = 0;
        RCSTA1bits.CREN1 = 1;
    } else
    {
        processIncomingByte(RCREG1);
    }
}

void processIncomingByte(const char inByte)
{
    static uint8_t input_pos = 0;

    switch(inByte)
    {
        case '\n': // end of text
            input_line[input_pos] = 0; // terminating null byte

            // terminator reached! process input_line here ...
            process_data(input_line);

            // reset buffer for next time
            input_pos = 0;
            break;

        case '\r': // discard carriage return
            break;

        default:
            // keep adding if not full ... allow for terminating null byte
            if(input_pos < (MAX_INPUT - 1))
            {
                input_line[input_pos++] = inByte;
            }
            break;
    } // end of switch
}

void process_data(char *data)
{
    // --finding length of serial msg-------------------------------------------
    //    uart_puts(data);
    //    sprintf(txBuff, "len of arr %u\n", strlen(data));
    //    uart_puts(txBuff);

    // <editor-fold defaultstate="collapsed" desc="Send raw data">
    /*if(input_line[0] == 'S')
    {
        uart_puts("First char is S");
    }
    if(input_line[0] == 'K')
    {
        uart_puts("First char is K");
    }*/
    // </editor-fold>

    // --Preprocess serial msg--------------------------------------------------
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

    // <editor-fold defaultstate="collapsed" desc="Parsing data and make seperate variable value">
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
                integerData[0] = atoi(parsedStrings[0]);    // hour
                integerData[1] = atoi(parsedStrings[1]);    // minute
                integerData[2] = atoi(parsedStrings[2]);    // second
                integerData[3] = atoi(parsedStrings[3]);    // len
                integerData[4] = atoi(parsedStrings[4]);    // CRC
                for(uint8_t i = 5; i < 15; i++)
                {
                    integerData[i] = 0;
                }
                sprintf(txBuff, "%d:%d:%d  %d  %d\n", integerData[0], integerData[1], 
                        integerData[2], integerData[3], integerData[4]);
                uart_puts(txBuff);
                uart_puts("--Time calib-----------------------------------------\n");
            }
            else
            {
                uart_puts("data no good\n");
                dataCount = 0;
                serialMsgFlags = 0;
            }
        } 
//        else
//        {
//            uart_puts("sorry no keyword\n");
//            dataCount = 0;
//            serialMsgFlags = 0;
//        }
    }
    
    if(serialMsgFlags == 2)
    {
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
                integerData[0] = atoi(parsedStrings[0]);    // day
                integerData[1] = atoi(parsedStrings[1]);    // date
                integerData[2] = atoi(parsedStrings[2]);    // month
                integerData[3] = atoi(parsedStrings[3]);    // year
                integerData[4] = atoi(parsedStrings[4]);    // len
                integerData[5] = atoi(parsedStrings[5]);    // CRC
                for(uint8_t i = 6; i < 15; i++)
                {
                    integerData[i] = 0;
                }
                sprintf(txBuff, "%d, %d-%d-%d  %d %d\n", integerData[0], integerData[1], 
                        integerData[2], integerData[3], integerData[4], integerData[5]);
                uart_puts(txBuff);
                uart_puts("--Date calib-----------------------------------------\n");
            }
            else
            {
                uart_puts("data no good\n");
                dataCount = 0;
                serialMsgFlags = 0;
            }
        } 
//        else
//        {
//            uart_puts("sorry no keyword\n");
//            dataCount = 0;
//            serialMsgFlags = 0;
//        }
    }
    else
    {
        serialMsgFlags = 0;
        dataCount = 0;
    }
    // </editor-fold>
}
