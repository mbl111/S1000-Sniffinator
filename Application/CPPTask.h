#pragma once

#include <string>
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"


namespace FreeRTOSCPP
{
	class Task
	{
	protected:
		Task(std::string name, uint32_t stackDepth, uint8_t priority)
			: _name(name), _stackDepth(stackDepth), _priority(priority), _running(false) {};
		~Task() {};
		virtual void Run() = 0;
	public:
		void Start();
		std::string ToString()
		{
			return "Task:" + _name;
		}
		bool isRunning()
		{
			return _running;
		}
	private:
		std::string _name;
		TaskHandle_t _taskHandle;
		unsigned int _stackDepth;
		unsigned int _priority;
		bool _running;
		static void _taskRunner(void* pvParams);
	};
}