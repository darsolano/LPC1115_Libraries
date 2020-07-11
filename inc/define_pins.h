/*
 * define_pins.h
 *
 *  Created on: Apr 1, 2016
 *  Author: dsolano
 *  For LPC_1115 NXP CM0
 */

#ifndef INCLUDE_DEFINE_PINS_H_
#define INCLUDE_DEFINE_PINS_H_

/* define_pins.h
 *
 * Define a useful macro for giving GPIO pins nicer, semantic names.
 *
 * The only macro exported is DEFINE_PIN. Use as:
 *  DEFINE_PIN(PIN_NAME, port number, pin number)
 * this will define the following:
 *  PIN_NAME_DEASSERT(), _OFF(), _LOW() to bring the pin low
 *  PIN_NAME_ASSERT(), _ON(), _HIGH() to bring the pin high
 *  PIN_NAME_INPUT() to make the pin an input
 *  PIN_NAME_OUTPUT() to make the pin an output
 *  PIN_NAME_READ() to read the current state of the pin
 *                  (returns a 1 or 0, typed as a uint_fast8_t)
 */

#include <chip.h>
#include <stdint.h>
#include <lpc_types.h>

//LPC_GPIO##port ->MASKED_ACCESS[1<<pin] = (0<<pin);
//LPC_GPIO##port ->DATA |= (1 << pin);

#define DEFINE_PIN(name, port, pin) \
inline static void name##_DEASSERT() { \
	LPC_GPIO[port].DATA[1 << pin] &= ~(1 << pin);\
} \
inline static void name##_OFF() { \
	LPC_GPIO[port].DATA[1 << pin] = (1 << pin);\
} \
inline static void name##_LOW() { \
	LPC_GPIO[port].DATA[1 << pin] = (1 << pin);\
} \
inline static void name##_SELECTED() { \
	LPC_GPIO[port].DATA[1 << pin] = 0;\
} \
inline static void name##_ASSERT() { \
	LPC_GPIO[port].DATA[1 << pin] = (1 << pin);\
} \
inline static void name##_ON() { \
	LPC_GPIO[port].DATA[1 << pin] = (1 << pin);\
} \
inline static void name##_DESELECTED() { \
	LPC_GPIO[port].DATA[1 << pin] = (1 << pin);\
} \
inline static void name##_HIGH() { \
	LPC_GPIO[port].DATA[1 << pin] = (1 << pin);\
} \
inline static void name##_TOGGLE() { \
	LPC_GPIO[port].DATA[1 << pin] ^= (1 << pin);\
} \
inline static void name##_INPUT() { \
	LPC_GPIO[port].DIR &= ~(1UL << pin); \
} \
inline static void name##_OUTPUT() { \
	LPC_GPIO[port].DIR |= (1UL << pin);\
} \
inline static uint_fast8_t name##_READ() { \
	return ( LPC_GPIO[port].DATA[1 << pin] >> pin) & 1; \
}

#endif /* INCLUDE_DEFINE_PINS_H_ */
