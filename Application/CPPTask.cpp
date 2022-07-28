#include "CPPTask.h"

using namespace FreeRTOSCPP;

void Task::Start()
{

	if (_running) 
	{
		printf("%s", ToString().c_str());
		configASSERT("Task is already running");
	}

	
	BaseType_t result = xTaskCreate(&_taskRunner, _name.c_str(), _stackDepth, this, _priority, &_taskHandle);

	if (result == pdFALSE) {
		printf("%s", ToString().c_str());
		configASSERT("Failed to start the Task");
	}

#ifdef FLASHPROGRAMMER_DEBUG
	printf("Task Starting: %s\n", ToString().c_str());
#endif

}

void Task::_taskRunner(void* pvParams)
{
	Task* taskpointer = static_cast<Task *>(pvParams);
	taskpointer->_running = true;
	taskpointer->Run();
#ifdef FLASHPROGRAMMER_DEBUG
		printf("Task Returned: %s\n", taskpointer->ToString().c_str());
#endif
	
	vTaskDelete(taskpointer->_taskHandle);
	

}