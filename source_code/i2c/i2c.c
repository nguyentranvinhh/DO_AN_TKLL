#include "i2c.h"

#define DS1307_ADDRESS  0b01101000
#define EEPROM_ADDRESS  0b01010000

#define TIME_I2C_DELAY				5
#define RTC_WRITE_CYCLE_DELAY		3
#define EEP_WRITE_CYCLE_DELAY		3

char isStarted = 0;

unsigned char BCD2Dec(unsigned char BCD){
	unsigned char L, H;
	L = BCD & 0x0F;
	H = (BCD>>4)*10;
	return (H+L);
}
unsigned char Dec2BCD(unsigned char Dec){
	unsigned char L, H;
	L=Dec % 10;
	H=(Dec/10)<<4;
	return (H+L);
}

void init_i2c(void)
{
    TRIS_SCL = 0;
    TRIS_SDA = 0;
}

void delay_i2c(unsigned char time)
{
    unsigned char i;
    for (i = 0; i < time; i++);
}

char read_sda_pin(void)
{
	if (SDA == 0)
	{
		return 0;
	}
	
	return 1;
}

void i2c_start_cond(void)
{
	if (isStarted)
	{
		// if started, do a restart condition
		// Set SDA to 1
		SDA = 1;
		//delay_i2c(TIME_I2C_DELAY);
		
		SCL = 1;
		//delay_i2c(TIME_I2C_DELAY);
	}
	
	// SCL is high, set SDA form 1 to 0
	SDA = 0;
	//delay_i2c(TIME_I2C_DELAY);
	
	SCL = 0;
	
	isStarted = 1;
}

void i2c_stop_cond(void)
{
	// Set SDA to 0
	SDA = 0;
	//delay_i2c(TIME_I2C_DELAY);
	
	SCL = 1;
	//delay_i2c(TIME_I2C_DELAY);
	
	SDA = 1;
	//delay_i2c(TIME_I2C_DELAY);
	
	isStarted = 0;
}

char i2c_write_byte(char data)
{
	char index, nack;
	
	for (index = 0; index < 8; index++)
	{
		// Write a bit to I2C bus
		if ((data & 0x80) == 0)
		{
			SDA = 0;
		}
		else
		{
			SDA = 1;
		}
		// SDA change propagation delay
		//delay_i2c(TIME_I2C_DELAY);
		
		// Set SCL high to indicate a new valid SDA value is available
		SCL = 1;
		//delay_i2c(TIME_I2C_DELAY);
		
		SCL = 0;
		
		data = data << 1;
	}
	
	// Let the slave drive data
	TRIS_SDA = 1;
	//delay_i2c(TIME_I2C_DELAY);
	
	// Set SCL high to indicate a new valid SDA value is available
	SCL = 1;
	//delay_i2c(TIME_I2C_DELAY);
	
	// SCL high, read out bit
	nack = read_sda_pin();
	
	SCL = 0;
	//delay_i2c(TIME_I2C_DELAY);
	
	TRIS_SDA = 0;
	
	return nack;
}

char i2c_read_byte(char ack)
{
	char data = 0;
	char index;
	
	TRIS_SDA = 1;
	//delay_i2c(TIME_I2C_DELAY);
	
	for (index = 0; index < 8; index++)
	{
		// Set SCL high to indicate a new valid SDA value is available
		SCL = 1;
		//delay_i2c(TIME_I2C_DELAY);
		
		// SCL high, read out bit
		data = (data << 1) | read_sda_pin();
	
		SCL = 0;
		//delay_i2c(TIME_I2C_DELAY);
	}
	
	// Send NACK
	
	TRIS_SDA = 0;
	
	if (ack)
	{
		SDA = 1;		// Send ACK
	}
	else 
	{
		SDA = 0;	// Send NACK
	}
	// SDA change propagation delay
	//delay_i2c(TIME_I2C_DELAY);
	
	// Set SCL high to indicate a new valid SDA value is available
	SCL = 1;
	//delay_i2c(TIME_I2C_DELAY);
	
	SCL = 0;
	
	return data;
}

//DS1307
void write_ds1307(unsigned char address, unsigned char data)
{
	i2c_start_cond();
    i2c_write_byte((DS1307_ADDRESS << 1) + WRITE);
    i2c_write_byte(address);
    i2c_write_byte(Dec2BCD(data));
    i2c_stop_cond();
}

unsigned char read_ds1307(unsigned char address)
{
	unsigned char data;
	i2c_start_cond();
    i2c_write_byte((DS1307_ADDRESS << 1) + WRITE);
    i2c_write_byte(address);  

    i2c_start_cond();
    i2c_write_byte((DS1307_ADDRESS << 1) + READ);
    data = i2c_read_byte(1);
    data = BCD2Dec(data);
    i2c_stop_cond();
	return data;
}

