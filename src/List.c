/*
 * 	List.c
 *  Creates and runs deadline-driven tasks.
 */

#include <List.h>

/*
 * Mallocs/initializes a deadline driven task struct
 */
ddTaskHandle Init_DD_Task() {
	ddTaskHandle newtask = (ddTaskHandle)pvPortMalloc(sizeof(ddTask_t));

    newtask->deadline = 0;
    newtask->function = NULL;
    newtask->handle = NULL;
    newtask->name = "";
    newtask->number = -1;
    newtask->next = NULL;
    newtask->previous = NULL;
    newtask->startTime = 0;
    newtask->timer = NULL;
    newtask->type = NoType;

    return newtask;
}

/*
 * Zeroes out a deadline-driven task struct and frees the associated memory.
 */
bool Free_DD_Task(ddTaskHandle task) {
	// Catch bad inputs
	if( task == NULL || task->next != NULL || task->previous != NULL) return false;

	task->deadline = 0;
    task->function = NULL;
	task->handle = NULL;
    task->name = "";
    task->number = -1;
    task->next = NULL;
    task->previous = NULL;
    task->startTime = 0;
    task->timer = NULL;
    task->type = NoType;

    vPortFree((void*)task);
    return true;
}

/*
 * Initialize a linked list for a deadline-driven task list
 */
void Init_DD_TaskList(ddListHandle list) {
	// Confirm valid pointer
	if(list == NULL) return;

    list->length = 0;
    list->head = NULL;
    list->tail = NULL;
}

/*
 * Insert a deadline-driven task struct into the overdue or active list
 */
void Insert_DD_Task(ddTaskHandle task , ddListHandle list) {
	// Check input parameters are not NULL
	if(list == NULL || task == NULL) return;

	// First task in the list, insert as the head and the tail
    if( list->length == 0 ) {
        list->head = task;
        list->tail = task;
        list->length = 1;
        vTaskPrioritySet(task->handle, BASE_DD_PRIORITY);
        return;
    }

    ddTaskHandle curTask = list->head;
    uint32_t curPriority = uxTaskPriorityGet(curTask->handle);
    curPriority += 1;

    if(curPriority == GENERATOR_DD_PRIORITY) return; // FreeRTOS scheduler is full

    if(task->type == Aperiodic) {
    	uint32_t tailPriority = uxTaskPriorityGet(list->tail->handle);
    	vTaskPrioritySet(task->handle, tailPriority);
    	list->tail->next = task;
    	list->tail = task;
    	list->length += 1;
    	while(curTask->next != NULL) {
    		vTaskPrioritySet(curTask->handle, curPriority);
    		curPriority -= 1;
    		curTask = curTask->next;
    	}
    	return;
    }

    // Iterate through the list starting at the head
    while(curTask != NULL){
    	bool equalButSmallerTaskNumber = (task->startTime == curTask->startTime) && (task->deadline == curTask->deadline) && (task->number < curTask->number);

        if(task->deadline < curTask->deadline || curTask->type == Aperiodic || equalButSmallerTaskNumber) {
        	// Found the location in the list for the task, insert and return
            if(curTask == list->head){
            	list->head = task;
            }

            task->next = curTask;
            task->previous = curTask->previous;
            curTask->previous = task;

            list->length += 1;
            vTaskPrioritySet(task->handle, curPriority);
            return;
        } else {
            if(curTask->next == NULL) {
            	// At the end of the list, insert the task as the tail and return
                list->tail = task;
                task->previous = curTask;
                task->next = NULL;
                curTask->next = task;

                list->length += 1;
                vTaskPrioritySet(curTask->handle, curPriority);
                vTaskPrioritySet(task->handle, BASE_DD_PRIORITY);
                return;
            }

            // Iterate to the next task in the list
            vTaskPrioritySet(curTask->handle, curPriority);
            curTask = curTask->next;
            curPriority -= 1;
        }
    }
}


/*
 * Remove a deadline-driven task from a task list
 */
