/**
  ******************************************************************************
  * @file     tm1637.h
  * @author   Krzysztof Cajler
  * @version  V1.0
  * @date     20/01/2026
  * @brief    Default under dev library file.
  ******************************************************************************
*/

#ifndef __TM1637_H
#define __TM1637_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "stm32l4xx.h"

#define DELAY_US		500

typedef struct tm1637_t {
    GPIO_TypeDef *clk_port;
	GPIO_TypeDef *dio_port;
	uint16_t clk_pin;
	uint16_t dio_pin;
	uint8_t brightness;
} tm1637_t;

/* Initialization of the display with all zeros,
 * mode = 0 -> tm1637_t initialized and zeros are shown
 * mode = 1 -> tm1638 initialized and dashes are shown
 *  */
int tm1637_init(tm1637_t *p, uint8_t mode);

/* Turn segments on, brightness = 0 -> default value passed in struct
 * brightness 1-8 -> turns on and changes default as well
 */
int tm1637_on(tm1637_t *p, uint8_t brighness);

/* Turns all segments off */
int tm1637_off(tm1637_t *p);

/* Update screen with new time in seconds, it will be converted to mm:ss */
int tm1637_update_time(tm1637_t *p, uint16_t seconds);

#ifdef __cplusplus
}
#endif

#endif /* __TM1637_H */

