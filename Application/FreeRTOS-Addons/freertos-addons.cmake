cmake_minimum_required(VERSION 3.5)


target_sources(AppMain PUBLIC
	FreeRTOS-Addons/ccondition_variable.cpp
	FreeRTOS-Addons/cevent_groups.cpp
	FreeRTOS-Addons/cmem_pool.cpp
	FreeRTOS-Addons/cmutex.cpp
	FreeRTOS-Addons/cqueue.cpp
	FreeRTOS-Addons/cread_write_lock.cpp
	FreeRTOS-Addons/csemaphore.cpp
	FreeRTOS-Addons/ctasklet.cpp
	FreeRTOS-Addons/cthread.cpp
	FreeRTOS-Addons/ctickhook.cpp
	FreeRTOS-Addons/ctimer.cpp
	FreeRTOS-Addons/cworkqueue.cpp
)

target_include_directories(AppMain PUBLIC FreeRTOS-Addons/include )