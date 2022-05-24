/*
 * scaling.c
 *
 *  Created on: 2022. máj. 24.
 *      Author: nemet
 */
#include "scaling.h"

void initCalcScaling(struct scaledNumth *sg)
{
	  sg->helpers.szamlalo 	= ( sg->ScaledOutput.max - sg->ScaledOutput.min);
	  sg->helpers.nevezo 	= sg->RawInput.max - sg->RawInput.min;
	  if (sg->helpers.nevezo != 0)
	  {
		  sg->helpers.tenyezo = (int32_t)(sg->helpers.szamlalo / sg->helpers.nevezo);
		  sg->calculationError = 0x00;
	  }
	  else
	  {
		  sg->calculationError = 0xff;
	  }
	  sg->initDone = 1;
}

void calcScaledVal(struct scaledNumth *sg)
{
	if (!sg->initDone)
	{

		initCalcScaling(sg);
	}

	if (sg->calculationEnabled)
	{
		sg->ScaledOutput.value = (int32_t)((sg->RawInput.value - sg->RawInput.min) * sg->helpers.tenyezo + sg->ScaledOutput.min);
    }
}
