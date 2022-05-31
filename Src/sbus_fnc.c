/*
 * sbus_fnc.c
 *
 *  Created on: Jan 10, 2022
 *      Author: nemet
 */

#include "sbus_fnc.h"
#include <stdint.h>
#include "header.h"
#include "timerFunctions.h"

void sbusCommTimeOver(struct sbusth *hsbus)
{
	hsbus->sbusHealth.sbusTimeOut = 1;
	HAL_UART_Receive_IT(hsbus->sbusUart, &(hsbus->func.oneCharBuffer), 1);
	setTimer(&(hsbus->func.sbusTimer));
}

void SBUS_init(struct sbusth *hsbus)
{
	uint8_t i;
	hsbus->number=++no_sbuses;
	sbuses[no_sbuses]=hsbus;

	HAL_UART_Receive_IT(hsbus->sbusUart, &(hsbus->func.oneCharBuffer), 1);

	hsbus->func.sbusTimer.set_value = 100;
	hsbus->func.sbusTimer.Callback = sbusCommTimeOver;
	hsbus->func.sbusTimer.ownerPtr = hsbus;

	initTimer(&(hsbus->func.sbusTimer));
	setTimer(&(hsbus->func.sbusTimer));

	for (i=0;i<17;i++)
	{
		hsbus->sbusCh[i].digVal.Aval = 700;
		hsbus->sbusCh[i].digVal.Bval = 1500;
		hsbus->sbusCh[i].rawVal.min = 140;
		hsbus->sbusCh[i].rawVal.max = 1728;
		hsbus->sbusCh[i].scaledVal.min = 0;
		hsbus->sbusCh[i].scaledVal.max = 100;

	}

}



void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	uint8_t eidx;
	struct sbusth *hsbus;

	//if (huart->Instance == USART1)

	for (eidx=1;eidx<=no_sbuses;eidx++)
		if ( huart == sbuses[eidx]->sbusUart)
		  {

			if (sbuses[eidx]->func.sbusTimer.cur_value > 15)
			{
				parseSbusMessage(sbuses[eidx],sbuses[eidx]->func.SBUS_rxBuffer);
				sbuses[eidx]->func.no_char =0;
			}
			else
			{
				sbuses[eidx]->func.no_char++;
			}
			sbuses[eidx]->func.SBUS_rxBuffer[sbuses[eidx]->func.no_char]=sbuses[eidx]->func.oneCharBuffer;
			HAL_UART_Receive_IT(huart, &(sbuses[eidx]->func.oneCharBuffer), 1);
			setTimer(&(sbuses[eidx]->func.sbusTimer));
		  }
}

void parseSbusMessage(struct sbusth *hsbus, uint8_t *sbus_msg_bytes)
{
	uint8_t i;
	int32_t szamlalo;
	int16_t nevezo;
	  // Decode the 16 regular channels



	hsbus->sbusCh[1].rawVal.value  =
	      (((uint16_t)sbus_msg_bytes[1]) | ((uint16_t)sbus_msg_bytes[2] << 8)) &
	      0x07FF;
	hsbus->sbusCh[2].rawVal.value = (((uint16_t)sbus_msg_bytes[2] >> 3) |
	                          ((uint16_t)sbus_msg_bytes[3] << 5)) &
	                         0x07FF;
	hsbus->sbusCh[3].rawVal.value =
	      (((uint16_t)sbus_msg_bytes[3] >> 6) | ((uint16_t)sbus_msg_bytes[4] << 2) |
	       ((uint16_t)sbus_msg_bytes[5] << 10)) &
	      0x07FF;
	hsbus->sbusCh[4].rawVal.value = (((uint16_t)sbus_msg_bytes[5] >> 1) |
	                          ((uint16_t)sbus_msg_bytes[6] << 7)) &
	                         0x07FF;
	hsbus->sbusCh[5].rawVal.value = (((uint16_t)sbus_msg_bytes[6] >> 4) |
	                          ((uint16_t)sbus_msg_bytes[7] << 4)) &
	                         0x07FF;
	hsbus->sbusCh[6].rawVal.value =
	      (((uint16_t)sbus_msg_bytes[7] >> 7) | ((uint16_t)sbus_msg_bytes[8] << 1) |
	       ((uint16_t)sbus_msg_bytes[9] << 9)) &
	      0x07FF;
	hsbus->sbusCh[7].rawVal.value = (((uint16_t)sbus_msg_bytes[9] >> 2) |
	                          ((uint16_t)sbus_msg_bytes[10] << 6)) &
	                         0x07FF;
	hsbus->sbusCh[8].rawVal.value = (((uint16_t)sbus_msg_bytes[10] >> 5) |
	                          ((uint16_t)sbus_msg_bytes[11] << 3)) &
	                         0x07FF;
	hsbus->sbusCh[9].rawVal.value =
	      (((uint16_t)sbus_msg_bytes[12]) | ((uint16_t)sbus_msg_bytes[13] << 8)) &
	      0x07FF;
	hsbus->sbusCh[10].rawVal.value = (((uint16_t)sbus_msg_bytes[13] >> 3) |
	                          ((uint16_t)sbus_msg_bytes[14] << 5)) &
	                         0x07FF;
	hsbus->sbusCh[11].rawVal.value = (((uint16_t)sbus_msg_bytes[14] >> 6) |
	                           ((uint16_t)sbus_msg_bytes[15] << 2) |
	                           ((uint16_t)sbus_msg_bytes[16] << 10)) &
	                          0x07FF;
	hsbus->sbusCh[12].rawVal.value = (((uint16_t)sbus_msg_bytes[16] >> 1) |
	                           ((uint16_t)sbus_msg_bytes[17] << 7)) &
	                          0x07FF;
	hsbus->sbusCh[13].rawVal.value = (((uint16_t)sbus_msg_bytes[17] >> 4) |
	                           ((uint16_t)sbus_msg_bytes[18] << 4)) &
	                          0x07FF;
	hsbus->sbusCh[14].rawVal.value = (((uint16_t)sbus_msg_bytes[18] >> 7) |
	                           ((uint16_t)sbus_msg_bytes[19] << 1) |
	                           ((uint16_t)sbus_msg_bytes[20] << 9)) &
	                          0x07FF;
	hsbus->sbusCh[15].rawVal.value = (((uint16_t)sbus_msg_bytes[20] >> 2) |
	                           ((uint16_t)sbus_msg_bytes[21] << 6)) &
	                          0x07FF;
	hsbus->sbusCh[16].rawVal.value = (((uint16_t)sbus_msg_bytes[21] >> 5) |
	                           ((uint16_t)sbus_msg_bytes[22] << 3)) &
	                          0x07FF;
	hsbus->sbusHealth.frameLost= (((uint16_t)sbus_msg_bytes[23]) &
			                           ((uint16_t)0x04));
	hsbus->sbusHealth. failsafeActivated = (((uint16_t)sbus_msg_bytes[23]) &
			                           ((uint16_t)0x08));
/*
	for (i=0;i<17;i++)
		{

			if (hsbus->sbusCh[i].digVal.calculationEnabled)
			{
				if (hsbus->sbusCh[i].rawVal.value <hsbus->sbusCh[i].digVal.Aval)
					hsbus->sbusCh[i].digVal.A = 1;
				else
					hsbus->sbusCh[i].digVal.A = 0;

				if ((hsbus->sbusCh[i].rawVal.value >= hsbus->sbusCh[i].digVal.Aval) && (hsbus->sbusCh[i].rawVal.value < hsbus->sbusCh[i].digVal.Bval))
					hsbus->sbusCh[i].digVal.B= 1;
				else
					hsbus->sbusCh[i].digVal.B = 0;

				if (hsbus->sbusCh[i].rawVal.value >=hsbus->sbusCh[i].digVal.Bval )
					hsbus->sbusCh[i].digVal.C= 1;
				else
					hsbus->sbusCh[i].digVal.C = 0;
			}

			if (hsbus->sbusCh[i].scaledVal.calculationEnabled&&((hsbus->sbusCh[i].rawVal.max - hsbus->sbusCh[i].rawVal.min)>0))
			{
				szamlalo 	= (hsbus->sbusCh[i].rawVal.value- hsbus->sbusCh[i].rawVal.min)*( hsbus->sbusCh[i].scaledVal.max - hsbus->sbusCh[i].scaledVal.min);
				nevezo 		= hsbus->sbusCh[i].rawVal.max - hsbus->sbusCh[i].rawVal.min;
				hsbus->sbusCh[i].scaledVal.value = (int16_t)(szamlalo/nevezo) + hsbus->sbusCh[i].scaledVal.min;

			}
*/

	hsbus->sbusHealth.sbusTimeOut = 0;
}

