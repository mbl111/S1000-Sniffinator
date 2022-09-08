#pragma once
#ifndef _H_PACOM_CONSOLE
#define _H_PACOM_CONSOLE

#include "FreeRTOS-Addons/include/thread.hpp"
#include "FreeRTOS-Addons/include/mutex.hpp"
#include "Applications/UARTForwarder.h"
#include "pico/stdio/driver.h"

namespace Pacom
{

	class Console : public cpp_freertos::Thread
	{
	  public:
		Console(USBUART usbUart) : Thread("Console", 1024, 5), _usbSerialIndex(usbUart) {};
		virtual void Run();
		void printHelpMessage();
		
		int stdioIn(char *buf, int len);
		void stdioOut(const char *buf, int len);
		void stdioFlush(void);

	  private:
		USBUART _usbSerialIndex;
		cpp_freertos::MutexStandard _logLock;
		
	};


	void stdioDriverOut(const char *buf, int len);
	void stdioDriverFlush(void);
	int stdioDriverIn(char *buf, int len);

} // namespace Pacom

#endif