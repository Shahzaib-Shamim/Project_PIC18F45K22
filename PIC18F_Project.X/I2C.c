#include "I2C.h"

//void I2C1_Initialize(void)
//{
//    SSP1CON1bits.SSPM = 0b1000; // Master mode using SSPADD as baud control
//    SSP1ADD = ((_XTAL_FREQ / I2C_Clock) / 4) - 1;
//    SSP1CON1bits.SSPEN = 1; //enable I2C
//    // CKP = 1 (when used as a slave)
//
//    SSP1CON2 = 0x00;
//    SSP1STATbits.CKE = 0; // disable SMBus
//    SSP1STATbits.SMP = 1; // disable slew rate for 100KHz, (0 when 400KHz)
//
//    PIR1bits.SSP1IF = 0; // clear SSP1 flag
//    PIR2bits.BCL1IF = 0; // clear bs collision flag
//}
//
//void I2C1_waitIdle(void)
//{
//    // Check for ACKEN, RCEN, PEN, RSEN, SEN in SSP1CON2 and R_nW in SSP1STAT
//    while((SSP1CON2 & 0x1F) || SSP1STATbits.R_nW); // added
//}
//
//// Send an I2C START
//// Return 0 if all ok, 1 if bus collision
//unsigned char I2C1_start(void)
//{
//    PIR2bits.BCL1IF = 0; // clear 'Bus collision" flag
//    SSP1CON2bits.SEN = 1; // initiate a START cycle
//    while(SSP1CON2bits.SEN || SSP1STATbits.R_nW); //wait until it has been sent  
//    //    i2c_waitIdle(); // added
//    return PIR2bits.BCL1IF; //return value of BCLIF flag
//}
//
//// Send an I2C STOP
//void I2C1_stop(void)
//{
//    SSP1CON2bits.PEN = 1; // initiate a STOP cycle
//    while(SSP1CON2bits.PEN || SSP1STATbits.R_nW); //wait until it has been sent
//    //   i2c_waitIdle(); // added
//}
//
//// Send an I2C REPEATED START
//void I2C1_restart(void)
//{
//    SSP1CON2bits.RSEN = 1; // initiate a REPEATED START cycle
//    while(SSP1CON2bits.RSEN || SSP1STATbits.R_nW); //wait until it has been sent
//    //    i2c_waitIdle(); // added
//}
//
////Send one byte. Return 0 if ACK received, or 1 if NAK received
//unsigned char I2C1_sendbyte(unsigned char dat)
//{
//    SSP1BUF = dat;
////    while(!SSP1STATbits.BF != 0); // added
//    while(SSP1STATbits.R_nW); //wait until byte sent and ACK/NAK received
//    //    i2c_waitIdle(); // added
//
//    /* added start */
////    while(SSP1STATbits.BF != 0);        // added, not using here
//    while(SSP1CON2bits.ACKSTAT != 0);
//    
//    while(!PIR1bits.SSP1IF);
//    PIR1bits.SSP1IF = 0; 
//    /* added end */
//
//    return SSP1CON2bits.ACKSTAT;
//}
//
////Receive one byte. ackflag=0 to send ACK, or 1 to send NAK in reply
//unsigned char I2C1_recvbyte(unsigned char ackflag)
//{
//    SSP1CON2bits.RCEN = 1; // initiate a RECEIVE cycle    
//    
////    while(!SSP1STATbits.BF != 1);   // added, not working here also
//    SSP1CON2bits.ACKDT = ackflag; //specify if we should send ACK or NAK after receiving
//    while(SSP1CON2bits.RCEN || SSP1STATbits.R_nW); //wait until RECEIVE has completed
//    //    i2c_waitIdle(); // added
//
//    SSP1CON2bits.ACKEN = 1; //initiate an ACK cycle
//    while(SSP1CON2bits.ACKEN || SSP1STATbits.R_nW); //wait until it has completed
//    //    i2c_waitIdle(); // added
//
//    /* added start */
//    // not working here
//    while(!PIR1bits.SSP1IF);
//    PIR1bits.SSP1IF = 0;
//    /* added end */
//
//    return SSPBUF;
//}
//
////Send an array of data to an I2C device.
////Return 0 if all OK, 1 if bus error, 2 if slave address NAK, 3 if slave register NAK, 4 if slave data NAK
//unsigned char I2C1_writeblock(unsigned char slave_address, unsigned char start_reg, unsigned char buflen, const unsigned char * bufptr)
//{
//    if(I2C1_start()) //send a start, and check if it succeeded
//    {
//        return 1; //abort if bus collision
//    }
//
//    //send the I2C slave address (force R/W bit low)
////    if(I2C1_sendbyte(slave_address & 0xFE))
//    if(I2C1_sendbyte((slave_address << 1) | 0))
//    {
//        I2C1_stop(); //if address was NAKed, terminate the cycle
//        return 2; //and return error code
//    }
//    //send the device register index
//    if(I2C1_sendbyte(start_reg))
//    {
//        I2C1_stop(); //if register was NAKed, terminate the cycle
//        return 3; //and return error code
//    }
//    //send the data. buflen might be zero!
//    for(; buflen > 0; --buflen)
//    {
//        if(I2C1_sendbyte(*bufptr++))
//        {
//            I2C1_stop(); //if register was NAKed, terminate the cycle
//            return 4; //and return error code
//        }
//
//    }
//    I2C1_stop();
//    return 0; //no error
//}
//
////If you use the read-block routine and pass repeatStart=true,
////it will do repeated-start condition intead of stop,start.
////Otherwise, it will do stop,start.
////Receive an array of data from an I2C device.
////Return 0 if all OK, 1 if bus error, 2 if slave address NAK, 3 if slave register NAK
//unsigned char I2C1_readblock(unsigned char slave_address, unsigned char start_reg, unsigned char buflen, unsigned char * bufptr)
//{
//    //do a dummy zero length write cycle to set the register address
//    unsigned char retval = I2C1_writeblock(slave_address, start_reg, 0, 0);
//    if(retval)
//    {
//        return retval; //abort if there was an error
//    }
//    //now start the READ cycle
//    if(I2C1_start()) //send a start, and check if it succeeded
//        return 1; //abort if bus collision
//    //send the I2C slave address (force the R/W bit high)
////    if(I2C1_sendbyte(slave_address | 0x01))
//    if(I2C1_sendbyte((slave_address << 1) | 1))
//    {
//        I2C1_stop(); //if address was NAKed, terminate the cycle
//        return 2; //and return error code
//    }
//    //receive the data.
//    for(; buflen > 0; --buflen)
//    {
//        unsigned char ackflag = (buflen == 1); //1 if this is the last byte to receive => send NAK
//        *bufptr++ = I2C1_recvbyte(ackflag);
//    }
//    I2C1_stop();
//    return 0; //no error
//}


