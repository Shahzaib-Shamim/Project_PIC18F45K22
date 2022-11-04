#include "I2C.h"

void I2C1_initialize(void)
{
    SSP1STATbits.SMP = 0; // 1 = 100KHz, 0 = 400KHz
    SSP1STATbits.CKE = 0; // disable SMBus

    SSP1CON1bits.SSPM = 0x08;
    SSPCON1bits.SSPEN = 1;

    SSP1CON2 = 0x00; // Reset MSSP Control Register

    SSP1ADD = ((_XTAL_FREQ / I2C1_Clock) / 4) - 1;

    PIR1bits.SSP1IF = 0; // clear SSP1 Interrupt flag
    //    PIR2bits.BCL1IF = 0;  // clear bs collision flag
}

void I2C1_is_idle(void)
{
    // Check for ACKEN, RCEN, PEN, RSEN, SEN in SSP1CON2 and R_nW in SSP1STAT
    while((SSP1CON2 & 0x1F) || SSP1STATbits.R_nW);
}

void I2C1_start(unsigned char stype)
{
    I2C1_is_idle();
    if(stype == I2C_START_CMD)
    {
        SSP1CON2bits.SEN = 1; // Start I2C Transmission
        while(SSP1CON2bits.SEN);
    }
    else
    {
        SSP1CON2bits.RSEN = 1; // ReStart I2C Transmission
        while(SSP1CON2bits.RSEN);
    }
}

void I2C1_rep_start(void)
{
    SSP1CON2bits.RSEN = 1;
    while(SSP1CON2bits.RSEN);
}

unsigned char I2C1_slave_ack(void)
{
    // Return: 1 = Acknowledge was not received from slave
    //         0 = Acknowledge was received from slave
    return (SSP1CON2bits.ACKSTAT);
}

void I2C1_stop(void)
{
    // Stop I2C Transmission
    SSP1CON2bits.PEN = 1;
    while(SSP1CON2bits.PEN);
}

void I2C1_write(unsigned char data)
{
    SSP1BUF = data;
    if(SSP1CON1bits.WCOL) // Check for write collision
        return;
    while(SSP1STATbits.BF != 0);

    I2C1_is_idle();
}

void I2C1_master_ack(unsigned char ack_type)
{
    SSP1CON2bits.ACKDT = ack_type; // 1 = Not Acknowledge, 0 = Acknowledge
    SSP1CON2bits.ACKEN = 1; // Enable Acknowledge
    while(SSP1CON2bits.ACKEN == 1);
}

unsigned char I2C1_read(void)
{
    // Ensure the I2C module is idle
    I2C1_is_idle();

    // Enable Receive Mode
    SSP1CON2bits.RCEN = 1;
    while(SSP1STATbits.BF != 1);

    return (SSP1BUF);
}

void I2C1_writeBlock(unsigned char slaveAddr, unsigned char startRegAddr, unsigned char buflen, const unsigned char * bufptr)
{
    I2C1_start(I2C_START_CMD);
    I2C1_write((slaveAddr << 1) & 0xFE);
    I2C1_write(startRegAddr);
    //send the data. buflen might be zero!
    for(; buflen > 0; --buflen)
    {
        I2C1_write(*bufptr++);
    }
    I2C1_stop();
}

void I2C1_readblock(unsigned char slaveAddr, unsigned char startRegAddr, unsigned char buflen, unsigned char * bufptr)
{
    I2C1_start(I2C_START_CMD);
    I2C1_write((slaveAddr << 1) & 0xFE);
    I2C1_write(startRegAddr);
    I2C1_start(I2C_REP_START_CMD);
    I2C1_write((uint8_t) ((slaveAddr << 1) | 0x01));

    //send the data. buflen might be zero!
    for(; buflen > 0; --buflen)
    {
        unsigned char ackflag = (buflen == 1); // NACK
        *bufptr++ = I2C1_read();
        I2C1_master_ack(ackflag);
    }
    I2C1_stop();
}

uint8_t timeout_counter = 0;

unsigned char I2C1_checkError(void)
{
    if(SSP1CON2bits.ACKSTAT)
    {
        //        i2c_stop();
        return 1;
    }
    if(SSP1CON1bits.WCOL)
    {
        //        i2c_stop();
        return 1;
    }
    if(SSP1CON1bits.SSPOV)
    {
        //        i2c_stop();
        return 1;
    }
    if(PIR2bits.BCL1IF)
    {
        //        i2c_stop();
        return 1;
    }
    if(timeout_counter > 250)
    {
        //        i2c_stop();
        return 1;
    }
    timeout_counter++; //Approximately 117 cycles between counter increments = ~914uS timeout period at 32MHz clock
    return 0;
}

void I2C1_clearBus(void)
{
    uint8_t initialState[] = {TRIS_SDA, TRIS_SCL, ANS_SDA, ANS_SCL, SDA, SCL, SSP1CON1bits.SSPEN}; //Backup initial pin setup state
    SSP1CON1bits.SSPEN = 0; //Disable MSSP if enabled
    TRIS_SDA = 1; //SDA - Set Data as input
    TRIS_SCL = 1; //SCL - Set Clock as input = high-impedance output
    ANS_SDA = 0; //Set both pins as digital
    ANS_SCL = 0;
    SCL = 0; //Set SCL PORT to be low, then we'll toggle TRIS to switch between low and high-impedance

    uint8_t validOnes = 0;

    //Clock the SCL pin until we get 10 valid ones in a row like we'd expect from an idle bus
    while(validOnes < 10)
    {
        TRIS_SCL = 0; //Clock low
        __delay_us(6); //Wait out clock low period
        TRIS_SCL = 1; //Clock high
        __delay_us(3); //Wait until we are in the mid point of clock high period

        if(SDA == 1)
        {
            //Read data and check if SDA is high (idle)
            validOnes++;
        }
        else
        {
            validOnes = 0; //if the data isn't a one, reset the counter so we get 10 in a row
        }
        __delay_us(3); //Wait remainder of clock high period
    }

    //Restore initial pin I/O state
    TRIS_SDA = initialState[0];
    TRIS_SCL = initialState[1];
    ANS_SDA = initialState[2];
    ANS_SCL = initialState[3];
    SDA = initialState[4];
    SCL = initialState[5];
    SSP1CON1bits.SSPEN = initialState[6];
}