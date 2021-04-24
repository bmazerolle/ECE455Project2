/*
 * 	Creator.c
 *  Creates and runs deadline-driven tasks.
 */

#include <Creator.h>

/*
 * Creates an aperiodic task to be injected into the deadline-driven scheduler.
 */
void AperiodicTaskCreator( void *pvParameters ) {
	while(1) {
	        TickType_t taskDeadline = aperiodicTaskDeadline;
	        TickType_t curTime = xTaskGetTickCount();

	        ddTaskHandle newTask = Init_DD_Task();
	        newTask->name = "Aperiodic Task";
	        newTask->number = 4;
	        newTask->type = Aperiodic;
	        newTask->function = AperiodicTask;
	        newTask->startTime = curTime;
	        newTask->deadline = taskDeadline + curTime;

	        Create_DD_Task(newTask);
	        vTaskDelay(portMAX_DELAY);
	}
}

/*
 * Runs the aperiodic task when the scheduler releases it.
 */
void AperiodicTask (void *pvParameters) {
	bool overdueFlag = false;
	ddTaskHandle this = (ddTaskHandle)pvParameters;
	TickType_t curTime, prevTime, timeLeft = 0;
	TickType_t executionTime = aperiodicTaskDuration / portTICK_PERIOD_MS;

    while(1) {
    	// Task is released
    	curTime = xTaskGetTickCount();
    	prevTime = curTime;
    	printf("\nAperiodic Task released at %u ms with priority %u\n", (unsigned int)curTime, (unsigned int)uxTaskPriorityGet( NULL ) );

    	// Execute the task for its pre-set duration
        for(int i = 0; i < executionTime; i++) {
        	if(this->deadline < curTime) {
				overdueFlag = true;
				break;
			}
			curTime = xTaskGetTickCount();
			if(curTime == prevTime) i--;
			prevTime = curTime;
		}

        curTime = xTaskGetTickCount();
    	if(overdueFlag == false) {
    		printf("\nAperiodic task completed at %u ms", (unsigned int)curTime);
    	} else {
    		printf("\nAperiodic task overdue at %u ms", (unsigned int)curTime);
    	}

        // Pause the task until its deadline
        timeLeft = this->deadline - curTime;
        vTaskDelayUntil(&curTime, timeLeft);

        Delete_DD_Task(xTaskGetCurrentTaskHandle(), 'A');
    }
}

/*
 * Creates the 1st periodic task to be injected into the deadline-driven scheduler.
 */
void PeriodicTaskCreator1(void *pvParameters) {
    while(1) {
        TickType_t taskDeadline = periodicTask1Period;
        TickType_t curTime = xTaskGetTickCount();

        ddTaskHandle newTask = Init_DD_Task();
        newTask->name = "Periodic Task 1";
        newTask->number = 1;
        newTask->type = Periodic;
        newTask->function = PeriodicTask1;
        newTask->startTime = curTime;
        newTask->deadline = taskDeadline + curTime;

        Create_DD_Task(newTask);
        vTaskDelay(periodicTask1Period);
    }
}

/*
 * Creates the 2nd periodic task to be injected into the deadline-driven scheduler.
 */
void PeriodicTaskCreator2(void *pvParameters) {
    while(1) {
        TickType_t taskDeadline = periodicTask2Period;
        TickType_t curTime = xTaskGetTickCount();

        ddTaskHandle newTask = Init_DD_Task();
        newTask->name = "Periodic Task 2";
        newTask->number = 2;
        newTask->type = Periodic;
        newTask->function = PeriodicTask2;
        newTask->startTime = curTime;
        newTask->deadline = taskDeadline + curTime;

        Create_DD_Task(newTask);
        vTaskDelay(periodicTask2Period);
    }
}

/*
 * Creates the 3rd periodic task to be injected into the deadline-driven scheduler.
 */
void PeriodicTaskCreator3(void *pvParameters) {
    while(1) {
        TickType_t taskDeadline = periodicTask3Period;
        TickType_t curTime = xTaskGetTickCount();

        ddTaskHandle newTask = Init_DD_Task();
        newTask->name = "Periodic Task 3";
        newTask->number = 3;
        newTask->type = Periodic;
        newTask->function = PeriodicTask3;
        newTask->startTime = curTime;
        newTask->deadline = taskDeadline + curTime;

        Create_DD_Task(newTask);
        vTaskDelay(periodicTask3Period);
    }
}

/*
 * Runs the 1st periodic task when the scheduler releases it.
 */
