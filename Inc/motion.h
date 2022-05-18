/*
 * motion.h
 *
 *  Created on: Jan 20, 2022
 *      Author: nemet
 */

#ifndef INC_MOTION_H_
#define INC_MOTION_H_


#include "sbus_fnc.h"
#include "servoFB.h"
#include "servoNoEnc.h"
#include "rc_configuration.h"

typedef struct motionth
{
	uint8_t taut_state;
	struct sbusth sbus;
	struct servoFBth servoA;
	struct servoFBth servoB;
	struct servoNeth servoC;
	struct servoNeth servoD;

	TIM_HandleTypeDef *encTim1;
	TIM_HandleTypeDef *encTim2;
	TIM_HandleTypeDef *pwmTim1;
	uint32_t *pwmCh1;
	TIM_HandleTypeDef *pwmTim2;
	uint32_t *pwmCh2;
	TIM_HandleTypeDef *pwmTim3;
	uint32_t *pwmCh3;
}motiont;



#endif /* INC_MOTION_H_ */
