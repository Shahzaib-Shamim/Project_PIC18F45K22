/* 
 * File:   EUSART.h
 * Author: Shahzaib
 *
 * Created on August 17, 2022, 10:23 PM
 */

#ifndef EUSART_H
#define	EUSART_H

// --Includes-------------------------------------------------------------------
#include <xc.h>
#include "main.h"

#define MAX_INPUT         50  // how much serial data we expect before a newline
#define DATA_PARSED_COUNT 27   // Number of bytes to parsed from a msg
#define MAX_CHAR_IN_BYTE  5

#define CALIB_MSG_LEN     27
//#define NORMAL_MSG_LEN    12

#define CALIB_MSG_DATE_LEN 7    // 60,68,day,date,month,year,CRC (60 = <, 68 = D)
#define CALIB_MSG_TIME_LEN 6    // 60,84,hour,minute,second,CRC (60 = <, 84 = T)

extern uint8_t dataMode;
extern uint8_t hour;
extern uint8_t minute;
extern uint8_t second;
extern uint8_t day;
extern uint8_t date;
extern uint8_t month;
extern uint8_t year;
extern float serialTemp1Rx;
extern float serialTemp2Rx;
extern float serialPres1Rx;
extern float serialPres2Rx;
extern uint8_t CRC;

extern uint8_t input_line[MAX_INPUT];   // serial msg buffer
//extern uint8_t dataBytes[DATA_PARSED_COUNT];         // number of parsed out bytes
//extern bool dataProcessed;   // flag when parsing is done
extern bool calibrationRxFlag;
extern bool setDateFlag;
extern bool setTimeFlag;
extern bool setTemp1Flag;
extern bool setTemp2Flag;
extern bool setPres1Flag;
extern bool setPres2Flag;

extern volatile uint8_t gRX_data;
extern volatile uint8_t gTX_data;

//volatile struct
//{
//    unsigned RX_ready : 1;
//    unsigned TX_done : 1;
//    unsigned RX_overrun : 1;
//} gU1_flags;
typedef struct
{
    unsigned RX_ready : 1;
    unsigned TX_done : 1;
    unsigned RX_overrun : 1;
} my_gU1_flags;
extern volatile my_gU1_flags gU1_flags;


// --Macro Declarations--------------------------------------------------------------------
#define BAUD_RATE 115200UL

#define EUSART1_DataReady  (EUSART1_is_rx_ready())

// --Data Type Definitions------------------------------------------------------
typedef union
{

  struct
  {
    unsigned perr : 1;
    unsigned ferr : 1;
    unsigned oerr : 1;
    unsigned reserved : 5;
  };
  uint8_t status;
} eusart1_status_t;

// --Global variables-----------------------------------------------------------
extern volatile uint8_t eusart1TxBufferRemaining;
extern volatile uint8_t eusart1RxCount;

/**
  Section: EUSART1 APIs
 */
extern void (*EUSART1_TxDefaultInterruptHandler)(void);
extern void (*EUSART1_RxDefaultInterruptHandler)(void);

/**
  @Summary
    Initialization routine that takes inputs from the EUSART1 GUI.

  @Description
    This routine initializes the EUSART1 driver.
    This routine must be called before any other EUSART1 routine is called.

  @Preconditions
    None

  @Param
    None

  @Returns
    None

  @Comment
    
 */
void EUSART1_Initialize(void);

/**
  @Summary
    Checks if the EUSART1 transmitter is ready to transmit data

  @Description
    This routine checks if EUSART1 transmitter is ready 
    to accept and transmit data byte

  @Preconditions
    EUSART1_Initialize() function should have been called
    before calling this function.
    EUSART1 transmitter should be enabled before calling 
    this function

  @Param
    None

  @Returns
    Status of EUSART1 transmitter
    TRUE: EUSART1 transmitter is ready
    FALSE: EUSART1 transmitter is not ready
    
  @Example
    <code>
    void main(void)
    {
        volatile uint8_t rxData;
        
        // Initialize the device
        SYSTEM_Initialize();
        
        while(1)
        {
            // Logic to echo received data
            if(EUSART1_is_rx_ready())
            {
                rxData = UART1_Read();
                if(EUSART1_is_tx_ready())
                {
                    EUSART1Write(rxData);
                }
            }
        }
    }
    </code>
 */