void PeriodicTask1(void *pvParameters) {
	bool overdueFlag = false;
	ddTaskHandle this = (ddTaskHandle)pvParameters;
	TickType_t curTime, prevTime, timeLeft = 0;
	TickType_t executionTime = periodicTask1Duration / portTICK_PERIOD_MS;

    while(1) {
        if(timeLeft != 0) vTaskDelayUntil(&curTime, timeLeft);
    	// Release the task
    	curTime = xTaskGetTickCount();
    	prevTime = curTime;
    	printf("\nPeriodic Task 1 released at %u ms with priority %u", (unsigned int)curTime, (unsigned int)uxTaskPriorityGet( NULL ) );

    	// Execute the task for its pre-set duration
        for(int i = 0; i < executionTime; i++) {
        	if(this->deadline < curTime) {
				overdueFlag = true;
				break;
			}
        	curTime = xTaskGetTickCount();
			if( curTime == prevTime ) i--;
			prevTime = curTime;
        }
        curTime = xTaskGetTickCount();
    	if(overdueFlag == false) {
    		printf("\nPeriodic Task 1 completed at %u ms", (unsigned int)curTime);
		} else {
			printf("\nPeriodic Task 1 overdue at %u ms", (unsigned int)curTime);
		}

		// Delay the next iteration of the task until the next period
        timeLeft = this->deadline - curTime;
        Delete_DD_Task(xTaskGetCurrentTaskHandle(), '1');
    }
}

/*
 * Runs the 2nd periodic task when the scheduler releases it.
 */
void PeriodicTask2(void *pvParameters) {
	bool overdueFlag = false;
	ddTaskHandle this = (ddTaskHandle)pvParameters;
	TickType_t curTime, prevTime, timeLeft = 0;
	TickType_t executionTime = periodicTask2Duration / portTICK_PERIOD_MS;

    while(1) {
        if(timeLeft != 0) vTaskDelayUntil(&curTime, timeLeft);
    	// Release the task
    	curTime = xTaskGetTickCount();
    	prevTime = curTime;
    	printf("\nPeriodic Task 2 released at %u ms with priority %u", (unsigned int)curTime, (unsigned int)uxTaskPriorityGet(NULL));

    	// Execute the task for its pre-set duration
        for(int i = 0; i < executionTime; i++) {
			if(this->deadline < curTime) {
				overdueFlag = true;
				break;
			}
        	curTime = xTaskGetTickCount();
			if( curTime == prevTime ) i--;
			prevTime = curTime;
        }
        curTime = xTaskGetTickCount();
    	if(overdueFlag == false) {
			printf("\nPeriodic Task 2 completed at %u ms", (unsigned int)curTime);
		} else {
			printf("\nPeriodic Task 2 overdue at %u ms", (unsigned int)curTime);
		}

        // Delay the next iteration of the task until the next period
        timeLeft = this->deadline - curTime;
        //if(timeLeft != 0) vTaskDelayUntil(&curTime, timeLeft);
        Delete_DD_Task(xTaskGetCurrentTaskHandle(), '2');
    }
}


/*
 * Runs the 3rd periodic task when the scheduler releases it.
 */
void PeriodicTask3(void *pvParameters) {
	bool overdueFlag = false;
	ddTaskHandle this = (ddTaskHandle)pvParameters;
	TickType_t curTime, prevTime, timeLeft = 0;
	TickType_t executionTime = periodicTask3Duration / portTICK_PERIOD_MS;

    while(1) {
        if(timeLeft != 0) vTaskDelayUntil(&curTime, timeLeft);
    	// Release the task
    	curTime = xTaskGetTickCount();
    	prevTime = curTime;
    	printf("\nPeriodic Task 3 released at %u ms with priority %u", (unsigned int)curTime, (unsigned int)uxTaskPriorityGet( NULL ) );

    	// Execute the task for its pre-set duration
        for(int i = 0; i < executionTime; i++) {
        	if(this->deadline < curTime) {
				overdueFlag = true;
				break;
			}
        	curTime = xTaskGetTickCount();
			if(curTime == prevTime) i--;
			prevTime = curTime;
        }
        curTime = xTaskGetTickCount();
    	if(overdueFlag == false) {
			printf("\nPeriodic Task 3 completed at %u ms", (unsigned int)curTime);
		} else {
			printf("\nPeriodic Task 3 overdue at %u ms", (unsigned int)curTime);
		}

		// Delay the next iteration of the task until the next period
        timeLeft = this->deadline - curTime;
        Delete_DD_Task(xTaskGetCurrentTaskHandle(), '3');
    }
}
