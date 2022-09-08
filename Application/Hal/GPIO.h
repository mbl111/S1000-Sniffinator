#pragma once

#include "pico/stdlib.h"

#define configGPIO_STATUSRED 25
#define configGPIO_STATUSGREEN 24
#define configGPIO_S1000RESET 23
#define configGPIO_S1000BOOT0 22
#define configGPIO_BLERESET 8

namespace Pacom
{
	namespace HAL
	{
		class GPIO
		{
		  public:
			GPIO(uint8_t gpioNumber, bool output = true);
			~GPIO(){};
			void set(bool on);
			bool get();
		  private:
			uint8_t _gpioNumber;
			bool _output;
		};
	} // namespace HAL
}