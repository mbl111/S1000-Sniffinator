#include "Hal/S1000.h"
#include "FreeRTOS.h"
#include "task.h"

Pacom::HAL::S1000 *Pacom::HAL::S1000::_instance = nullptr;

void Pacom::HAL::S1000::restart()
{
	cpp_freertos::LockGuard guard(_mutex);
	_restartPin.set(true);
	vTaskDelay(pdMS_TO_TICKS(10));
	_restartPin.set(false);
}

void Pacom::HAL::S1000::enterBootloader()
{
	cpp_freertos::LockGuard guard(_mutex);
	if (!_bootloaderEnteredManually)
		_bootloaderPin.set(true);
	vTaskDelay(pdMS_TO_TICKS(10));
	
	_restartPin.set(true);
	vTaskDelay(pdMS_TO_TICKS(10));
	_restartPin.set(false);
	
	vTaskDelay(pdMS_TO_TICKS(100));
	if (!_bootloaderEnteredManually)
		_bootloaderPin.set(false);

	_bootloaderEnteredManually = false;
}

void Pacom::HAL::S1000::setBoot0Pin()
{
	cpp_freertos::LockGuard guard(_mutex);
	_bootloaderPin.set(true);
	_bootloaderEnteredManually = true;
}

void Pacom::HAL::S1000::resetBoot0Pin()
{
	cpp_freertos::LockGuard guard(_mutex);
	_bootloaderPin.set(false);
	_bootloaderEnteredManually = false;
}

void Pacom::HAL::S1000::resetBle()
{
	cpp_freertos::LockGuard guard(_mutex);
	_bleResetPin.set(false);
	vTaskDelay(pdMS_TO_TICKS(10));
	_bleResetPin.set(true);
}