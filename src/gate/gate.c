/*
 * gate.c
 *
 *  Created on: 13. 12. 2016
 *      Author: Matej Vargovcik
 */

#include "gate.h"
#include "servo.h"

void setGateAngle(int degrees);

void initializeGate() {
	initializeServo();
	closeGate();
}

void openGate() {
	setGateAngle(90);
}

void closeGate() {
	setGateAngle(0);
}

void setGateAngle(int degrees) {
	setServoSignalLength(degrees*1000/180 + 400);
}