void refreshSbusDigitChs(struct sbusth *hsbus)
{
	uint8_t i;
	for (i=0;i<17;i++)
		{

			if (hsbus->sbusCh[i].digVal.calculationEnabled)
			{
				if (hsbus->sbusCh[i].rawVal.value <hsbus->sbusCh[i].digVal.Aval)
					hsbus->sbusCh[i].digVal.A = 1;
				else
					hsbus->sbusCh[i].digVal.A = 0;

				if ((hsbus->sbusCh[i].rawVal.value >= hsbus->sbusCh[i].digVal.Aval) && (hsbus->sbusCh[i].rawVal.value < hsbus->sbusCh[i].digVal.Bval))
					hsbus->sbusCh[i].digVal.B= 1;
				else
					hsbus->sbusCh[i].digVal.B = 0;

				if (hsbus->sbusCh[i].rawVal.value >=hsbus->sbusCh[i].digVal.Bval )
					hsbus->sbusCh[i].digVal.C= 1;
				else
					hsbus->sbusCh[i].digVal.C = 0;
			}
		}
}

void refreshSbusAnChs(struct sbusth *hsbus)
{
 int32_t szamlalo;
 int16_t nevezo;
 uint8_t i;

	for (i=0;i<17;i++)
		{
			if (hsbus->sbusCh[i].scaledVal.calculationEnabled&&((hsbus->sbusCh[i].rawVal.max - hsbus->sbusCh[i].rawVal.min)>0))
			{
				szamlalo 	= (hsbus->sbusCh[i].rawVal.value- hsbus->sbusCh[i].rawVal.min)*( hsbus->sbusCh[i].scaledVal.max - hsbus->sbusCh[i].scaledVal.min);
				nevezo 		= hsbus->sbusCh[i].rawVal.max - hsbus->sbusCh[i].rawVal.min;
				hsbus->sbusCh[i].scaledVal.value = (int16_t)(szamlalo/nevezo) + hsbus->sbusCh[i].scaledVal.min;

			}
		}
}

void refreshSbusCh(struct sbusChth *sbusCh)
{
	int32_t szamlalo;
	int16_t nevezo;
	if (sbusCh->scaledVal.calculationEnabled&&((sbusCh->rawVal.max - sbusCh->rawVal.min)>0))
		{
			szamlalo 	= (sbusCh->rawVal.value - sbusCh->rawVal.min)*( sbusCh->scaledVal.max - sbusCh->scaledVal.min);
			nevezo 		= sbusCh->rawVal.max - sbusCh->rawVal.min;
			sbusCh->scaledVal.value = (int16_t)(szamlalo/nevezo) + sbusCh->scaledVal.min;

		}
}
