/**
  ******************************************************************************
  * @file     tm1637.c
  * @author   Krzysztof Cajler
  * @version  V1.0
  * @date     20/01/2026
  * @brief    Default under dev library file.
  ******************************************************************************
*/

/** Includes -----------------------------------------------------------------*/
#include "tm1637.h"

/** Private defines -----------------------------------------------------------*/
#define AUTO_INCR		0x40

#define DIGIT0 			0xC0
#define DIGIT1			0xC1
#define DIGIT2			0xC2
#define DIGIT3			0xC3
#define DIGIT4			0xC4

#define DISP_OFF		0x80
#define DISP_ON			0x88

#define NUMBER_0		0x3F
#define NUMBER_1		0x06
#define NUMBER_2		0x5B
#define NUMBER_3		0x4F
#define NUMBER_4		0x66
#define NUMBER_5		0x6D
#define NUMBER_6		0x7D
#define NUMBER_7		0x07
#define NUMBER_8		0x7F
#define NUMBER_9		0x6F
#define DASH			0x40


/** Private Function Definitions ------------------------------------------------*/
int start_communication(tm1637_t *p);
int stop_communication(tm1637_t *p);
int write_data(tm1637_t *p, uint8_t data);
int convert_time(uint8_t digits[4], uint16_t seconds);
void delay_clk_cycles(uint16_t cycles);

/** Public Functions ------------------------------------------------------------*/
int tm1637_init(tm1637_t *p, uint8_t mode)
{
	if(p == NULL) return -1;
	if(p->brightness > 7) p->brightness = 7;

	start_communication(p);
	write_data(p, AUTO_INCR);
	stop_communication(p);

	start_communication(p);
	write_data(p, DIGIT0);
	if (mode == 0)
	{
		for(uint8_t i = 0; i < 4; i++)
		{
			write_data(p, NUMBER_0);
		}
	} else {
		for(uint8_t i = 0; i < 4; i++)
		{
			write_data(p, DASH);
		}
	}

	stop_communication(p);

	start_communication(p);
	write_data(p, DISP_ON | p->brightness);
	stop_communication(p);

	return 0;

}

int tm1637_off(tm1637_t *p)
{
	if(p == NULL) return -1;

	start_communication(p);
	write_data(p, DISP_OFF);
	stop_communication(p);

	return 0;
}

int tm1637_on(tm1637_t *p, uint8_t brightness)
{
	if(p == NULL) return -1;
	if(brightness > 7) brightness = 7;

	start_communication(p);
	if (brightness == 0){
		write_data(p, p->brightness);
	} else {
		write_data(p, DISP_ON | brightness);
	}
	write_data(p, DISP_ON);
	stop_communication(p);

	return 0;
}

int tm1637_update_time(tm1637_t *p, uint16_t seconds)
{
	if(p == NULL) return -1;

	uint8_t digits[4];
	convert_time(digits, seconds);

	start_communication(p);
	write_data(p, DIGIT0);

	for(uint8_t i = 0; i < 4; i++)
	{
		uint8_t hex;
		switch(digits[i]) {
		case 0: hex = NUMBER_0; break;
		case 1: hex = NUMBER_1; break;
		case 2: hex = NUMBER_2; break;
		case 3: hex = NUMBER_3; break;
		case 4: hex = NUMBER_4; break;
		case 5: hex = NUMBER_5; break;
		case 6: hex = NUMBER_6; break;
		case 7: hex = NUMBER_7; break;
		case 8: hex = NUMBER_8; break;
		case 9: hex = NUMBER_9; break;
		default: hex = 0xFF; break;
		}
		write_data(p, hex);;
	}

	stop_communication(p);

	return 0;
}
/** Private Functions -----------------------------------------------------------*/
int start_communication(tm1637_t *p)
{
	if(p == NULL) return -1;

	p->dio_port->BSRR = p->dio_pin;
	p->clk_port->BSRR = p->clk_pin;
	delay_clk_cycles(2*DELAY_US);

	p->dio_port->BSRR = p->dio_pin << 16;

	return 0;
}

int stop_communication(tm1637_t *p)
{
	if(p == NULL) return -1;

	p->clk_port->BSRR = p->clk_pin << 16;
	delay_clk_cycles(2*DELAY_US);
	p->dio_port->BSRR = p->dio_pin << 16;
	delay_clk_cycles(2*DELAY_US);
	p->clk_port->BSRR = p->clk_pin;
	delay_clk_cycles(2*DELAY_US);
	p->dio_port->BSRR = p->dio_pin;

	return 0;
}

int write_data(tm1637_t *p, uint8_t data)
{
	if(p == NULL) return -1;

	uint8_t _data = data;
	for(uint8_t i = 0; i < 8; i++)
	{
		p->clk_port->BSRR = p->clk_pin << 16;
		if(_data & 0x01) {
			p->dio_port->BSRR = p->dio_pin;
		} else {
			p->dio_port->BSRR = p->dio_pin << 16;
		}
		delay_clk_cycles(3*DELAY_US);
		_data >>= 1;
		p->clk_port->BSRR = p->clk_pin;
		delay_clk_cycles(3*DELAY_US);
	}

	p->clk_port->BSRR = p->clk_pin << 16; // CLK low
	p->dio_port->BSRR = p->dio_pin;       // release DIO (HIGH)
	delay_clk_cycles(2*DELAY_US);

	p->clk_port->BSRR = p->clk_pin;       // CLK high (ACK sampled here)
	delay_clk_cycles(2*DELAY_US);

	return 0;
}

int convert_time(uint8_t digits[4], uint16_t seconds)
{
	if (digits == NULL) return -1;

	uint8_t m = seconds / 60;
	uint8_t s = seconds % 60;

	digits[0] = m / 10;
	digits[1] = m % 10;
	digits[2] = s / 10;
	digits[3] = s % 10;

	return 0;
}

void delay_clk_cycles(uint16_t cycles) {
	for(uint16_t i = 0; i < cycles; i++)
	{
		__NOP();
	}
}
