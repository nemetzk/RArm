/*
 * pwm.h
 *
 *  Created on: Jan 19, 2022
 *      Author: nemet
 */

#ifndef INC_PWM_H_
#define INC_PWM_H_
#include "stm32f4xx_hal.h"


//TIM_HandleTypeDef *htim, uint32_t Channel
typedef struct pwmchth
{
	TIM_HandleTypeDef *htim;
	uint32_t *ch;
	uint32_t setval;
}pwmcht;

void pwm_init(pwmcht *pwmch);
void pwmSet(pwmcht *pwmch, uint32_t pulsewdt);
void set_MOTOR_CW(pwmcht *pwmch);
void set_MOTOR_CCW(pwmcht *pwmch);
void set_MOTOR_STOP(pwmcht *pwmch);

#endif /* INC_PWM_H_ */
