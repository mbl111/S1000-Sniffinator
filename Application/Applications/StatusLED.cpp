#include "StatusLED.h"
#include <stdio.h>
#include "pico/stdlib.h"


using namespace Pacom;

void StatusLED::Run()
{
	
	SetStatus(StatusLED::BOOTING);
	
	//Loop
	while (true)
	{
		if (_restoreTime > 0 && xTaskGetTickCount() > _restoreTime)
		{
			SetStatus(_lastStatus);
		}

		switch (_currentStatus)
		{
		case StatusLED::BOOTING:
			if (xTaskGetTickCount() - _lastFlashTime > configSTATUSLED_SLOWTICKS) {
				_flashState ^= 0xFF;
				_lastFlashTime = xTaskGetTickCount();
			}
			
			_statusGreen.set(_flashState & 0x01);
			_statusRed.set(_flashState & 0x01);
			break;
			
		case StatusLED::CONNECTED:
			if (xTaskGetTickCount() - _lastFlashTime > configSTATUSLED_SLOWTICKS)
			{
				_flashState ^= 0xFF;
				_lastFlashTime = xTaskGetTickCount();
			}
			
			_statusGreen.set(_flashState & 0x01);
			_statusRed.set(false);
			break;
		
		case StatusLED::ERROR:
			_statusGreen.set(false);
			_statusRed.set(true);
			break;
			
		case StatusLED::CONSOLE_ERROR:
			if (xTaskGetTickCount() - _lastFlashTime > configSTATUSLED_FASTTICKS)
			{
				_flashState ^= 0xFF;
				_lastFlashTime = xTaskGetTickCount();
			}
			_statusGreen.set(false);
			_statusRed.set(_flashState & 0x01);
			break;
			
		case StatusLED::COMMAND:
			if (xTaskGetTickCount() - _lastFlashTime > configSTATUSLED_FASTTICKS)
			{
				_flashState ^= 0xFF;
				_lastFlashTime = xTaskGetTickCount();
			}
			
			_statusGreen.set(_flashState & 0x01);
			_statusRed.set(false);
			break;
			
		default:
			if (xTaskGetTickCount() - _lastFlashTime > configSTATUSLED_SLOWTICKS) {
				_flashState ^= 0xFF;
				_lastFlashTime = xTaskGetTickCount();
			}

			_statusGreen.set(false);
			_statusRed.set(_flashState & 0x01);
			break;
		}
		
		vTaskDelay(pdMS_TO_TICKS(100));
	}
	
}

