/*
 * 	Scheduler.c
 *  Schedules the created deadline-driven tasks
 */

#include <Scheduler.h>

static ddList_t activeList;
static ddList_t overdueList;

static QueueHandle_t xSchedulerMessageQueue;
static QueueHandle_t xMonitorMessageQueue;


/*
 * Accepts scheduling messages and calls scheduling helper functions accordingly.
 */
void DD_Scheduler(void *pvParameters) {
	messageHandle message;
    ddTaskHandle taskHandle = NULL;

    while(1) {
        if(xQueueReceive(xSchedulerMessageQueue, (void*)&message, portMAX_DELAY) == pdTRUE) {
            Transfer_DD_TaskList(&activeList, &overdueList); // Transfer any overdue tasks to the overdue list
            while(overdueList.length > 5) Remove_DD_TaskList(NULL, &overdueList, false, true); // Trim down the overdue list if larger than 5

            if(xTaskGetTickCount() > 1500){
            	exit(0);
            }

			if(message.type == CREATE) {
				// Insert the deadline driven task into the active list
				taskHandle = (ddTaskHandle)message.data;
				Insert_DD_Task(taskHandle, &activeList);

				uint16_t flag = 1;
				char name[32] = "";
				strcat(name, taskHandle->name);
				if (name[14] == '1') {
					xQueueOverwrite(xTask1Queue, &flag);
				} else if(name[14] == '2'){
					xQueueOverwrite(xTask2Queue, &flag);
				} else if(name[14] == '3'){
					xQueueOverwrite(xTask3Queue, &flag);
				} else {
					xQueueOverwrite(xTaskAperiodicQueue, &flag);
				}

			} else if (message.type == DELETE) {
				// Remove the deadline driven task from its list
				Remove_DD_TaskList(message.sender, &activeList, false, false);

				uint16_t flag = 1;
				char name[32] = "";
				strcat(name, taskHandle->name);
				if (name[14] == '1') {
					xQueueOverwrite(xTask1Queue, &flag);
				} else if(name[14] == '2'){
					xQueueOverwrite(xTask2Queue, &flag);
				} else if(name[14] == '3'){
					xQueueOverwrite(xTask3Queue, &flag);
				} else {
					xQueueOverwrite(xTaskAperiodicQueue, &flag);
				}

			} else if (message.type == ACTIVE_LIST) {
				// Get the active list
				message.data = (void*)Get_DD_TaskList(&activeList);

				// Clear the monitor queue if full
				if(uxQueueSpacesAvailable(xMonitorMessageQueue) == 0) xQueueReset(xMonitorMessageQueue);

				// Send data to monitor queue
				if( xMonitorMessageQueue == NULL) return;
				if( xQueueSend( xMonitorMessageQueue, &message, (TickType_t) portMAX_DELAY ) != pdPASS ) return;

			} else if (message.type == OVERDUE_LIST) {
				// Get the overdue list
				message.data = (void*)Get_DD_TaskList(&overdueList);

				// Clear the monitor queue if full
				if(uxQueueSpacesAvailable(xMonitorMessageQueue) == 0) xQueueReset(xMonitorMessageQueue);

				// Send data to monitor queue
				if(xMonitorMessageQueue == NULL) return;
				if(xQueueSend( xMonitorMessageQueue, &message, (TickType_t) portMAX_DELAY ) != pdPASS) return;

			}
        }
    }
}

/*
 * Initializes the active and overdue lists and corresponding tasks/queues
 */
void DD_Scheduler_Init() {
    Init_DD_TaskList(&overdueList);
    Init_DD_TaskList(&activeList);

    // Assign highest priority to inter-task communications
    xSchedulerMessageQueue = xQueueCreate(MAX_DD_TASK_PRIORITY, sizeof(messageHandle));
    vQueueAddToRegistry(xSchedulerMessageQueue,"Scheduler Queue");

    xMonitorMessageQueue = xQueueCreate(2, sizeof(messageHandle));
    vQueueAddToRegistry(xMonitorMessageQueue,"Monitor Queue");

    xTaskCreate(DD_Scheduler , "DD Scheduler Task" 	, configMINIMAL_STACK_SIZE , NULL , SCHEDULER_DD_PRIORITY , NULL);
    xTaskCreate(Monitor		 , "Monitor Task"   	, configMINIMAL_STACK_SIZE , NULL , MONITOR_DD_PRIORITY   , NULL);

}

/*
 * Creates a deadline-driven task as a FreeRTOS task and sends a create notification to the scheduler
 */
