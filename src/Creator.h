#ifndef CREATOR_H_
#define CREATOR_H_

#include <CommonConfig.h>
#include <Scheduler.h>

TaskHandle_t PeriodicTaskCreatorHandle1;
TaskHandle_t PeriodicTaskCreatorHandle2;
TaskHandle_t PeriodicTaskCreatorHandle3;
TaskHandle_t AperiodicTaskCreatorHandle;

void PeriodicTask1(void *pvParameters);
void PeriodicTaskCreator1(void *pvParameters);
void PeriodicTask2(void *pvParameters);
void PeriodicTaskCreator2(void *pvParameters);
void PeriodicTask3(void *pvParameters);
void PeriodicTaskCreator3(void *pvParameters);

void AperiodicTask(void *pvParameters);
void AperiodicTaskCreator(void *pvParameters);


// Test Bench 1
#define periodicTask1Period 		(500)
#define periodicTask1Duration   	(95)
#define periodicTask2Period 		(500)
#define periodicTask2Duration   	(150)
#define periodicTask3Period 		(750)
#define periodicTask3Duration   	(250)
#define aperiodicTaskDuration 		(150)
#define aperiodicTaskDeadline 		(1500)

/*
// Test Bench 2
#define periodicTask1Period 		(250)
#define periodicTask1Duration   	(95)
#define periodicTask2Period 		(500)
#define periodicTask2Duration   	(150)
#define periodicTask3Period 		(750)
#define periodicTask3Duration   	(250)
#define aperiodicTaskDuration 		(0)
#define aperiodicTaskDeadline 		(1500)
*/
/*
// Test Bench 3
#define periodicTask1Period 		(500)
#define periodicTask1Duration   	(100)
#define periodicTask2Period 		(500)
#define periodicTask2Duration   	(200)
#define periodicTask3Period 		(500)
#define periodicTask3Duration   	(200)
#define aperiodicTaskDuration 		(0)
#define aperiodicTaskDeadline 		(1500)
*/

#endif
