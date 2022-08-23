#ifndef __TIMERFUNCTIONSH
#define __TIMERFUNCTIONSH

#include <stdint.h>
#include "stm32f4xx_hal.h"

#define TIMEREK_SZAMA         20

typedef struct myTimerTypeh
{
	uint16_t number;
	uint16_t set_value;
	uint16_t cur_value;
	uint8_t Elapsed;
	uint8_t Enabled;
	void *ownerPtr;
	void (*Callback)(void *callBackPtr);

}myTimerType;

static unsigned int tidx ;
static myTimerType *timer[TIMEREK_SZAMA+1];

void initTimer(myTimerType *Timer_ptr);
void TimerProgramCyclic(TIM_HandleTypeDef *htim);
void setTimer(myTimerType *Timer_ptr);
unsigned int seeTimer(unsigned char index);
//unsigned char timerElapsed(unsigned char index);
void TimerekSetup(TIM_HandleTypeDef *htim);

#endif

