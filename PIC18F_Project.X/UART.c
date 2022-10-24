#include "UART.h"

bool uartProcessFlag = false;
char input_line[MAX_INPUT];

void uart1_init(void)
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

    IPR1bits.RC1IP = 0; // set receiver interrupt for high priority
    IPR1bits.TX1IP = 0; // set transmitter interrupt for low priority
    PIE1bits.RC1IE = 1; // enable receiver interrupts
    PIE1bits.TX1IE = 0; // disable transmitter interrupts
}

//void uart1_putc(char *c)

void uart1_putc(uint8_t c)
{
    //    TXREG1 = *c;
    TXREG1 = c;
    while(TXSTA1bits.TRMT1 == 0);
    //  {
    //    Nop();
    //  }
}

void uart1_puts(char *pStr)
{
    while(*pStr)
    {
        //        uart1_putc(pStr);
        //        pStr++;

        uart1_putc(*pStr++);
    }
}

void uart1_getc(void)
{
    if(RCSTA1bits.FERR1)
    {
        uint8_t er = RCREG1;
    } 
    else if(RCSTA1bits.OERR1)
    {
        RCSTA1bits.CREN1 = 0;
        RCSTA1bits.CREN1 = 1;
    } 
    else
    {
        processIncomingByte(RCREG1);
    }
}

void uart1_newline(void)
{
    uart1_puts("\n");
}

void processIncomingByte(const char inByte)
{
    static uint8_t input_pos = 0;

    switch(inByte)
    {
        case '\n': // end of text
            input_line[input_pos] = 0; // terminating null byte

            // terminator reached! process input_line here ...
            uartProcessFlag = true;

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
