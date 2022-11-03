/* 
 * File:   EEPROM.h
 * Author: Shahzaib
 *
 * Created on September 10, 2022, 9:30 PM
 */

#ifndef EEPROM_H
#define	EEPROM_H

// --Includes-------------------------------------------------------------------
#include <xc.h>
#include "main.h"

// --Macro Declarations---------------------------------------------------------
// AT24C32 EEPROM Chip (DS3231 module onboard EEPROM)
#define EEPROM_ADDR    0x57
#define EEPROM_ADDR_WR (EEPROM_ADDR << 1) & 0xFE
#define EEPROM_ADDR_RD (EEPROM_ADDR << 1) | 0x01

/**
 * Write byte to EEPROM to a given address.
 * @param address = 0 to 4095 (0-127 pages, 0-31 bytes per page)
 * @param pData = 0 to 0xFFh
 */
static void EEPROM_write1Byte(uint8_t slaveAddr, uint16_t address, uint8_t pData);

/**
 * Read byte from from a given address.
 * @param address = 0 to 4095 (0-127 pages, 0-31 bytes per page)
 * @return = 1 byte data
 */
static uint8_t EEPROM_read1Byte(uint8_t slaveAddr, uint16_t address);

//void EEPROM_writeBlock(uint8_t slaveAddr, uint16_t address, uint8_t pData, uint8_t noOfBytes);
/**
 * Write no of bytes to a given address.
 * @param address = 0 to 4095 (0-127 pages, 0-31 bytes per page)
 * @param pData = Read data from EEPROM into pData array
 * @param noOfBytes = 0 to 31
 */
void EEPROM_writeMultipleBytes(uint8_t slaveAddr, uint8_t page, uint8_t byteOffset, uint8_t *pData, uint8_t noOfBytes);

//uint8_t EEPROM_readBlock(uint8_t slaveAddr, uint16_t address, uint8_t pData, uint8_t noOfBytes);
/**
 * Reads no of bytes from a given starting address.
 * @param address = 0 to 0 to 4095 (0-127 pages, 0-31 bytes per page)
 * @param pData = Read data from EEPROM into pData array
 * @param noOfBytes = 0 to 31
 */
void EEPROM_readMultipleBytes(uint8_t slaveAddr, uint8_t page, uint8_t byteOffset, uint8_t *pData, uint8_t noOfBytes);

// TO DO: eepromWriteBytes, eepromReadBytes

/**
 * Erase EEPROM page by writing 0xFF data for given times to a given page address
 * Example:
 * for(uint8_t x = 0; x < 128; x++)        // x = page
 *     EEPROM_erase(EEPROM_ADDR, x, 0, 32);
 */
void EEPROM_erase(uint8_t slaveAddr, uint8_t page, uint8_t byteOffset, uint8_t noOfBytes);

/**
 * Example to read and write from/to EEPROM:
 * page: 0 to 127
 * offset will be 0 to 31
 * total: 32 * 128 page = 4096 bytes
 * so page will be 0 to 127
 * Note: 1 page can store 32 bytes only!
 * 
 * Example: write 0x01 at page 0 offset 3
 * so 12-bit address will be (127*32)+3 = 0x003h
 */ 
void EEPROM_write1ByteToPage(uint8_t slaveAddr, uint8_t page, uint8_t byteOffset, uint8_t pData);
uint8_t EEPROM_read1ByteFromPage(uint8_t slaveAddr, uint8_t page, uint8_t byteOffset);

/**
 * Write 16bit data into EEPROM
 * @param page = 0 to 127
 * @param byteOffset = 0 to 31
 * @param pData = data to write/read
 */
void EEPROM_write_16bitData(uint8_t slaveAddr, uint8_t page, uint8_t byteOffset, uint16_t pData);

/**
 * Read 16bit data from EEPROM
 * @param page = 0 to 127
 * @param byteOffset = 0 to 31
 * @return 16bit data
 */
uint16_t EEPROM_read_16bitData(uint8_t slaveAddr, uint8_t page, uint8_t byteOffset);

/**
 * Converts 4byte float data into 4x1byte data
 * @param floatData = float data variable
 * @param pByteData = pointer to byte array
 */
//void EEPROM_floatToByte(float floatData, uint8_t *pByteData);
void EEPROM_floatToByte(float *pfloatData, uint8_t *pByteData);

/**
 * Converts 1byte data into float data
 * @param pByteData = pointer to byte array
 * @return float variable
 */
//float EEPROM_byteToFloat(uint8_t *pByteData);
void EEPROM_byteToFloat(uint8_t *pByteData, float *pFloatData);




#endif	/* EEPROM_H */

