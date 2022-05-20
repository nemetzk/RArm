/*
 * encoder.c
 *
 *  Created on: Jan 11, 2022
 *      Author: nemet
 */

#include "encoder.h"
#include "ServoFB.h"
#include "header.h"

#include "stm32f4xx_hal.h"

	TIM_HandleTypeDef *enc_htim;

#define NO_ENCODERS 		3  				 								//encoder array -hez
#define	ENCODER_ZERO_VAL	32767

	Encoder *encoders[NO_ENCODERS+1]; 	 								//encoder array -hez
uint8_t noEncoders = 0;				 								//encoder array -hez
/*
void spdMeas(Encoder *hencoder)
{
	hencoder->speed = (hencoder->val - hencoder->spd_preval)/10;
	hencoder->spd_preval = hencoder->val;

	setTimer(&hencoder->encoderMeasTim);
}
*/
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

}
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
		debug_absval = abs(hencoder->readVal - hencoder->preReadval);

		if ( debug_absval > 1 )
		{
			hencoder->speed =(hencoder->readVal - hencoder->preReadval);
			hencoder->preReadval = hencoder->readVal;
			ServoInitProcess(hencoder->ownerPtr);

		}

		if (hencoder->pulseInitContinue)
			hencoder->state = EA_IDLE;

	break;

	case EA_IDLE: //NYUGALMI ÁLLAPOTBAN IDŐ UTÁN NINCS ÚJABB IMPULZUS
		hencoder->readVal =  __HAL_TIM_GET_COUNTER(hencoder->enctim);
		if ((abs(hencoder->readVal - hencoder->preReadval)>1))
				{
					calculateReversedValue(hencoder);
					hencoder->speed = ((hencoder->val - hencoder->pre_val));
					hencoder->pre_val = hencoder->val;
					if (hencoder->speed > 0)
						hencoder->state = EA_MEASURE_PLS_POS;
					if (hencoder->speed < 0)
						hencoder->state = EA_MEASURE_PLS_NEG;
				}
	break;

	case EA_MEASURE_PLS_POS: //ÉPPEN POZITÍV ENCODER IRÁNYÚ IMPULZUSOK JÖNNEK
		hencoder->readVal =  __HAL_TIM_GET_COUNTER(hencoder->enctim);
		if ((abs(hencoder->readVal - hencoder->preReadval)>1))
				{
					calculateReversedValue(hencoder);
					hencoder->speed = ((hencoder->val - hencoder->pre_val));
					hencoder->pre_val = hencoder->val;
					if (hencoder->speed > 0)
						hencoder->state = EA_MEASURE_PLS_POS;
					if (hencoder->speed < 0)
						hencoder->state = EA_MEASURE_PLS_NEG;
				}
		else if ((hencoder->val == hencoder->pre_val))
		{
			hencoder->speed = 0;
			hencoder->state = EA_IDLE;
		}
	break;

	case EA_MEASURE_PLS_NEG: //ÉPPEN NEGATÍV ENCODER IRÁNYÚ IMPULZUSOK JÖNNEK
		hencoder->readVal =  __HAL_TIM_GET_COUNTER(hencoder->enctim);
		if ((abs(hencoder->readVal - hencoder->preReadval)>1))
				{
					calculateReversedValue(hencoder);
					hencoder->speed = (int16_t)((hencoder->val - hencoder->pre_val));
					hencoder->pre_val = hencoder->val;
					if (hencoder->speed > 0) hencoder->state = EA_MEASURE_PLS_POS;
					if (hencoder->speed < 0) hencoder->state = EA_MEASURE_PLS_NEG;
				}
		else if ((hencoder->val == hencoder->pre_val))
		{
			hencoder->speed = 0;
			hencoder->state = EA_IDLE;
		}
	break;
	} //switch


	setTimer(&hencoder->encoderMeasTim);
	hencoder->preReadval = hencoder->readVal;
	hencoder->pre_val = hencoder->val;
}

void encoder_init(Encoder *hencoder, TIM_HandleTypeDef *htim)
{
	hencoder->number=++noEncoders; 									//encoder array -hez
	hencoder->timNum = htim->Instance;
	hencoder->enctim = htim;

	encoders[noEncoders]=hencoder; 									//encoder array -hez
	htim->Instance->CNT = ENCODER_ZERO_VAL;

	initTimer(&hencoder->encoderTim);

	hencoder->state = EA_INIT;
	hencoder->encoderMeasTim.set_value = 100;
	hencoder->encoderMeasTim.Callback = encoderAutomata_cycl; // korábban spdMeas;
	hencoder->encoderMeasTim.ownerPtr = hencoder;

	initTimer(&hencoder->encoderMeasTim);
	setTimer(&hencoder->encoderMeasTim);
	HAL_TIM_Encoder_Start_IT(hencoder->enctim, TIM_CHANNEL_ALL);

}
/*
void HAL_TIM_IC_CaptureCallback (TIM_HandleTypeDef *htim)
{
	uint8_t eidx = 0;
	for (eidx=0;eidx<=noEncoders;eidx++)							 //encoder array -hez
	{
		//if (htim->Instance == encoders[eidx]->timNum)
		if (htim == encoders[eidx]->enctim)
		{
			encoders[eidx]->readVal =  __HAL_TIM_GET_COUNTER(htim);

			if (encoders[eidx]->pre_val!=encoders[eidx]->val)
			{
				setTimer(&(encoders[eidx]->encoderTim));
			}

				encoderAutomata_cycl(encoders[eidx]);

				if (encoders[eidx]->Callback != NULL)
				{
					encoders[eidx]->Callback(encoders[eidx]->ownerPtr);
				}
				//encoders[eidx]->preReadval = encoders[eidx]->readVal;
			}
	}

}
*/
