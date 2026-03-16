/*
 * encoder.c
 *
 *  Created on: Jan 21, 2026
 *      Author: krzys
 */

#include "encoder.h"

void encoder_init(encoder_t *encoder)
{
	uint8_t a = HAL_GPIO_ReadPin(encoder->port_A, encoder->pin_A);
	uint8_t b = HAL_GPIO_ReadPin(encoder->port_B, encoder->pin_B);
	encoder->last_state = (a << 1) | b;
}

int8_t encoder_step(encoder_t *encoder)
{
	static const int8_t transition_table[16] = {
			 0, -1,  1,  0,
			 1,  0,  0, -1,
			-1,  0,  0,  1,
			 0,  1, -1,  0
	};

	uint8_t a = HAL_GPIO_ReadPin(encoder->port_A, encoder->pin_A);
	uint8_t b = HAL_GPIO_ReadPin(encoder->port_B, encoder->pin_B);

	uint8_t state = (a << 1) | b;

	uint8_t idx = (encoder->last_state << 2) | state;
	encoder ->last_state = state;

	return transition_table[idx];
}
