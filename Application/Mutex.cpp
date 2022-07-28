#include "Mutex.h"

using namespace FreeRTOSCPP;

Mutex::Mutex()
{
	_handle = xSemaphoreCreateMutex();
	
	if (_handle == NULL)
	{
		configASSERT(!"Failed to create Mutex!");
	}
}

Mutex::~Mutex()
{
	vSemaphoreDelete(_handle);
}

bool Mutex::Lock(TickType_t timeout)
{
	BaseType_t success = xSemaphoreTake(_handle, timeout);
	return success == pdTRUE ? true : false;
}

bool Mutex::Unlock()
{
	BaseType_t success = xSemaphoreGive(_handle);
	return success == pdTRUE ? true : false;
}

LockGuard::LockGuard(Mutex& mutex)
	: _mutex(mutex)
{
	_mutex.Lock();
}

LockGuard::~LockGuard()
{
	_mutex.Unlock();
}