/* 
 * File:   UART.h
 * Author: Shahzaib Shamim
 *
 * Created on October 11, 2022, 10:59 PM
 */

#ifndef UART_H
#define	UART_H

// --Includes-------------------------------------------------------------------
#include <xc.h>
#include "main.h"

// --Defines--------------------------------------------------------------------
#define BAUD_RATE 115200

#define MAX_INPUT 50

// --Global variables-----------------------------------------------------------
extern char input_line[MAX_INPUT];
extern uint8_t uartRxFlag;

// --UART functions---------------------------------------------------
/**
 * Initializes UART1 module
 */
void uart_init(void);

/**
 * Send one character
 * @param char
 */
//void uart_send(char *);
void uart_send(uint8_t);

/**
 * Send string of characters
 * @param char array
 */
void uart_puts(char *);

/**
 * Receive one character
 */
void uart_receiver(void);

// --Global functions for serial process----------------------------------------
/**
 * Do parsing on the reeive msg
 * @param char array
 */
void process_data(char *);

/**
 * Make string of characters until \n
 * @param char
 */
void processIncomingByte(const char);


#endif	/* UART_H */