bool EUSART1_is_tx_ready(void);

/**
  @Summary
    Checks if the EUSART1 receiver ready for reading

  @Description
    This routine checks if EUSART1 receiver has received data 
    and ready to be read

  @Preconditions
    EUSART1_Initialize() function should be called
    before calling this function
    EUSART1 receiver should be enabled before calling this 
    function

  @Param
    None

  @Returns
    Status of EUSART1 receiver
    TRUE: EUSART1 receiver is ready for reading
    FALSE: EUSART1 receiver is not ready for reading
    
  @Example
    <code>
    void main(void)
    {
        volatile uint8_t rxData;
        
        // Initialize the device
        SYSTEM_Initialize();
        
        while(1)
        {
            // Logic to echo received data
            if(EUSART1_is_rx_ready())
            {
                rxData = UART1_Read();
                if(EUSART1_is_tx_ready())
                {
                    EUSART1_Write(rxData);
                }
            }
        }
    }
    </code>
 */
bool EUSART1_is_rx_ready(void);

/**
  @Summary
    Checks if EUSART1 data is transmitted

  @Description
    This function return the status of transmit shift register

  @Preconditions
    EUSART1_Initialize() function should be called
    before calling this function
    EUSART1 transmitter should be enabled and EUSART1_Write
    should be called before calling this function

  @Param
    None

  @Returns
    Status of EUSART1 receiver
    TRUE: Data completely shifted out if the USART shift register
    FALSE: Data is not completely shifted out of the shift register
    
  @Example
    <code>
    void main(void)
    {
        volatile uint8_t rxData;
        
        // Initialize the device
        SYSTEM_Initialize();
        
        while(1)
        {
            if(EUSART1_is_tx_ready())
            {
        LED_0_SetHigh();
                EUSART1Write(rxData);
            }
      if(EUSART1_is_tx_done()
            {
                LED_0_SetLow();
            }
        }
    }
    </code>
 */
bool EUSART1_is_tx_done(void);

/**
  @Summary
    Gets the error status of the last read byte.

  @Description
    This routine gets the error status of the last read byte.

  @Preconditions
    EUSART1_Initialize() function should have been called
    before calling this function. The returned value is only
    updated after a read is called.

  @Param
    None

  @Returns
    the status of the last read byte

  @Example
  <code>
    void main(void)
    {
        volatile uint8_t rxData;
        volatile eusart1_status_t rxStatus;
        
        // Initialize the device
        SYSTEM_Initialize();
        
        // Enable the Global Interrupts
        INTERRUPT_GlobalInterruptEnable();
        
        while(1)
        {
            // Logic to echo received data
            if(EUSART1_is_rx_ready())
            {
                rxData = EUSART1_Read();
                rxStatus = EUSART1_get_last_status();
                if(rxStatus.ferr){
                    LED_0_SetHigh();
                }
            }
        }
    }
    </code>
 */
eusart1_status_t EUSART1_get_last_status(void);

/**
  @Summary
    Read a byte of data from the EUSART1.

  @Description
    This routine reads a byte of data from the EUSART1.

  @Preconditions
    EUSART1_Initialize() function should have been called
    before calling this function. The transfer status should be checked to see
    if the receiver is not empty before calling this function.

  @Param
    None

  @Returns
    A data byte received by the driver.
 */
uint8_t EUSART1_Read(void);

/**
 @Summary
   Writes a byte of data to the EUSART1.

 @Description
   This routine writes a byte of data to the EUSART1.

 @Preconditions
   EUSART1_Initialize() function should have been called
   before calling this function. The transfer status should be checked to see
   if transmitter is not busy before calling this function.

 @Param
   txData  - Data byte to write to the EUSART1

 @Returns
   None
 */
void EUSART1_Write(uint8_t txData);

/**
  @Summary
    Maintains the driver's transmitter state machine and implements its ISR.

  @Description
    This routine is used to maintain the driver's internal transmitter state
    machine.This interrupt service routine is called when the state of the
    transmitter needs to be maintained in a non polled manner.

  @Preconditions
    EUSART1_Initialize() function should have been called
    for the ISR to execute correctly.

  @Param
    None

  @Returns
    None
 */
