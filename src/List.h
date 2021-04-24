#ifndef LIST_H_
#define LIST_H_

#include <CommonConfig.h>

bool Free_DD_Task(ddTaskHandle task);
char* Get_DD_TaskList(ddListHandle list);
ddTaskHandle Init_DD_Task();
void Add_DD_Overdue_TaskList(ddListHandle overdueList, ddTaskHandle curTask);
void Init_DD_TaskList(ddListHandle list);
void Insert_DD_Task(ddTaskHandle task , ddListHandle list);
void Remove_DD_TaskList(TaskHandle_t task, ddListHandle list, bool transfer, bool trim);
void Transfer_DD_TaskList(ddListHandle activeList, ddListHandle overdueList);

#endif
