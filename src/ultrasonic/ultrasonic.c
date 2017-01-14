/*
 * ultrasonic.c
 *
 *  Created on: 14. 1. 2017
 *      Author: Matej Vargovcik
 */

#include "ultrasonic.h"
#include <stddef.h>
#include "stm32l1xx.h"
#include "display/display.h"

typedef struct UltrasonicSensor {
	GPIO_TypeDef *gpioOut;
	GPIO_TypeDef *gpioIn;
	GPIO_TypeDef *gpioLed;
	uint16_t pinOut;
	uint16_t pinIn;
	uint16_t pinLed;
	int placeOccupied;
	int senseCount;
} UltrasonicSensor;

UltrasonicSensor ultrasonicSensor(GPIO_TypeDef *gpioOut, uint16_t pinOut, GPIO_TypeDef *gpioIn,
		uint16_t pinIn, GPIO_TypeDef *gpioLed, uint16_t pinLed) {
	UltrasonicSensor result;
	result.gpioOut = gpioOut;
	result.gpioIn = gpioIn;
	result.gpioLed = gpioLed;
	result.pinOut = pinOut;
	result.pinIn = pinIn;
	result.pinLed = pinLed;
	result.placeOccupied = 0;
	result.senseCount = 0;
	return result;
}

UltrasonicSensor ultrasonicSensors[kUltrasonicSensorsCount];

void initializeUltrasonicSensors() {
	ultrasonicSensors[0] = ultrasonicSensor(GPIOA, GPIO_Pin_3, GPIOA, GPIO_Pin_2, GPIOA, GPIO_Pin_10);
	//ultrasonicSensors[1] = ultrasonicSensor(GPIOB, GPIO_PinSource3, GPIOB, GPIO_PinSource5, GPIOB, GPIO_PinSource4);
	//ultrasonicSensors[2] = ultrasonicSensor(GPIOC, GPIO_PinSource0, GPIOC, GPIO_PinSource1, GPIOB, GPIO_PinSource0);
	//ultrasonicSensors[3] = ultrasonicSensor(GPIOA, GPIO_PinSource4, GPIOA, GPIO_PinSource1, GPIOA, GPIO_PinSource0);

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);

	for (int i = 0; i < kUltrasonicSensorsCount; i++) {
		//trigger_sensor
		GPIO_InitTypeDef gpioInitStruct;
		gpioInitStruct.GPIO_OType = GPIO_OType_PP;
		gpioInitStruct.GPIO_Speed = GPIO_Speed_400KHz;

		gpioInitStruct.GPIO_Mode = GPIO_Mode_OUT;
		gpioInitStruct.GPIO_Pin = ultrasonicSensors[i].pinOut;
		GPIO_Init(ultrasonicSensors[i].gpioOut, &gpioInitStruct);

		gpioInitStruct.GPIO_Mode = GPIO_Mode_IN;
		gpioInitStruct.GPIO_Pin = ultrasonicSensors[i].pinIn;
		GPIO_Init(ultrasonicSensors[i].gpioIn, &gpioInitStruct);

		gpioInitStruct.GPIO_Mode = GPIO_Mode_OUT;
		gpioInitStruct.GPIO_Pin = ultrasonicSensors[i].pinLed;
		GPIO_Init(GPIOA, &gpioInitStruct);//(ultrasonicSensors[i].gpioLed, &gpioInitStruct);
	}
}

int measureProximity(int sensor) {
	  GPIO_TypeDef *gpioOut = ultrasonicSensors[sensor].gpioOut;
	  uint16_t pinOut = ultrasonicSensors[sensor].pinOut;
	  GPIO_TypeDef *gpioIn = ultrasonicSensors[sensor].gpioIn;
	  uint16_t pinIn = ultrasonicSensors[sensor].pinIn;
	  GPIO_SetBits(gpioOut, pinOut);
	  for(int i=0;i<10;i++);
	  GPIO_ResetBits(gpioOut, pinOut);
	  //wait for pulse on echo pin
	  while(GPIO_ReadInputDataBit(gpioIn, pinIn)==0);
	  //measure pulse width
	  int time = 0;
	  while(GPIO_ReadInputDataBit(gpioIn, pinIn)==1) time++;

	  float distance=(time/58.0)-7.0;

	  ultrasonicSensors[sensor].senseCount++;
	  displayPlacesCount(ultrasonicSensors[sensor].senseCount);

	  if (distance < 3.0) {
		  ultrasonicSensors[sensor].placeOccupied = 1;
		  //TODO
		  GPIO_SetBits(ultrasonicSensors[sensor].gpioLed, ultrasonicSensors[sensor].pinLed);
	  }
	  else {
		  ultrasonicSensors[sensor].placeOccupied = 0;
		  //TODO
		  GPIO_ResetBits(ultrasonicSensors[sensor].gpioLed, ultrasonicSensors[sensor].pinLed);
	  }

	  return ultrasonicSensors[sensor].placeOccupied;
}
