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

#define MAX_INPUT 30

// --Global variables-----------------------------------------------------------
extern char input_line[MAX_INPUT];
extern bool uartProcessFlag;

// --UART functions-------------------------------------------------------------
/**
 * Initializes UART1 module
 */
void uart1_init(void);

/**
 * Send one character
 * @param char
 */
void uart1_putc(uint8_t);

/**
 * Send string of characters
 * @param char array
 */
void uart1_puts(char *);

/**
 * Receive one character
 */
void uart1_getc(void);

/**
 * Print newline and carriage return
 */
void uart1_newline(void);


// --Global functions for serial process----------------------------------------
/**
 * Make string of characters until \n
 * @param char
 */
void processIncomingByte(const char);


#endif	/* UART_H */

