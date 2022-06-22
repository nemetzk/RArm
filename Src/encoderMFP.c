/*
 * encoderMFP.c
 *
 *  Created on: 2022. jún. 17.
 *      Author: nemet
 */
#include "encoderMFP.h"
#include "stm32f4xx_hal.h"

	TIM_HandleTypeDef *enc_htim;

#define NO_ENCODERS 		3  				 								//encoder array -hez
#define	ENCODER_ZERO_VAL	32767

	Encoder *encoders[NO_ENCODERS+1]; 	 								//encoder array -hez
uint8_t noEncoders = 0;

void encoderAutomata_cycl(Encoder *hencoder) //Ez váltja fel a korábbi spdMeas fv-t
{

	switch (hencoder->state)
	{

	case EA_INIT:
		hencoder->speed = 0;
		hencoder->preReadval = ENCODER_ZERO_VAL;
		hencoder->readVal = ENCODER_ZERO_VAL;
		hencoder->val = ENCODER_ZERO_VAL;
		hencoder->state = EA_WF_FIRST_PULSE;
	break;

	case EA_WF_FIRST_PULSE: //VÁRJA INITKOR AZ ELSŐ IMPULZUST
		hencoder->readVal =  __HAL_TIM_GET_COUNTER(hencoder->enctim);
		hencoder->diff_10ms = hencoder->readVal - hencoder->preReadval;
		if ( abs(hencoder->diff_10ms) > 1 ) //HALL SENSOR FILTER
		{
			hencoder->preReadval = hencoder->readVal;
			hencoder->state = EA_WORKING;
		}



	break;

	case EA_WORKING: //NYUGALMI ÁLLAPOTBAN IDŐ UTÁN NINCS ÚJABB IMPULZUS
		hencoder->readVal =  __HAL_TIM_GET_COUNTER(hencoder->enctim);
		hencoder->diff_10ms = hencoder->readVal - hencoder->preReadval;
		if ( abs(hencoder->diff_10ms) > 1 ) //HALL SENSOR FILTER
		{
			calculateReversedValue(hencoder);
			if (++(hencoder->count_100ms) >= 10) //100ms-letelt
			{

				hencoder->diff_100ms =hencoder->val - hencoder->preval_100ms;
				hencoder->preval_100ms = hencoder->val;
				averageCalcCycl(&hencoder->encAVG, hencoder->diff_100ms);
				hencoder->speed = hencoder->encAVG.averageVal;
				hencoder->count_100ms = 0;
			}
			hencoder->preReadval = hencoder->readVal;
			//setTimer(&(hencoder->encoderTim));
		}
	break;

	} //switch
	setTimer(&hencoder->encoderMeasTim);
} //function

void encoder_init(Encoder *hencoder, TIM_HandleTypeDef *htim)
{
	hencoder->number=++noEncoders; 									//encoder array -hez
	hencoder->timNum = htim->Instance;
	hencoder->enctim = htim;

	encoders[noEncoders]=hencoder; 									//encoder array -hez
	htim->Instance->CNT = ENCODER_ZERO_VAL;
	//initTimer(&hencoder->encoderTim);
	hencoder->state = EA_INIT;

	hencoder->encAVG.stackCapacity = 5;

	hencoder->encoderMeasTim.set_value = 50;
	hencoder->encoderMeasTim.Callback = encoderAutomata_cycl; // korábban spdMeas;
	hencoder->encoderMeasTim.ownerPtr = hencoder;
	initTimer(&hencoder->encoderMeasTim);
	setTimer(&hencoder->encoderMeasTim);

	HAL_TIM_Encoder_Start_IT(hencoder->enctim, TIM_CHANNEL_ALL);
} //function

void calculateReversedValue(Encoder *hencoder)
{
	if (hencoder->reverseEncoderValue)
	{
		hencoder->val = (2*ENCODER_ZERO_VAL)-hencoder->readVal;
	}
	else
	{
		hencoder->val = hencoder->readVal;
	}
}  //function