void I2C1_initialize(void)
{
//    TRISCbits.RC3 = 1;          // SCL
//    TRISCbits.RC4 = 1;          // SDA

    SSP1STATbits.SMP = 0; // 1 = 100KHz, 0 = 400KHz
    SSP1STATbits.CKE = 0; // disable SMBus
    //    SSP1STAT = 0x80;      // Slew Rate is disable for 100 kHz mode
    // CKP = 1 (when used as a slave)

    SSP1CON1bits.SSPM = 0x08;
    SSPCON1bits.SSPEN = 1;
    //    SSP1CON1 = 0x28;      // Enable SDA and SCL, I2C Master mode, clock = FOSC/(4 * (SSPADD + 1))
    
    SSP1CON2 = 0x00;    // Reset MSSP Control Register
    
    SSP1ADD = ((_XTAL_FREQ / I2C1_Clock) / 4) - 1;
//    SSP1ADD = 19;

    PIR1bits.SSP1IF = 0; // clear SSP1 Interrupt flag
//    PIR2bits.BCL1IF = 0; // clear bs collision flag
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
//    I2C1_is_idle();
    SSP1CON2bits.RSEN = 1;
    while(SSP1CON2bits.RSEN); // added
}

unsigned char I2C1_slave_ack(void)
{
    // Return: 1 = Acknowledge was not received from slave
    //         0 = Acknowledge was received from slave
    return(SSP1CON2bits.ACKSTAT);
}

void I2C1_stop(void)
{
//    I2C1_is_idle();
    // Stop I2C Transmission
    SSP1CON2bits.PEN = 1;
    while(SSP1CON2bits.PEN);
}

void I2C1_write(unsigned char data)
{
//    I2C1_is_idle();
    SSP1BUF = data;
    if(SSP1CON1bits.WCOL) // Check for write collision
        return;
    while(SSP1STATbits.BF != 0);

    // added
//    while(!PIR1bits.SSP1IF && !i2c_checkError());
//    while(SSP1CON2bits.ACKSTAT != 0);
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
//    uint8_t recieve = 0;
    // Ensure the I2C module is idle
    I2C1_is_idle();
    
    // Enable Receive Mode
    SSP1CON2bits.RCEN = 1;
    while(SSP1STATbits.BF != 1);
    // added
//    while(!PIR1bits.SSP1IF && !i2c_checkError());
//    recieve = SSP1BUF;
//    SSP1CON2bits.ACKEN = ack;
//    return recieve;
    return(SSP1BUF);
}

void I2C1_writeBlock(unsigned char slaveAddr, unsigned char startRegAddr, unsigned char buflen, const unsigned char * bufptr)
{
    I2C1_start(I2C_START_CMD);
    I2C1_write((slaveAddr << 1) & 0xFE);
    I2C1_write(startRegAddr);
    //send the data. buflen might be zero!
    for (; buflen>0; --buflen)
    {
        I2C1_write(*bufptr++);
    }
    I2C1_stop();
}

//unsigned char I2C1_readblock(unsigned char slaveAddr, unsigned char startRegAddr, unsigned char buflen, unsigned char * bufptr)
void I2C1_readblock(unsigned char slaveAddr, unsigned char startRegAddr, unsigned char buflen, unsigned char * bufptr)
{
    I2C1_start(I2C_START_CMD);
    I2C1_write((slaveAddr << 1) & 0xFE);
    I2C1_write(startRegAddr);
    I2C1_start(I2C_REP_START_CMD);
    I2C1_write((uint8_t)( (slaveAddr << 1) | 0x01) );
    
    //send the data. buflen might be zero!
    for (; buflen>0; --buflen)
    {
//        if(buflen == 1) // if this is last byte
//        {
//            *bufptr++ = I2C1_read();
//            I2C1_master_ack(I2C_DATA_NOACK);
//        }
//        else
//        {
//            *bufptr++ = I2C1_read();
//            I2C1_master_ack(I2C_DATA_ACK);
//        }
        unsigned char ackflag = (buflen == 1);  // NACK
        *bufptr++ = I2C1_read();
        I2C1_master_ack(ackflag);
    }
    I2C1_stop();
}

uint8_t timeout_counter = 0;

//uint8_t I2C1_checkError(void)
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

#define SDA         LATCbits.LATC4
#define SCL         LATCbits.LATC3
#define TRIS_SDA    TRISCbits.TRISC4
#define TRIS_SCL    TRISCbits.TRISC3
#define ANS_SDA     ANSELCbits.ANSC4
#define ANS_SCL     ANSELCbits.ANSC3

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