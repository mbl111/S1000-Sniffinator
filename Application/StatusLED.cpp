#include "StatusLED.h"
#include <stdio.h>
#include "pico/stdlib.h"


using namespace FlashProgrammer;

void StatusLED::Run()
{
	
	//Setup
	//Status LED
	gpio_init(configSTATUSLED_RED);
	gpio_init(configSTATUSLED_GREEN);
	
	gpio_set_dir(configSTATUSLED_RED, true);
	gpio_set_dir(configSTATUSLED_GREEN, true);
	
	SetStatus(StatusLED::BOOTING);
	
	//Loop
	while (isRunning())
	{
		
		switch (_currentStatus)
		{
		case StatusLED::BOOTING:
			if (xTaskGetTickCount() - _lastFlashTime > configSTATUSLED_SLOWTICKS) {
				_flashState ^= 0xFF;
				_lastFlashTime = xTaskGetTickCount();
			}
			gpio_put(configSTATUSLED_GREEN, _flashState & 0x01);
			gpio_put(configSTATUSLED_RED, _flashState & 0x01);
			break;
		case StatusLED::WAITING_FOR_CONNECTION:
			gpio_put(configSTATUSLED_GREEN, true);
			gpio_put(configSTATUSLED_RED, true);
		case StatusLED::CONNECTED:
			gpio_put(configSTATUSLED_GREEN, true);
			gpio_put(configSTATUSLED_RED, false);
		case StatusLED::PROGRAMMING:
			if (xTaskGetTickCount() - _lastFlashTime > configSTATUSLED_SLOWTICKS) {
				_flashState ^= 0xFF;
				_lastFlashTime = xTaskGetTickCount();
			}
			
			gpio_put(configSTATUSLED_GREEN, _flashState & 0x01);
			gpio_put(configSTATUSLED_RED, false);
			break;
		case StatusLED::ERROR:
			gpio_put(configSTATUSLED_GREEN, false);
			gpio_put(configSTATUSLED_RED, true);
			break;
		default:
			if (xTaskGetTickCount() - _lastFlashTime > configSTATUSLED_SLOWTICKS) {
				_flashState ^= 0xFF;
				_lastFlashTime = xTaskGetTickCount();
			}
			gpio_put(configSTATUSLED_GREEN, false);
			gpio_put(configSTATUSLED_RED, _flashState & 0x01);
			break;
		}
		
		vTaskDelay(pdMS_TO_TICKS(100));
	}
	
}

