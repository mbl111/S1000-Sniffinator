#pragma once

#include "UARTForwarder.h"
#include "Console.h"
#include "StatusLED.h"
#include "HardwareVersion.h"

namespace Pacom
{
		class ApplicationManager
		{
		  public:
			static ApplicationManager &Instance();

			void Initialize();

			HWUartToUSBForwarder &getLogUartOutApplication() { return *_logUart; };
			USBToUartForwarder &getLogUartInApplication() { return *_logUartIn;};
			HWUartToUSBForwarder &getBleUartOutApplication() { return *_bleUart; };
			USBToUartForwarder &getBleUartInApplication() { return *_bleUartIn; };

			UartToUSBForwarder &getNonIso485TXApplication() { return *_NonIso485TX; };
			UartToUSBForwarder &getNonIso485RXApplication() { return *_NonIso485RX; };
			UartToUSBForwarder &getIso485TXApplication() { return *_Iso485TX; };
			UartToUSBForwarder &getIso485RXApplication() { return *_Iso485RX; };

			Console &getConsoleApplication() { return *_console; };

			StatusLED &getStatusLEDApplication() { return *_statusLED; };

			HAL::HardwareVersion &getHardwareVersion() { return *_hardwareVersion; };
		  private:
			ApplicationManager(){};
			static ApplicationManager *_instance;

			StatusLED *_statusLED = nullptr;
			HWUartToUSBForwarder *_logUart = nullptr;
			USBToUartForwarder *_logUartIn = nullptr;
			HWUartToUSBForwarder *_bleUart = nullptr;
			USBToUartForwarder *_bleUartIn = nullptr;
			UartToUSBForwarder *_NonIso485TX = nullptr;
			UartToUSBForwarder *_NonIso485RX = nullptr;
			UartToUSBForwarder *_Iso485TX = nullptr;
			UartToUSBForwarder *_Iso485RX = nullptr;
			Console *_console;
			
			
			HAL::HardwareVersion *_hardwareVersion = nullptr;
		};
}