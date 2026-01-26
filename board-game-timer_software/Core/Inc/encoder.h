/*
 * encoder.h
 *
 *  Created on: Jan 21, 2026
 *      Author: krzys
 */

#ifndef INC_ENCODER_H_
#define INC_ENCODER_H_

#include "stm32l4xx_hal.h"

typedef struct encoder {
	GPIO_TypeDef *port_A;
	uint16_t pin_A;
	GPIO_TypeDef *port_B;
	uint16_t pin_B;
	uint8_t last_state;
} encoder_t;

void encoder_init(encoder_t *encoder);
int8_t encoder_step(encoder_t *enc);

#endif /* INC_ENCODER_H_ */
