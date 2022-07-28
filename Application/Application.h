#pragma once

#include "StatusLED.h"
#include "FreeRTOS-Addons/include/thread.hpp"

namespace FlashProgrammer
{

	class Application : public cpp_freertos::Thread
	{
		public:
			Application();
			virtual void Run();
			static StatusLED* Status;
	};
	
}

int AppMain();
