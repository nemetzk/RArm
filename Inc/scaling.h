#ifndef SCALING_H_
#define SCALING_H_

#include "stm32f4xx_hal.h"
#include "math.h"
#include "timerFunctions.h"

typedef struct scaledNumItemth
{
	int32_t value;
	int32_t min;
	int32_t max;
}scaledNumItemt;

typedef struct scaleHelpersth
{
	int32_t szamlalo;
	int32_t nevezo;
	double tenyezo;
}scaleHelperst;


typedef struct scaledNumth
{
	uint8_t number;
	//void *ownerPtr;
	scaledNumItemt ScaledOutput;
	scaledNumItemt RawInput;
	scaleHelperst helpers;
	uint8_t calculationError;
}scaledNumt;

#define NO_SCALE_INPUTS 		10
#define NO_SCALE_OUTPUTS 		10

#endif
