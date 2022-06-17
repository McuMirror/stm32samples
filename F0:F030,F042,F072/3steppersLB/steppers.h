/*
 * This file is part of the 3steppers project.
 * Copyright 2021 Edward V. Emelianov <edward.emelianoff@gmail.com>.
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
#ifndef STEPPERS_H__
#define STEPPERS_H__

#include <stm32f0.h>
#include "commonproto.h"

// amount of tries to detect motor stall
#define NSTALLEDMAX     (5)
// amount of steps to detect stalled state
#define STALLEDSTEPS    (15)
// amount of tries to keep current position (need for states near problem places)
#define KEEPPOSMAX      (10)

// stepper states
typedef enum{
    STP_RELAX,      // 0 - no moving
    STP_ACCEL,      // 1 - start moving with acceleration
    STP_MOVE,       // 2 - moving with constant speed
    STP_MVSLOW,     // 3 - moving with slowest constant speed (end of moving)
    STP_DECEL,      // 4 - moving with deceleration
    STP_STALL,      // 5 - stalled
    STP_ERR         // 6 - wrong/error state
} stp_state;

// end-switches reaction
enum{
    ESW_IGNORE,     // don't stop @ end-switch
    ESW_ANYSTOP,    // stop @ esw in any moving direction
    ESW_STOPMINUS,  // stop only in negative moving
    ESW_AMOUNT      // number of records
};

// find zero stages: fast -> 0, slow -> +, slow -> 0

void addmicrostep(uint8_t i);
void encoders_UPD(uint8_t i);

void init_steppers();
int32_t encoder_position(uint8_t i);
int setencpos(uint8_t i, int32_t position);
errcodes setmotpos(uint8_t i, int32_t position);

errcodes getpos(uint8_t i, int32_t *position);
errcodes getremainsteps(uint8_t i, int32_t *position);
errcodes motor_absmove(uint8_t i, int32_t abssteps);
errcodes motor_relmove(uint8_t i, int32_t relsteps);
errcodes motor_relslow(uint8_t i, int32_t relsteps);
errcodes motor_goto0(uint8_t i);

uint8_t geteswreact(uint8_t i);

void emstopmotor(uint8_t i);
void stopmotor(uint8_t i);
stp_state getmotstate(uint8_t i);
void process_steppers();

#endif // STEPPERS_H__
