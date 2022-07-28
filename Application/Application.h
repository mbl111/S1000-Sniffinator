#pragma once

#include "StatusLED.h"
#include "CPPTask.h"

namespace FlashProgrammer
{

	class Application : public FreeRTOSCPP::Task
	{
		public:
			Application();
			virtual void Run();
			static StatusLED* Status;
	};
	
}

int AppMain();
