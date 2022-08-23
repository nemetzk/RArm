
#include "main.h"
#include "timerFunctions.h"

static unsigned char notimers = 0;
TIM_HandleTypeDef *myhtim;

void TimerProgramCyclic(TIM_HandleTypeDef *htim)
{
	for ( tidx = 0 ; tidx <= notimers ; tidx++ )
		{
		if (timer[tidx]->Enabled){
			if (timer[tidx]->cur_value == (timer[tidx]->set_value -1))
			{
				if (timer[tidx]->Callback != NULL)
					timer[tidx]->Callback(timer[tidx]->ownerPtr);
				timer[tidx]->cur_value++ ;
			}
			else if ((timer[tidx]->cur_value) < (timer[tidx]->set_value))
			{
				timer[tidx]->cur_value++ ;
				timer[tidx]->Elapsed = 0x00;
			}
			else
			{
				timer[tidx]->Elapsed = 0xff;
				timer[tidx]->Enabled = 0x00;
			}
		 }
		} //for
} //void

void setTimer(myTimerType *Timer_ptr)
{
	//Timer_ptr->cur_value=Timer_ptr->set_value;
	Timer_ptr->cur_value=0;
	Timer_ptr->Enabled = 0xff;
}


unsigned int seeTimer(unsigned char index) {
	uint16_t xvar;
	xvar=timer[index]->cur_value;;
	return xvar;
}

/*
unsigned char timerElapsed(myTimerType *Timer_ptr) {
	return Timer_ptr->cur_value==0;
}
*/

void TimerekSetup(TIM_HandleTypeDef *htim){
//	unsigned char timer_setup_idx;
	HAL_TIM_Base_Start_IT(htim);
	myhtim=htim;
}


void initTimer(myTimerType *Timer_ptr) //void initTimer(myTimerType *Timer_ptr, void(*callBackFun)(void))
{
	/*
	 * kívül ezeket kell hozzárendelni - így kell deklarálni

	*/
	Timer_ptr->cur_value=0;
	Timer_ptr->number=++notimers;
	timer[notimers]=Timer_ptr;
	//seeTimer(1);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if(htim->Instance == TIM10)
	{
		//TimerProgramCyclic(htim);
		//HAL_GPIO_TogglePin(TEST_PIN_GPIO_Port, TEST_PIN_Pin);

	}
	TimerProgramCyclic(htim);
	/*
	if(htim->Instance == TIM4) //1ms
	{

	}
*/
}