void Create_DD_Task(ddTaskHandle task) {

    if( task == NULL ) return;

    xTaskCreate(task->function,
                task->name,
                configMINIMAL_STACK_SIZE,
                (void*)task,
				MIN_DD_PRIORITY,
                &(task->handle));

    if(task->handle == NULL) return;

    // Suspend the task until it was been added to the deadline driven scheduler
    vTaskSuspend(task->handle);

    messageHandle message = {CREATE, xTaskGetCurrentTaskHandle(), task};

    if(xSchedulerMessageQueue == NULL) return;
    if(xQueueSend(xSchedulerMessageQueue, &message, portMAX_DELAY) != pdPASS) return;

    // Resume the task once it's been added to the deadline driven scheduler
    uint16_t flag = 0;
	char name[32] = "";
	strcat(name, task->name);
	if (name[14] == '1') {
		xQueueReceive(xTask1Queue, &flag, portMAX_DELAY);
	} else if(name[14] == '2'){
		xQueueReceive(xTask2Queue, &flag, portMAX_DELAY);
	} else if(name[14] == '3'){
		xQueueReceive(xTask3Queue, &flag, portMAX_DELAY);
	} else {
		xQueueReceive(xTaskAperiodicQueue, &flag, portMAX_DELAY);
	}
    vTaskResume(task->handle);
    return;
}

/*
 * Sends a delete command to deadline-driven scheduler and then deletes the FreeRTOS task
 */
void Delete_DD_Task(TaskHandle_t task, char number) {
    if(task == NULL) return;

    messageHandle task_message = {DELETE, task, NULL};

    // Send the message to the scheduler queue
    if(xSchedulerMessageQueue == NULL) return;
	if(xQueueSend(xSchedulerMessageQueue, &task_message, portMAX_DELAY) != pdPASS) return;

	uint16_t flag = 0;
	if(number == '1') {
		xQueueReceive(xTask1Queue, &flag, portMAX_DELAY);
	} else if(number == '2'){
		xQueueReceive(xTask2Queue, &flag, portMAX_DELAY);
	} else if(number == '3'){
		xQueueReceive(xTask3Queue, &flag, portMAX_DELAY);
	} else {
		xQueueReceive(xTaskAperiodicQueue, &flag, portMAX_DELAY);
	}

    vTaskDelete(task);
    return;
}

/*
 * Task that calls the active/overdue list printers every 500ms
 */
void Monitor(void *pvParameters) {

	uint32_t delay = 10;
	uint32_t totalDelay = 0;
	vTaskDelay(delay / portTICK_PERIOD_MS);
	delay = 500;
    while(1) {
    	totalDelay += 500;
        vTaskDelay(delay / portTICK_PERIOD_MS);
    	Get_Active_DD_TaskList(totalDelay);
        Get_Overdue_DD_TaskList(totalDelay);
    }
}

/*
 * Prints the contents of the active task list
 */
void Get_Active_DD_TaskList(uint32_t totalDelay) {
	messageHandle activeMessage = {ACTIVE_LIST, NULL, NULL};

	// Send Message to scheduler queue
	if(xSchedulerMessageQueue == NULL) return;
	if(xQueueSend(xSchedulerMessageQueue, &activeMessage, (TickType_t)portMAX_DELAY ) != pdPASS) return;

	// Wait for the response data from the monitor queue
	if(xMonitorMessageQueue == NULL) return;
	if(xQueueReceive( xMonitorMessageQueue, &activeMessage, (TickType_t) portMAX_DELAY) == pdTRUE) {
		printf("\n\nActive Tasks at %u: \n%s\n", (unsigned int)totalDelay, (char*)(activeMessage.data));
		vPortFree(activeMessage.data);
		activeMessage.data = NULL;
	}

	return;
}

/*
 * Prints the contents of the overdue task list
 */
void Get_Overdue_DD_TaskList(uint32_t totalDelay) {
	messageHandle overdueMessage = {OVERDUE_LIST, NULL, NULL};

    // Send Message to scheduler queue
    if(xSchedulerMessageQueue == NULL) return;
	if(xQueueSend(xSchedulerMessageQueue, &overdueMessage, (TickType_t)portMAX_DELAY ) != pdPASS) return;

    // Wait for the response data from the monitor queue
    if(xMonitorMessageQueue == NULL) return;
	if(xQueueReceive( xMonitorMessageQueue, &overdueMessage, (TickType_t) portMAX_DELAY) == pdTRUE) {
		printf("Overdue Tasks at %u: \n%s\n", totalDelay, (char*)(overdueMessage.data));
		vPortFree(overdueMessage.data);
		overdueMessage.data = NULL;
	}

    return;
}
