/*
*******************************************************************************
** 
**  : 
*******************************************************************************
*/

#ifndef __GPIO_LIB_H__
#define __GPIO_LIB_H__

#ifdef __cplusplus
extern "C" {
#endif


typedef struct gpio_groupbit_info_t
{
	unsigned int  groupnumber;
	unsigned int  bitnumber;
	unsigned int  value;
}gpio_groupbit_info_t;

#define GPIO_SET_DIR            0x1
#define GPIO_GET_DIR            0x2
#define GPIO_READ_BIT           0x3
#define GPIO_WRITE_BIT          0x4
#define GPIO_SET_74164          0x5


typedef enum gpio_addr_e
{
				// 3531	8		3531 2		3520A 2
	GPIO_ADDR_02_1=0,
	GPIO_ADDR_02_3,
	GPIO_ADDR_02_7,
	GPIO_ADDR_03_4,
	GPIO_ADDR_03_5,
	GPIO_ADDR_03_6,
	GPIO_ADDR_03_7,
	GPIO_ADDR_04_6 ,	//
	GPIO_ADDR_05_0,	//						
	GPIO_ADDR_05_1,		//						
	GPIO_ADDR_05_2,		//						
	GPIO_ADDR_05_3,		//
	GPIO_ADDR_07_1,
	GPIO_ADDR_07_3,
	GPIO_ADDR_07_7,		//	
	GPIO_ADDR_08_2,		//	
	GPIO_ADDR_08_3,		//						
	GPIO_ADDR_08_6,		//						
	GPIO_ADDR_09_6,		//  		
	GPIO_ADDR_09_7,		// 		
	GPIO_ADDR_10_0,		// 		
	GPIO_ADDR_10_1,		// 		
	GPIO_ADDR_10_2,		// 		
	GPIO_ADDR_10_3,		// 		
	GPIO_ADDR_10_4,		// 		
	GPIO_ADDR_10_5,		//  		
	GPIO_ADDR_10_6,		//  		
	GPIO_ADDR_10_7,		// 		
	GPIO_ADDR_11_0,		// 
	GPIO_ADDR_11_1,		// 
	GPIO_ADDR_11_2,		// 		
	GPIO_ADDR_11_3,		// 
	GPIO_ADDR_12_0,		// 
	GPIO_ADDR_12_1,		// 
	GPIO_ADDR_12_2,		// 
	GPIO_ADDR_12_3,		// 		
	GPIO_ADDR_12_6,		//
	GPIO_ADDR_13_0,
	GPIO_ADDR_14_0,		// 
	GPIO_ADDR_14_1,		// 
	GPIO_ADDR_14_2,		// 
	GPIO_ADDR_14_5,		// 			
	GPIO_ADDR_17_3,		// USB 		USB 
	GPIO_ADDR_17_4,		// USB 		USB 
	GPIO_ADDR_17_5,		// USB 		USB 
	GPIO_ADDR_17_6,		// USB 		USB 
}gpio_addr_e;



#define GPIO_FILE       "/dev/hi_gpio"



void gpioSetUsbPower();

int gpioLibInit();
void gpioLibDestroy();


int setGpioDir(int addr, int value);
int getGpioValue(int addr);
int setGpioValue(int addr, int value);

/* ic 0  1 */
void gpioSetIcPower(int value);

/*  0  1 */
void gpioSetPower(int value);

/*  0  1 */
void gpioSetDiskPower(int value);

/*  0  1 */
void gpioSetBeep(int value);

/* io  0  1 */
int gpioGetStartValue();

/* io  0  1 */
int gpioGetRestartValue();
/* io  0  1 */
void gpioSetAlarm(int value);
/* io  0  1 */
void gpioSetRec(int value);
/* io  0  1 */
void gpioSetHdd(int value);


#ifdef __cplusplus
}
#endif


#endif//__GPIO_LIB_H__

