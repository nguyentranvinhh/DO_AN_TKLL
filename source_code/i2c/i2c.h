#ifndef _I2C_H_
#define _I2C_H_

#include <p18f4620.h>


#define WRITE		0
#define READ		1

#define TRIS_SCL	TRISCbits.RC3
#define TRIS_SDA	TRISCbits.RC4

#define SCL         PORTCbits.RC3
#define SDA         PORTCbits.RC4

unsigned char BCD2Dec(unsigned char BCD);
unsigned char Dec2BCD(unsigned char Dec);
void init_i2c(void);
void i2c_control(unsigned char mRW_bit);
void i2c_start(void);
void i2c_restart(void);
void i2c_stop(void);
void i2c_write(unsigned char abyte);
unsigned char i2c_read(void);

//DS1307
void write_ds1307(unsigned char address, unsigned char data);
unsigned char read_ds1307(unsigned char address);


#endif