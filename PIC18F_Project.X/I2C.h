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

#define SDA         LATCbits.LATC4
#define SCL         LATCbits.LATC3
#define TRIS_SDA    TRISCbits.TRISC4
#define TRIS_SCL    TRISCbits.TRISC3
#define ANS_SDA     ANSELCbits.ANSC4
#define ANS_SCL     ANSELCbits.ANSC3

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

void I2C1_readblock(unsigned char slaveAddr, unsigned char startRegAddr, unsigned char buflen, unsigned char * bufptr);

unsigned char I2C1_checkError(void);

void I2C1_clearBus(void);

#endif	/* I2C_H */

