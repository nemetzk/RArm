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
	int32_t tenyezo;
}scaleHelperst;



typedef struct scaledNumth
{
	uint8_t calculationEnabled;
	scaledNumItemt ScaledOutput;
	scaledNumItemt RawInput;
	scaleHelperst helpers;
	uint8_t calculationError;
	uint8_t initDone;
}scaledNumt;

#endif
