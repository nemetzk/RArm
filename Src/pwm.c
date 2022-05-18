/*
 * pwm.c
 *
 *  Created on: Jan 19, 2022
 *      Author: nemet
 */
#include "pwm.h"

void pwm_init(struct pwmchth *pwmch)
{
	 //pwmch->setval = pulsewdt;
			  HAL_TIM_PWM_Stop(pwmch->htim, pwmch->ch);

			  TIM_OC_InitTypeDef sConfigOC = {0};
			  sConfigOC.OCMode = TIM_OCMODE_PWM1;
			  sConfigOC.Pulse = 1500;
			  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
			  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
			  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
			  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
			  if (HAL_TIM_PWM_ConfigChannel(pwmch->htim, &sConfigOC, pwmch->ch) != HAL_OK)
			  {
			    Error_Handler();
			  }
			  //HAL_TIM_PWM_Start(pwmch->htim, pwmch->ch);
	//set_MOTOR_STOP(pwmch);
}

	void pwmSet(pwmcht *pwmch, uint32_t pulsewdt)
	{
		__HAL_TIM_SET_COMPARE(pwmch->htim, pwmch->ch, pulsewdt);
		/*
		  pwmch->setval = pulsewdt;
		  HAL_TIM_PWM_Stop(pwmch->htim, pwmch->ch);

		  TIM_OC_InitTypeDef sConfigOC = {0};
		  sConfigOC.OCMode = TIM_OCMODE_PWM1;
		  sConfigOC.Pulse = pulsewdt;
		  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
		  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
		  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
		  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
		  if (HAL_TIM_PWM_ConfigChannel(pwmch->htim, &sConfigOC, pwmch->ch) != HAL_OK)
		  {
		    Error_Handler();
		  }
		  HAL_TIM_PWM_Start(pwmch->htim, pwmch->ch);
		  */
	}

	void set_MOTOR_CW(pwmcht *pwmch)
	{
		pwmSet(pwmch,1300);
		if (TIM_CHANNEL_STATE_GET(pwmch->htim, pwmch->ch) == HAL_TIM_CHANNEL_STATE_READY)
			HAL_TIM_PWM_Start(pwmch->htim, pwmch->ch);
	}

	void set_MOTOR_CCW(pwmcht *pwmch)
	{
		pwmSet(pwmch,500);
		if (TIM_CHANNEL_STATE_GET(pwmch->htim, pwmch->ch) == HAL_TIM_CHANNEL_STATE_READY)
			HAL_TIM_PWM_Start(pwmch->htim, pwmch->ch);
	}

	void set_MOTOR_STOP(pwmcht *pwmch)
	{
		pwmSet(pwmch,1500);
		HAL_TIM_PWM_Stop(pwmch->htim, pwmch->ch);
	}