void EUSART1_Transmit_ISR(void);

/**
  @Summary
    Maintains the driver's receiver state machine and implements its ISR

  @Description
    This routine is used to maintain the driver's internal receiver state
    machine.This interrupt service routine is called when the state of the
    receiver needs to be maintained in a non polled manner.

  @Preconditions
    EUSART1_Initialize() function should have been called
    for the ISR to execute correctly.

  @Param
    None

  @Returns
    None
 */
void EUSART1_Receive_ISR(void);

/**
  @Summary
    Maintains the driver's receiver state machine

  @Description
    This routine is called by the receive state routine and is used to maintain 
    the driver's internal receiver state machine. It should be called by a custom
    ISR to maintain normal behavior

  @Preconditions
    EUSART1_Initialize() function should have been called
    for the ISR to execute correctly.

  @Param
    None

  @Returns
    None
 */
void EUSART1_RxDataHandler(void);

/**
  @Summary
    Set EUSART1 Framing Error Handler

  @Description
    This API sets the function to be called upon EUSART1 framing error

  @Preconditions
    Initialize  the EUSART1 before calling this API

  @Param
    Address of function to be set as framing error handler

  @Returns
    None
 */
void EUSART1_SetFramingErrorHandler(void (* interruptHandler)(void));

/**
  @Summary
    Set EUSART1 Overrun Error Handler

  @Description
    This API sets the function to be called upon EUSART1 overrun error

  @Preconditions
    Initialize  the EUSART1 module before calling this API

  @Param
    Address of function to be set as overrun error handler

  @Returns
    None
 */
void EUSART1_SetOverrunErrorHandler(void (* interruptHandler)(void));

/**
  @Summary
    Set EUSART1 Error Handler

  @Description
    This API sets the function to be called upon EUSART1 error

  @Preconditions
    Initialize  the EUSART1 module before calling this API

  @Param
    Address of function to be set as error handler

  @Returns
    None
 */
void EUSART1_SetErrorHandler(void (* interruptHandler)(void));

/**
  @Summary
    Sets the transmit handler function to be called by the interrupt service

  @Description
    Calling this function will set a new custom function that will be 
    called when the transmit interrupt needs servicing.

  @Preconditions
    EUSART1_Initialize() function should have been called
    for the ISR to execute correctly.

  @Param
    A pointer to the new function

  @Returns
    None
 */
void EUSART1_SetTxInterruptHandler(void (* interruptHandler)(void));

/**
  @Summary
    Sets the receive handler function to be called by the interrupt service

  @Description
    Calling this function will set a new custom function that will be 
    called when the receive interrupt needs servicing.

  @Preconditions
    EUSART1_Initialize() function should have been called
    for the ISR to execute correctly.

  @Param
    A pointer to the new function

  @Returns
    None
 */
void EUSART1_SetRxInterruptHandler(void (* interruptHandler)(void));

// --Global additional functions for UART
/**
 * Send one character to UART
 */
void EUSART1_send(uint8_t c); //uint8_t

/**
 * Send characters of given length to UART
 */
void EUSART1_send_array(uint8_t *c, uint8_t len);    //uint8_t

/**
 * Send string of characters to UART
 */
void EUSART1_send_string(uint8_t *c); //uint8_t

//wait until N caracter are received and write the value in the passed array
void EUSART1_Read_Text(uint8_t *Output, uint8_t length);

//wait until N caracter are received and write the value in the passed array
void EUSART1_Read_Line(uint8_t *Output);

//write a string
//void EUSART1_Write_String(uint8_t *text);

// added from example code
/*
 * Get one character
 */
uint8_t UART1_getc(void);

/*
 * Send one character
 */
void UART1_putc(uint8_t Data);

/*
 * Send a zero terminated string
 */
void UART1_puts(char * pStr);

// --Serial input parsing and process functions---------------------------------
void parseData(uint8_t *data);   // char

/**
 * Receive each character from serial and make string of character
 * @param inByte
 */
void processIncomingByte(const uint8_t inByte);   // char

//void float2Bytes(uint8_t bytes_temp[4], float float_variable);

#endif	/* EUSART_H */

