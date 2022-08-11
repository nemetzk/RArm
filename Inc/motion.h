/*
 * motion.h
 *
 *  Created on: Jan 20, 2022
 *      Author: nemet
 */

#ifndef INC_MOTION_H_
#define INC_MOTION_H_


#include "sbus_fnc.h"
#include "servoNoEnc.h"
#include "rc_configuration.h"
#include "ServoMFP.h"

#define STAT_LED_1_BLINK_NOENC_INTERVAL 50

typedef struct subCycleth
{
	uint8_t state;
}subCyclet;

typedef struct motionth
{
	uint8_t taut_state;
	struct sbusth sbus;
	struct servoFBth servoA;
	struct servoFBth servoB;
	struct servoFBth servoC;
	struct servoNeth servoD;
	struct subCycleth opManu;
	struct subCycleth opSemi;
	struct subCycleth opAut;

	TIM_HandleTypeDef *encTim1;
	TIM_HandleTypeDef *encTim2;
	TIM_HandleTypeDef *pwmTim1;
	uint32_t *pwmCh1;
	TIM_HandleTypeDef *pwmTim2;
	uint32_t *pwmCh2;
	TIM_HandleTypeDef *pwmTim3;
	uint32_t *pwmCh3;
}motiont;

int8_t moveServoA(motiont *motionb,int32_t position);
int8_t moveServoB(motiont *motionb,int32_t position);
int8_t moveServoC(motiont *motionb,int32_t position);

/*
 *
 * before changing the servo routines it was:
 *
 *
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
*/
#endif /* INC_MOTION_H_ */
