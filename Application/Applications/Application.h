#pragma once

#include "StatusLED.h"
#include "UARTForwarder.h"
#include "Console.h"
#include "FreeRTOS-Addons/include/thread.hpp"

namespace Pacom
{

	class Application : public cpp_freertos::Thread
	{
		public:
			Application();
			virtual void Run();

			static Pacom::HAL::HardwareUart hwUart0;
			static Pacom::HAL::HardwareUart hwUart1;
	};
	
}

int AppMain();
