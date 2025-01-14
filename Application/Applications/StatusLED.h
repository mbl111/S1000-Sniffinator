#pragma once
#include "FreeRTOS.h"
#include "task.h"
#include "Hal/GPIO.h"
#include "FreeRTOS-Addons/include/thread.hpp"



#define configSTATUSLED_FASTTICKS pdMS_TO_TICKS(100)
#define configSTATUSLED_SLOWTICKS pdMS_TO_TICKS(600)


namespace Pacom
{
	class StatusLED:public cpp_freertos::Thread
	{
	public:
	  enum Status
	  {
			NONE,
			BOOTING,
			CONNECTED,
			ERROR,
		    CONSOLE_ERROR,
			COMMAND,
			
		};
		
	public:
		StatusLED() : Thread("Status LED", 1024, 1), _statusRed(configGPIO_STATUSRED), _statusGreen(configGPIO_STATUSGREEN){};
	  
		void SetStatus(Status newStatus)
		{
			//printf("[STATUSLED] Setting Status: %s\n", GetStatusName(newStatus).c_str());
			this->_lastStatus = NONE;
			this->_currentStatus = newStatus;
			_flashState = 0;
			_restoreTime = 0;
			_lastFlashTime = xTaskGetTickCount();

			_statusGreen.set(false);
			_statusRed.set(true);
		}

		void SetTemporaryStatus(Status newStatus, uint32_t ticksToStayActive)
		{
			SetTemporaryStatus(newStatus, GetStatus(), ticksToStayActive);
		}

		void SetTemporaryStatus(Status newStatus, Status restoreStatus, uint32_t ticksToStayActive)
		{
			//printf("[STATUSLED] Setting Temporary Status: %s\n", GetStatusName(newStatus).c_str());
			if (_lastStatus == NONE)
				this->_lastStatus = restoreStatus;
			
			this->_currentStatus = newStatus;
			_flashState = 0;
			_lastFlashTime = xTaskGetTickCount();
			_restoreTime = xTaskGetTickCount() + ticksToStayActive;
		}
		Status GetStatus(){return this->_currentStatus;}

		inline std::string GetStatusName(Status status) {
			switch (status)
			{
			case Pacom::StatusLED::NONE:
				return "NONE";
			case Pacom::StatusLED::BOOTING:
				return "BOOTING";
			case Pacom::StatusLED::COMMAND:
				return "COMMAND";
			case Pacom::StatusLED::CONNECTED:
				return "CONNECTED";
			case Pacom::StatusLED::CONSOLE_ERROR:
				return "CONSOLE ERROR";
			case Pacom::StatusLED::ERROR:
				return "ERROR";	
			}
		}

	  private:
		void Run();
		TaskHandle_t* _taskHandle;
		Status _currentStatus;
		Status _lastStatus;
		uint32_t _restoreTime;
		uint32_t _lastFlashTime;
		uint8_t _flashState;
		HAL::GPIO _statusRed;
		HAL::GPIO _statusGreen;
	};
	
	void TaskHandler(void* pvParams);
}