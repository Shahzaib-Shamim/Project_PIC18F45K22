#include "ADS1115.h"
#include "I2C.h"

ADS1115_Object_t myADC;

// --Global Functions Section---------------------------------------------------

void ADS1115_init(ADS1115_Object_t *instance)
{
    ADS1115_ConfigBits_t config;
    config.value = 0;
    
    uint8_t buff[2];

    I2C1_start(I2C_START_CMD);
    I2C1_write((instance->address << 1) & 0xFE);
    I2C1_write(REGISTER_CONFIG);
    I2C1_write(HIGH_BYTE(instance->config.value));
    I2C1_write(LOW_BYTE(instance->config.value));
    I2C1_stop();
    
    __delay_us(10000);

    I2C1_start(I2C_START_CMD);
    I2C1_write((uint8_t)( (instance->address << 1) & 0xFE) );
    I2C1_write(REGISTER_CONFIG);
    I2C1_start(I2C_REP_START_CMD);
    I2C1_write((uint8_t)( (instance->address << 1) | 0x01) );
    buff[0] = I2C1_read();
    I2C1_master_ack(I2C_DATA_ACK);
    buff[1] = I2C1_read();
    I2C1_master_ack(I2C_DATA_NOACK);
    I2C1_stop();
    
    config.value = (uint16_t)( (buff[0] << 8) | buff[1] );
    config.os = 0;

    if(config.value == instance->config.value)
    {
        instance->isPresent = 1;
    }
    else
    {
        instance->isPresent = 0;
    }
}

//float ADS1115_readSingleEnded(ADS1115_Object_t *instance, const uint8_t channel)
void ADS1115_readSingleEnded(ADS1115_Object_t *instance, const uint8_t channel, float* floatVar)
{
    instance->config.os = OS_START_SINGLE_CONVERSION;
    switch(channel)
    {
        case CHANNEL_0:
        {
            instance->config.mux = MUX_SINGLE_0;
            break;
        }

        case CHANNEL_1:
        {
            instance->config.mux = MUX_SINGLE_1;
            break;
        }

        case CHANNEL_2:
        {
            instance->config.mux = MUX_SINGLE_2;
            break;
        }

        case CHANNEL_3:
        {
            instance->config.mux = MUX_SINGLE_3;
            break;
        }
    }
    
    I2C1_start(I2C_START_CMD);
    I2C1_write((uint8_t)( (instance->address << 1) & 0xFE) );
    I2C1_write(REGISTER_CONFIG);
    I2C1_write(HIGH_BYTE(instance->config.value));
    I2C1_write(LOW_BYTE(instance->config.value));
    I2C1_stop();
    
    while(!conversionComplete(instance));
//    return computeVoltage(instance, getLastConversion(instance));
    *floatVar = computeVoltage(instance, getLastConversion(instance));
}

static uint16_t getLastConversion(ADS1115_Object_t *instance)
{
    uint16_t value;
    uint8_t buff[2];
    
    I2C1_start(I2C_START_CMD);
    I2C1_write((uint8_t)( (instance->address << 1) & 0xFE) );
    I2C1_write(REGISTER_CONVERSION);
    I2C1_start(I2C_REP_START_CMD);
    I2C1_write((uint8_t)( (instance->address << 1) | 0x01) );
    buff[0] = I2C1_read();
    I2C1_master_ack(I2C_DATA_ACK);
    buff[1] = I2C1_read();
    I2C1_master_ack(I2C_DATA_NOACK);
    I2C1_stop();
    
    value = (uint16_t)( (buff[0] << 8) | buff[1] );
    if(value > 32768)
    {
        value = 0;
    }
    return value;
}

static uint16_t conversionComplete(ADS1115_Object_t *instance)
{ 
    ADS1115_ConfigBits_t config;
    uint8_t buff[2];
    config.value = 0;
    config.os = 1;
    
    I2C1_start(I2C_START_CMD);
    I2C1_write((uint8_t)( (instance->address << 1) & 0xFE) );
    I2C1_write(REGISTER_CONFIG);
    I2C1_start(I2C_REP_START_CMD);
    I2C1_write( (uint8_t)( (instance->address << 1) | 0x01) );
    buff[0] = I2C1_read();
    I2C1_master_ack(I2C_DATA_ACK);
    buff[1] = I2C1_read();
    I2C1_master_ack(I2C_DATA_NOACK);
    I2C1_stop();
    
    config.value = (uint16_t)( (buff[0] << 8) | buff[1] );
    return config.os;
}

static float computeVoltage(ADS1115_Object_t *instance, const uint16_t value)
{
    float range;
    switch(instance->config.pga)
    {
        case PGA_FSR_6_144:
        {
            range = 6.144f;
            break;
        }
        case PGA_FSR_4_096:
        {
            range = 4.096f;
            break;
        }
        case PGA_FSR_2_048:
        {
            range = 2.048f;
            break;
        }
        case PGA_FSR_1_024:
        {
            range = 1.024f;
            break;
        }
        case PGA_FSR_0_512:
        {
            range = 0.512f;
            break;
        }
        case PGA_FSR_0_256:
        {
            range = 0.0256f;
            break;
        }
    }

    return((float) value * (range / 32768.0f));
}