void Remove_DD_TaskList(TaskHandle_t task, ddListHandle list, bool transfer, bool trim) {
	// Catch bad inputs
	if((task == NULL && !trim) || list == NULL || list->length == 0) return;

	ddTaskHandle curTask = list->head;
    uint32_t curPriority = uxTaskPriorityGet(curTask->handle);

    // If the list has only one element that is the task to remove, zero-out the list
    if(list->length == 1 && (curTask->handle == task || trim)) {
		list->length = 0;
    	list->head = NULL;
		list->tail = NULL;
		if(!transfer) Free_DD_Task(curTask);
		return;
	}

    // If we're trimming down the size of the overdue list, just remove the head
    if(trim) {
    	list->head = curTask->next;
    	curTask->next->previous = NULL;
    	(list->length)--;
    	curTask->previous = NULL;
    	curTask->next = NULL;
    	Free_DD_Task(curTask);
    	return;
    }

    while(curTask != NULL) {
        if(curTask->handle == task) {

            // Remove the task from the list (specific cases for head and tail)
            if(list->tail->handle == task) {
                list->head = curTask->previous;
                curTask->previous->next = NULL;
            } else if(list->head->handle == task) {
            	list->head = curTask->next;
            	curTask->next->previous = NULL;
            } else {
                curTask->previous->next = curTask->next;
                curTask->next->previous = curTask->previous;
            }

            // Decrement the list size and free the task itself
            list->length -= 1;
            if(!transfer) Free_DD_Task(curTask);
            return;
        } else {
        	// Iterate to the next task in the list
			curPriority--;
			vTaskPrioritySet(curTask->handle, curPriority);
			curTask = curTask->next;
        }
    }

    // Reset all priorities if task not in list
    curTask = list->tail;
    curPriority = BASE_DD_PRIORITY;
    while(curTask != NULL) {
        vTaskPrioritySet(curTask->handle, curPriority);
        curTask = curTask->previous;
        curPriority++;
    }
}

/*
 * Adds a task to the overdue list
 */
void Add_DD_Overdue_TaskList(ddListHandle overdueList, ddTaskHandle curTask) {
	// Overdue list is empty
	if(overdueList->length == 0) {
		overdueList->length = 1;
		overdueList->head = curTask;
		overdueList->tail = curTask;
	} else {
		// Otherwise, add the task to the tail of the overdue list
		ddTaskHandle prevTail = overdueList->tail;
		overdueList->tail = curTask;
		prevTail->next = curTask;
		curTask->previous = prevTail;
		overdueList->length += 1;
	}

	// Stop its execution
	vTaskSuspend(curTask->handle);
	vTaskDelete(curTask->handle);
}

/*
 * Transfers a task from the active to the overdue list
 */
void Transfer_DD_TaskList(ddListHandle activeList, ddListHandle overdueList) {
	// Confirm valid list
	if(activeList == NULL || overdueList == NULL) return;

	ddTaskHandle curTask = activeList->head;
    TickType_t curTicks = xTaskGetTickCount();

    // Iterate through the list and remove overdue tasks from the active list and add to the overdue list
    while(curTask != NULL) {
        if(curTicks > 0 && curTask->deadline < (curTicks)) {
        	Remove_DD_TaskList(curTask->handle, activeList, true, false);
        	curTask->next = NULL;
        	Add_DD_Overdue_TaskList(overdueList, curTask);
        }
        curTask = curTask->next;
    }
}



/*
 * Generates and returns a formatted string of the contents of the input list
 */
char* Get_DD_TaskList(ddListHandle list) {
	// Malloc and reset the space for the string
    uint32_t lenList= list->length;
	char* outputString = (char*)pvPortMalloc(((configMAX_TASK_NAME_LEN + 50) * (lenList + 1)));
	outputString[0] = '\0';

    if(lenList == 0) {
    	char emptyString[21] = ("Nothing in list.");
    	strcat(outputString, emptyString);
    } else {
    	// Starting from the head, iterate through the list and append the formatted data to the outputString
    	ddTaskHandle curTask = list->head;
		while(curTask != NULL) {
			char curString[60];
			uint32_t deadline = (unsigned int) curTask->deadline;
			sprintf(curString, "Task: %s with deadline: %u \n", curTask->name, deadline);
			strcat(outputString, curString);
			curTask = curTask->next;
		}
    }
    return outputString;
}


