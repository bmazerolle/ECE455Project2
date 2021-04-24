#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include <CommonConfig.h>
#include <List.h>

void DD_Scheduler( void *pvParameters );
void DD_Scheduler_Init( void );
void Create_DD_Task(ddTaskHandle task);
void Delete_DD_Task(TaskHandle_t task, char number);
void Monitor(void *pvParameters);
void Get_Active_DD_TaskList(uint32_t totalDelay);
void Get_Overdue_DD_TaskList(uint32_t totalDelay);

#endif
