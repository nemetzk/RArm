/*
 * rc_configuration.h
 *
 *  Created on: Apr 5, 2022
 *      Author: nemet
 */

#ifndef INC_RC_CONFIGURATION_H_
#define INC_RC_CONFIGURATION_H_

/* **********		RC TRANSMITTER CHANNEL CONFIG 2- switch naming ********** */

#define SH_A motionb->sbus.sbusCh[SH].digVal.A
#define SH_B motionb->sbus.sbusCh[SH].digVal.B

#define SC_A motionb->sbus.sbusCh[SC].digVal.A
#define SC_B motionb->sbus.sbusCh[SC].digVal.B
#define SC_C motionb->sbus.sbusCh[SC].digVal.C

#define SD_A motionb->sbus.sbusCh[SD].digVal.A
#define SD_B motionb->sbus.sbusCh[SD].digVal.B
#define SD_C motionb->sbus.sbusCh[SD].digVal.C

#define SG_A motionb->sbus.sbusCh[SG].digVal.A
#define SG_B motionb->sbus.sbusCh[SG].digVal.B
#define SG_C motionb->sbus.sbusCh[SG].digVal.C

#endif /* INC_RC_CONFIGURATION_H_ */
