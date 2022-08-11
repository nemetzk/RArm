/*
 * scaling.c
 *
 *  Created on: 2022. máj. 24.
 *      Author: nemet
 */
#include "scaling.h"

scaledNumt *scaleInputs[NO_SCALE_INPUTS+1]; 	 								//encoder array -hez
uint8_t noScales = 0;

void initScaledVal(struct scaledNumth *sg)
{
	sg->number=++noScales;
	scaleInputs[noScales] = sg;
	initCalcScaling(sg);
}

void initCalcScaling(struct scaledNumth *sg)
{
	  sg->helpers.szamlalo 	= ( sg->ScaledOutput.max - sg->ScaledOutput.min);
	  sg->helpers.nevezo 	= sg->RawInput.max - sg->RawInput.min;
	  if (sg->helpers.nevezo != 0)
	  {
		  sg->helpers.tenyezo =((double) (sg->helpers.szamlalo) / (double) (sg->helpers.nevezo));
		  sg->calculationError = 0x00;
	  }
	  else
	  {
		  sg->calculationError = 0xff;
	  }
}

void calcScaledVal(struct scaledNumth *sg)
{

		sg->ScaledOutput.value = (int32_t)(((double)sg->RawInput.value - (double)sg->RawInput.min) * (double)sg->helpers.tenyezo + (double)sg->ScaledOutput.min);
}


