#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "hardware/clocks.h"
#include "pico/stdlib.h"
#include "Applications/Application.h"


void vApplicationMallocFailedHook(void);
void vApplicationIdleHook(void);
void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName);

#define traceTASK_SWITCHED_IN() printf((int)pxCurrentTCB->pcTaskName);

extern "C" int main(void)
	{
		//stdio_init_all();
		const char *rtos_name;
		#if ( portSUPPORT_SMP == 1 )
		rtos_name = "FreeRTOS SMP";
		#else
		rtos_name = "FreeRTOS";
		#endif

		#if ( portSUPPORT_SMP == 1 ) && ( configNUM_CORES == 2 )
		printf("%s on both cores:\n", rtos_name);
		vLaunch();
		#endif

		#if ( mainRUN_ON_CORE == 1 )
		printf("%s on core 1:\n", rtos_name);
		multicore_launch_core1(vLaunch);
		while (true) ;
		#else
		printf("%s on core 0:\n", rtos_name);
		
		return AppMain();
		#endif

		return -1;
	}
	void vApplicationMallocFailedHook(void)
	{
		/* Called if a call to pvPortMalloc() fails because there is insufficient
		free memory available in the FreeRTOS heap.  pvPortMalloc() is called
		internally by FreeRTOS API functions that create tasks, queues, software
		timers, and semaphores.  The size of the FreeRTOS heap is set by the
		configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */

		/* Force an assert. */
		configASSERT((volatile void *) NULL);
	}
	/*-----------------------------------------------------------*/

	void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName)
	{
		(void) pcTaskName;
		(void) pxTask;

		/* Run time stack overflow checking is performed if
		configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
		function is called if a stack overflow is detected. */

		/* Force an assert. */
		configASSERT((volatile void *) NULL);
	}
	/*-----------------------------------------------------------*/

	void vApplicationIdleHook(void)
	{
		volatile size_t xFreeHeapSpace;

		/* This is just a trivial example of an idle hook.  It is called on each
		cycle of the idle task.  It must *NOT* attempt to block.  In this case the
		idle task just queries the amount of FreeRTOS heap that remains.  See the
		memory management section on the http://www.FreeRTOS.org web site for memory
		management options.  If there is a lot of heap memory free then the
		configTOTAL_HEAP_SIZE value in FreeRTOSConfig.h can be reduced to free up
		RAM. */
		xFreeHeapSpace = xPortGetFreeHeapSize();

		/* Remove compiler warning about xFreeHeapSpace being set but never used. */
		(void) xFreeHeapSpace;
	}
	/*-----------------------------------------------------------*/