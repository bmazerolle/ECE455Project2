#ifndef COMMONCONFIG_H_
#define COMMONCONFIG_H_

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "stm32f4xx.h"
#include "../FreeRTOS_Source/include/FreeRTOS.h"
#include "../FreeRTOS_Source/include/queue.h"
#include "../FreeRTOS_Source/include/semphr.h"
#include "../FreeRTOS_Source/include/task.h"
#include "../FreeRTOS_Source/include/timers.h"

#include "stm32f4_discovery.h"
#include "FreeRTOSHooks.h"

# define MIN_DD_PRIORITY        			(1)
# define MONITOR_DD_PRIORITY       			(2)
# define BASE_DD_PRIORITY 					(3)
# define GENERATOR_DD_PRIORITY   			(configMAX_PRIORITIES - 3)
# define SCHEDULER_DD_PRIORITY	   			(configMAX_PRIORITIES - 1)
# define MAX_DD_TASK_PRIORITY 				(SCHEDULER_DD_PRIORITY - 4)

typedef enum taskType {
    Aperiodic,
	Periodic,
	NoType
} taskType;


typedef struct ddTask_t {
    TickType_t        	deadline;
    TaskFunction_t    	function;
    TaskHandle_t      	handle;
    const char *      	name;
    struct ddTask_t* 	next;
    uint32_t			number;
    struct ddTask_t* 	previous;
    TickType_t        	startTime;
    xTimerHandle      	timer;
    taskType    	  	type;
} ddTask_t;

typedef ddTask_t* ddTaskHandle;


typedef struct ddList_t {
    uint32_t        length;
    ddTaskHandle 	head;
    ddTaskHandle 	tail;
} ddList_t;

typedef ddList_t* ddListHandle;


typedef enum messageCommand_t {
    CREATE,
    DELETE,
    ACTIVE_LIST,
    OVERDUE_LIST
} messageCommand_t;

typedef struct messageHandle {
	messageCommand_t 	type;
    TaskHandle_t      	sender;
    void*             	data;
} messageHandle;

QueueHandle_t xTask1Queue;
QueueHandle_t xTask2Queue;
QueueHandle_t xTask3Queue;
QueueHandle_t xTaskAperiodicQueue;

#endif
