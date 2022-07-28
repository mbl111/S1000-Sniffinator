#pragma once

#include "FreeRTOS.h"
#include "semphr.h"

namespace FreeRTOSCPP
{
	class Mutex
	{
	public:
		Mutex();
		~Mutex();
		bool Lock(TickType_t timeout = portMAX_DELAY);
		bool Unlock();
	private:
		QueueHandle_t _handle;
	};
	
	class LockGuard
	{
	public:
		LockGuard(Mutex& mutex);
		
		~LockGuard();
	private:
		LockGuard(const LockGuard&); //Hide Copy constructor
		Mutex& _mutex;
	};
}