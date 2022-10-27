#include "EEPROM.h"
#include "I2C.h"

static void EEPROM_write1Byte(uint8_t slaveAddr, uint16_t address, uint8_t pData)
{
    I2C1_start(I2C_START_CMD);
    I2C1_write((uint8_t)( (slaveAddr << 1) & 0xFE) );
    I2C1_write(HIGH_BYTE(address));
    I2C1_write(LOW_BYTE(address));
    I2C1_write(pData);
    I2C1_stop();
}

static uint8_t EEPROM_read1Byte(uint8_t slaveAddr, uint16_t address)
{
    uint8_t pData;
    
    I2C1_start(I2C_START_CMD);
    I2C1_write((uint8_t)( (slaveAddr << 1) & 0xFE) );
    I2C1_write(HIGH_BYTE(address));
    I2C1_write(LOW_BYTE(address));
    I2C1_start(I2C_REP_START_CMD);
    I2C1_write((uint8_t)( (slaveAddr << 1) | 0x01) );
    pData = I2C1_read();
    I2C1_master_ack(I2C_DATA_NOACK);
    I2C1_stop();
    
    return pData;
}

void EEPROM_writeMultipleBytes(uint8_t slaveAddr, uint8_t page, uint8_t byteOffset, uint8_t *pData, uint8_t noOfBytes)
{
    uint16_t eepromAddress = (page * 32) + byteOffset;
     
    I2C1_start(I2C_START_CMD);
    I2C1_write((uint8_t)( (slaveAddr << 1) & 0xFE) );
    I2C1_write(HIGH_BYTE(eepromAddress));
    I2C1_write(LOW_BYTE(eepromAddress));
    for (; noOfBytes > 0; --noOfBytes)
    {
        I2C1_write(*pData++);
    }
    I2C1_stop();
}

void EEPROM_readMultipleBytes(uint8_t slaveAddr, uint8_t page, uint8_t byteOffset, uint8_t *pData, uint8_t noOfBytes)
{
    uint8_t ackflag;
    uint16_t eepromAddress = (page * 32) + byteOffset;
    
    I2C1_start(I2C_START_CMD);
    I2C1_write((uint8_t)( (slaveAddr << 1) & 0xFE) );
    I2C1_write(HIGH_BYTE(eepromAddress));
    I2C1_write(LOW_BYTE(eepromAddress));
    I2C1_start(I2C_REP_START_CMD);
    I2C1_write((uint8_t)( (slaveAddr << 1) | 0x01) );
    
    for (; noOfBytes > 0; --noOfBytes)
    {
//        if(noOfBytes == 1) // if this is last byte
//        {
//            ackflag = 1;
//        }
//        else
//        {
//            ackflag = 0;
//        }
        ackflag = (noOfBytes == 1);  // NACK
        *pData++ = I2C1_read();
        I2C1_master_ack(ackflag);
    }
    I2C1_stop();
}

void EEPROM_erase(uint8_t slaveAddr, uint8_t page, uint8_t byteOffset, uint8_t noOfBytes)
{
    uint16_t eepromAddress = (page * 32) + byteOffset;
     
    I2C1_start(I2C_START_CMD);
    I2C1_write((uint8_t)( (slaveAddr << 1) & 0xFE) );
    I2C1_write(HIGH_BYTE(eepromAddress));
    I2C1_write(LOW_BYTE(eepromAddress));
    for (; noOfBytes > 0; --noOfBytes)
    {
        I2C1_write(0xFF);
    }
    I2C1_stop();
}

void EEPROM_write1ByteToPage(uint8_t slaveAddr, uint8_t page, uint8_t byteOffset, uint8_t pData)
{
    uint16_t eepromAddress = (page * 32) + byteOffset;
    EEPROM_write1Byte(slaveAddr, eepromAddress, pData);
}

uint8_t EEPROM_read1ByteFromPage(uint8_t slaveAddr, uint8_t page, uint8_t byteOffset)
{
    uint16_t eepromAddress = (page * 32) + byteOffset;
    return EEPROM_read1Byte(slaveAddr, eepromAddress);
}

void EEPROM_write_16bitData(uint8_t slaveAddr, uint8_t page, uint8_t byteOffset, uint16_t pData)
{
    uint16_t eepromAddress = (page * 32) + byteOffset;    
    
    I2C1_start(I2C_START_CMD);
    I2C1_write((uint8_t)( (slaveAddr << 1) & 0xFE) );
    I2C1_write(HIGH_BYTE(eepromAddress));
    I2C1_write(LOW_BYTE(eepromAddress));
    I2C1_write(HIGH_BYTE(pData));
    I2C1_write(LOW_BYTE(pData));
    I2C1_stop();
}

uint16_t EEPROM_read_16bitData(uint8_t slaveAddr, uint8_t page, uint8_t byteOffset)
{
    uint16_t eepromAddress = (page * 32) + byteOffset;
    uint8_t readBuff[2];
    
    I2C1_start(I2C_START_CMD);
    I2C1_write((slaveAddr << 1) & 0xFE);
    I2C1_write(HIGH_BYTE(eepromAddress));
    I2C1_write(LOW_BYTE(eepromAddress));
    I2C1_start(I2C_REP_START_CMD);
    I2C1_write((uint8_t)( (slaveAddr << 1) | 0x01) );
    readBuff[0] = I2C1_read();
    I2C1_master_ack(I2C_DATA_ACK);
    readBuff[1] = I2C1_read();
    I2C1_master_ack(I2C_DATA_NOACK);
    I2C1_stop();
    
    return (uint16_t) ((readBuff[0] << 8) | readBuff[1]);
}

//void EEPROM_floatToByte(float floatData, uint8_t *pByteData)
//{
//    uint8_t *pointer_to_float;
//    pointer_to_float = (uint8_t *)&floatData;
//    pByteData[0] = *pointer_to_float++;
//    pByteData[1] = *pointer_to_float++;
//    pByteData[2] = *pointer_to_float++;
//    pByteData[3] = *pointer_to_float;
//}
void EEPROM_floatToByte(float *pfloatData, uint8_t *pByteData)
{
    uint8_t *pointer_to_float;
    pointer_to_float = (uint8_t *)pfloatData;
    pByteData[0] = *pointer_to_float++;
    pByteData[1] = *pointer_to_float++;
    pByteData[2] = *pointer_to_float++;
    pByteData[3] = *pointer_to_float;
}

//float EEPROM_byteToFloat(uint8_t *pByteData)
//{
//    float floatData;
//    uint8_t *pointer_to_float;
//    pointer_to_float = (uint8_t *)&floatData;
//    *pointer_to_float++ = pByteData[0];
//    *pointer_to_float++ = pByteData[1];
//    *pointer_to_float++ = pByteData[2];
//    *pointer_to_float = pByteData[3];
//    return floatData;
//}
void EEPROM_byteToFloat(uint8_t *pByteData, float *pFloatData)
{
    uint8_t *pointer_to_float;
    pointer_to_float = (uint8_t *)pFloatData;
    *pointer_to_float++ = pByteData[0];
    *pointer_to_float++ = pByteData[1];
    *pointer_to_float++ = pByteData[2];
    *pointer_to_float = pByteData[3];
}

