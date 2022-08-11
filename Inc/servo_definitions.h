/*
 * servo_definitions.h
 *
 *  Created on: 2022. jún. 24.
 *      Author: nemet
 */

#ifndef INC_SERVO_DEFINITIONS_H_
#define INC_SERVO_DEFINITIONS_H_

#define SERVOA 	&motionb->servoA
#define SERVOB 	&motionb->servoB
#define SERVOC 	&motionb->servoC


#define SERVO_A_SPD 	motionb->servoA.encoder.diff_10ms;
#define SERVO_B_SPD 	motionb->servoB.encoder.diff_10ms;
#define SERVO_C_SPD 	motionb->servoC.encoder.diff_10ms;

#define ENCODER_SPD_MIN	1

#define SERVO_A_HAS_SPD 	(abs(motionb->servoA.encoder.diff_10ms) < ENCODER_SPD_MIN)
#define SERVO_B_HAS_SPD 	(abs(motionb->servoB.encoder.diff_10ms) < ENCODER_SPD_MIN)
#define SERVO_C_HAS_SPD 	(abs(motionb->servoC.encoder.diff_10ms) < ENCODER_SPD_MIN)

#endif /* INC_SERVO_DEFINITIONS_H_ */
