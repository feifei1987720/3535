
#ifndef _GPIO_I2C_H_
#define _GPIO_I2C_H_


#define GPIO_I2C_READ   0x01
#define GPIO_I2C_WRITE  0x03


unsigned char gpio_i2c_read(unsigned char devaddress, unsigned char address);
void gpio_i2c_write(unsigned char devaddress, unsigned char address, unsigned char value);


#endif

