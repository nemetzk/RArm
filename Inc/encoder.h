#pragma once

#include <stdint.h>
#include "timerFunctions.h"
#include "stm32F4xx_hal.h"

static uint32_t Period = 65535;

//https://www.youtube.com/watch?v=MWdqny4OBvQ
#define EA_INIT				0
#define EA_WF_FIRST_PULSE	99
#define EA_IDLE				1
#define EA_MEASURE_PLS_POS	10
#define EA_MEASURE_PLS_NEG	20



typedef struct Encoderh{
	uint8_t number;
	int32_t timNum;
	int32_t val;
	int32_t readVal;
	int32_t preReadval;
	int32_t pre_val;
	int32_t spd_preval;
	int8_t state;		// ha +1 CW; ha -1 CCW; ha 0 nem mozog
	int8_t reverseEncoderValue;
	uint8_t pulseInitContinue;
	uint8_t pulseInitPassed;
	int16_t speed;
	TIM_HandleTypeDef *enctim;
	myTimerType encoderTim;
	myTimerType encoderMeasTim;
	void *ownerPtr;
	void (*Callback)(void *callBackPtr);
}Encoder;


	//void encoderTimEA(void);
	//void encoder2TimEA(void);

