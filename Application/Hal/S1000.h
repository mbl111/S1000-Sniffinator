#pragma once

#include "Hal/GPIO.h"
#include "FreeRTOS-Addons/include/mutex.hpp"

namespace Pacom
{
	namespace HAL
	{
		class S1000
		{
		  public:
			static S1000 &Instance(){
				if (_instance == nullptr){
					_instance = new S1000();
				}
				
				return *_instance;
			};
				
			void restart();
			void enterBootloader();
			void resetBle();

		  private:
			S1000() : _restartPin(configGPIO_S1000RESET), _bootloaderPin(configGPIO_S1000BOOT0), _bleResetPin(configGPIO_BLERESET){
			};
			GPIO _restartPin;
			GPIO _bootloaderPin;
			GPIO _bleResetPin;
			cpp_freertos::MutexStandard _mutex;
			static S1000 *_instance;
		};
	} // namespace HAL
}


