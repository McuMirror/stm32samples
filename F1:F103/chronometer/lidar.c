/*
 * This file is part of the chronometer project.
 * Copyright 2019 Edward V. Emelianov <edward.emelianoff@gmail.com>.
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

#include "flash.h"
#include "lidar.h"
#include "usart.h"

uint16_t last_lidar_dist = 0;
uint16_t last_lidar_stren = 0;
uint16_t lidar_triggered_dist = 0;

extern uint32_t shotms[];

/**
 * @brief parse_lidar_data - parsing of string from lidar
 * @param txt - the string or NULL (if you want just check trigger state)
 * @return trigger state
 */
uint8_t parse_lidar_data(char *txt){
    static uint8_t triggered = 0;
    if(!txt){
        // clear trigger state after timeout -> need to monitor lidar
        uint32_t len = Tms - shotms[LIDAR_TRIGGER];
        //if(len > MAX_TRIG_LEN || len > (uint32_t)the_conf.trigpause[LIDAR_TRIGGER]){
        if(len > MAX_TRIG_LEN){
            triggered = 0;
            DBG("MAX time gone, untrigger!");
        }
        return triggered;
    }
    last_lidar_dist = txt[2] | (txt[3] << 8);
    last_lidar_stren = txt[4] | (txt[5] << 8);
    if(last_lidar_stren < LIDAR_LOWER_STREN) return 0; // weak signal
    if(!lidar_triggered_dist){ // first run
        lidar_triggered_dist = last_lidar_dist;
        return 0;
    }
    IWDG->KR = IWDG_REFRESH;
    if(triggered){ // check if body gone
        if(last_lidar_dist < the_conf.dist_min || last_lidar_dist > the_conf.dist_max || last_lidar_dist > lidar_triggered_dist + LIDAR_DIST_THRES){
            triggered = 0;
#ifdef EBUG
            SEND("Untriggered! distance=");
            printu(1, last_lidar_dist);
            SEND(" signal=");
            printu(1, last_lidar_stren);
            newline(1);
#endif
        }
    }else{
        if(last_lidar_dist > the_conf.dist_min && last_lidar_dist < the_conf.dist_max){
            savetrigtime();
            triggered = 1;
            lidar_triggered_dist = last_lidar_dist;
            fillshotms(LIDAR_TRIGGER);
#ifdef EBUG
            SEND("Triggered! distance=");
            printu(1, last_lidar_dist);
            SEND(" signal=");
            printu(1, last_lidar_stren);
            newline(1);
#endif
        }
    }
    return triggered;
}
