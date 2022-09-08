cmake_minimum_required(VERSION 3.5)


target_sources(${PROJECT_NAME} PUBLIC
	${CMAKE_CURRENT_LIST_DIR}/ccondition_variable.cpp
	${CMAKE_CURRENT_LIST_DIR}/cevent_groups.cpp
	${CMAKE_CURRENT_LIST_DIR}/cmem_pool.cpp
	${CMAKE_CURRENT_LIST_DIR}/cmutex.cpp
	${CMAKE_CURRENT_LIST_DIR}/cqueue.cpp
	${CMAKE_CURRENT_LIST_DIR}/cread_write_lock.cpp
	${CMAKE_CURRENT_LIST_DIR}/csemaphore.cpp
	${CMAKE_CURRENT_LIST_DIR}/ctasklet.cpp
	${CMAKE_CURRENT_LIST_DIR}/cthread.cpp
	${CMAKE_CURRENT_LIST_DIR}/ctickhook.cpp
	${CMAKE_CURRENT_LIST_DIR}/ctimer.cpp
	${CMAKE_CURRENT_LIST_DIR}/cworkqueue.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC FreeRTOS-Addons/include )