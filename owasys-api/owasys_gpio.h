/*
 * Copyright © 2017 Owasys.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef __OWASYS_GPIO_H__
#define __OWASYS_GPIO_H__

#include <linux/types.h>

   #ifdef OWA4X
#define FIRST_GPIO 	GPIO_TO_PIN(0, 0)
#define LAST_GPIO 	GPIO_TO_PIN(3, 21)
   #endif
   #ifdef OWA5X
#define FIRST_GPIO 	GPIO_TO_PIN(1, 0)
#define LAST_GPIO 	GPIO_TO_PIN(5, 29)
   #endif
   #if defined OWA344 || defined OWA340
#define FIRST_GPIO 	GPIO_TO_PIN(1, 0)
#define LAST_GPIO 	GPIO_TO_PIN(5, 29)
   #endif

#define INVALID_VALUE(VALUE) ( (VALUE < 0) || (VALUE > 1) )
#define INVALID_PIN(PIN) ( (PIN < FIRST_GPIO) || (PIN > LAST_GPIO) )

typedef struct _t_owasys_gpio
{
   int pin;
   int value;
} t_owasys_gpio;

typedef t_owasys_gpio t_owa4x_gpio;

/* Get basic GPIO characteristics info */
#define SET_OUTPUT_VALUE	_IOW ('Y', 1, struct _t_owasys_gpio)
#define GET_INPUT_VALUE	   _IOW ('Y', 2, struct _t_owasys_gpio)
#define SET_GPIO_MODE	   _IOW ('Y', 3, struct _t_owasys_gpio)
#define SET_GPIO_INPUT	   _IOW ('Y', 4, struct _t_owasys_gpio)
#define SET_GPIO_OUTPUT	   _IOW ('Y', 5, struct _t_owasys_gpio)

#endif /* __OWA4X_GPIO_H__ */
