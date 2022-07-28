#pragma once
#include "FreeRTOS.h"
#include "task.h"
#include "CPPTask.h"

#define configSTATUSLED_RED 25
#define configSTATUSLED_GREEN 24

#define configSTATUSLED_FASTTICKS pdMS_TO_TICKS(200)
#define configSTATUSLED_SLOWTICKS pdMS_TO_TICKS(600)

namespace FlashProgrammer
{
	class StatusLED:public FreeRTOSCPP::Task
	{
	public:
		enum Status
		{
			BOOTING,
			WAITING_FOR_CONNECTION,
			CONNECTED,
			PROGRAMMING,
			ERROR,
		};
		
	public:
		void SetStatus(Status newStatus)
		{ 
			this->_currentStatus = newStatus;
			_flashState = 0;
			_lastFlashTime = xTaskGetTickCountFromISR();
		}
		Status GetStatus()
		{return this->_currentStatus;
		}
		StatusLED() : Task("Status LED", 1024, 5)
		{}
		;
	private:
		void Run();
		TaskHandle_t* _taskHandle;
		Status _currentStatus;
		uint32_t _lastFlashTime;
		uint8_t _flashState;
	};
	
	void TaskHandler(void* pvParams);
}