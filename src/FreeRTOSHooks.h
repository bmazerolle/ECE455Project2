#ifndef FREERTOSHOOKS_H_
#define FREERTOSHOOKS_H_

#include <CommonConfig.h>


void vApplicationMallocFailedHook( void );
void vApplicationStackOverflowHook( xTaskHandle pxTask, signed char *pcTaskName );
void vApplicationIdleHook( void );


#endif /* FREERTOSHOOKS_H_ */
