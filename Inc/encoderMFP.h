/*
 * encoderMFP.h
 *
 *  Created on: 2022. jún. 17.
 *      Author: nemet
 */

#ifndef INC_ENCODERMFP_H_
#define INC_ENCODERMFP_H_


#pragma once

#include <stdint.h>
#include "timerFunctions.h"
#include "stm32F4xx_hal.h"
#include "averageCalc.h"

static uint32_t Period = 65535;

//https://www.youtube.com/watch?v=MWdqny4OBvQ
#define EA_INIT				0
#define EA_WF_FIRST_PULSE	99
#define EA_WORKING			 1


//int32_t debug_absval;

typedef struct Encoderh{
	uint8_t number;
	int32_t timNum;
	int32_t val;
	int32_t readVal;
	int32_t preReadval;
	int32_t diff_10ms;
	int32_t diff_100ms;
	int32_t preval_100ms;
	uint8_t count_100ms;

	int8_t state;		// ha +1 CW; ha -1 CCW; ha 0 nem mozog
	int8_t reverseEncoderValue;
	uint8_t pulseInitContinue;
	uint8_t pulseInitPassed;
	int32_t speed;
	TIM_HandleTypeDef *enctim;
	myTimerType encoderTim;
	myTimerType encoderMeasTim;
	myTimerType encoderCyclTim;
	uint8_t timeToMeasSpd;
	void *ownerPtr;
	void (*Callback)(void *callBackPtr);
	averageCalct encAVG;
}Encoder;


void encoderAutomata_cycl(Encoder *hencoder);
void encoder_init(Encoder *hencoder, TIM_HandleTypeDef *htim);
void calculateReversedValue(Encoder *hencoder);

#endif /* INC_ENCODERMFP_H_ */
