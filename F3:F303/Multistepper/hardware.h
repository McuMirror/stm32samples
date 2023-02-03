/*
 * This file is part of the multistepper project.
 * Copyright 2023 Edward V. Emelianov <edward.emelianoff@gmail.com>.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <stm32f3.h>

#define USBPU_port  GPIOA
#define USBPU_pin   (1<<13)
#define USBPU_ON()  pin_set(USBPU_port, USBPU_pin)
#define USBPU_OFF() pin_clear(USBPU_port, USBPU_pin)

// Buttons amount
#define BTNSNO      (7)
// Buttons ports & pins
extern volatile GPIO_TypeDef* const BTNports[BTNSNO];
extern const uint32_t BTNpins[BTNSNO];
// state 1 - pressed, 0 - released (pin active is zero)
#define BTN_state(x)    ((BTNports[x]->IDR & BTNpins[x]) ? 0 : 1)


extern volatile uint32_t Tms;

void hw_setup();

