/* 
 * File:   I2C.h
 * Author: Shahzaib
 *
 * Created on September 5, 2022, 9:52 PM
 */

#ifndef I2C_H
#define	I2C_H

// --Includes-------------------------------------------------------------------
#include <xc.h>
#include "main.h"

/*// --Defines--------------------------------------------------------------------
#define I2C_Clock	400000

#define ACK   0
#define NACK  1

void I2C1_Initialize(void);

void I2C1_waitIdle(void);

// Send an I2C START
// Return 0 if all ok, 1 if bus collision
unsigned char I2C1_start(void);

// Send an I2C STOP
void I2C1_stop(void);

// Send an I2C REPEATED START
void I2C1_restart(void);

//Send one byte. Return 0 if ACK received, or 1 if NAK received
unsigned char I2C1_sendbyte(unsigned char dat);

//Receive one byte. ackflag=0 to send ACK, or 1 to send NAK in reply
unsigned char I2C1_recvbyte(unsigned char ackflag);

//Send an array of data to an I2C device.
//Return 0 if all OK, 1 if bus error, 2 if slave address NAK, 3 if slave register NAK, 4 if slave data NAK
unsigned char I2C1_writeblock(unsigned char slave_address, unsigned char start_reg, unsigned char buflen, const unsigned char * bufptr);

//If you use the read-block routine and pass repeatStart=true,
//it will do repeated-start condition intead of stop,start.
//Otherwise, it will do stop,start.
//Receive an array of data from an I2C device.
//Return 0 if all OK, 1 if bus error, 2 if slave address NAK, 3 if slave register NAK
unsigned char I2C1_readblock(unsigned char slave_address, unsigned char start_reg, unsigned char buflen, unsigned char * bufptr);
*/

/* Code from EDA Forum*/
// --Defines--------------------------------------------------------------------
#define I2C1_Clock   400000
#define I2C_WRITE   0b11111110
#define I2C_READ    0b00000001

// I2C Bus Control Definition
#define I2C_DATA_ACK 0
#define I2C_DATA_NOACK 1
#define I2C_WRITE_CMD 0
#define I2C_READ_CMD 1

#define I2C_START_CMD 0
#define I2C_REP_START_CMD 1
#define I2C_REQ_ACK 0
#define I2C_REQ_NOACK 0

void I2C1_initialize(void);

void I2C1_idle(void);

void I2C1_start(unsigned char stype);

void i2c_rep_start(void);

void I2C1_stop(void);

unsigned char I2C1_slave_ack(void);

void I2C1_master_ack(unsigned char ack_type);

void I2C1_write(unsigned char data);

unsigned char I2C1_read(void);

void I2C1_writeBlock(unsigned char slaveAddr, unsigned char startRegAddr, unsigned char buflen, const unsigned char * bufptr);

//Receive an array of data from an I2C device.
//Return 0 if all OK, 1 if bus error, 2 if slave address NAK, 3 if slave register NAK
//unsigned char I2C1_readblock(unsigned char slaveAddr, unsigned char startRegAddr, unsigned char buflen, unsigned char * bufptr);
void I2C1_readblock(unsigned char slaveAddr, unsigned char startRegAddr, unsigned char buflen, unsigned char * bufptr);

unsigned char I2C1_checkError(void);

void I2C1_clearBus(void);

#endif	/* I2C_H */

