#include "gpioLib.h"
#include "configLib.h"

int main()
{
	configLibInit();
	gpioLibInit();

	setGpioDir(GPIO_ADDR_17_3, 1);
	setGpioDir(GPIO_ADDR_17_4, 1);
	setGpioDir(GPIO_ADDR_17_5, 1);
	setGpioDir(GPIO_ADDR_17_6, 1);

//	setGpioDir(GPIO_ADDR_07_7, 1);

//	gpioSetIcPower(1);
	gpioSetUsbPower();

	gpioLibDestroy();

	return 0;
}

