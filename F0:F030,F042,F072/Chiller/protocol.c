/*
 * This file is part of the Chiller project.
 * Copyright 2018 Edward V. Emelianov <edward.emelianoff@gmail.com>.
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
#include "hardware.h"
#include "protocol.h"
#include "usart.h"
#include "adc.h"
#include "mainloop.h"

extern uint8_t crit_error;

#ifdef EBUG
/**
 * @brief debugging_proc - debugging functions
 * @param command - rest of cmd
 */
static void debugging_proc(const char *command){
    const char *ptr = command;
    int i;
    switch(*ptr++){
        case 'w':
            usart1_send("Test watchdog", 0);
            while(1){nop();}
        break;
        case 'A': // raw ADC values depending on next symbol
            i = *ptr++ - '0';
            if(i < 0 || i > NUMBER_OF_ADC_CHANNELS){
                usart1_send("Wrong channel nuber!", 0);
                return;
            }
            put_string("ADC value: ");
            put_uint(getADCval(i));
            put_string(", ");
            usart1_sendbuf();
        break;
        case 'F':
            put_string("PB_IDR, flow_cntr: ");
            put_uint(GPIOB->IDR);
            put_string(", ");
            put_uint(flow_cntr);
            usart1_sendbuf();
        break;
        case 'T': // all raw T values
            for(i = 0; i < 4; ++i){
                put_uint(getADCval(i));
                put_char('\t');
            }
            usart1_sendbuf();
        break;
        default:
        break;
    }
}
#endif

/**
 * @brief get_ntc - show value of ith NTC temperature
 * @param str (i) - user string, first char should be '0'..'3'
 */
static void get_ntc(const char *str){
    uint8_t N = *str - '0';
    if(N > 3) return;
    put_string("NTC");
    put_char(*str);
    put_char('=');
    put_int(NTCval[N]);
}

#define STR(a)      XSTR(a)
#define XSTR(a)     #a
/**
 * @brief process_command - command parser
 * @param command - command text (all inside [] without spaces)
 * @return text to send over terminal or NULL
 */
char *process_command(const char *command){
    const char *ptr = command;
    char *ret = NULL;
    int32_t N;
    usart1_sendbuf(); // send buffer (if it is already filled)
    switch(*ptr++){
        case '?': // help
            SEND_BLK(
                "Ax - alarm on(1)/off(0)\n"
                "Cx - cooler PWM\n"
                "CLR- clear critical error\n"
                "F  - get flow sensor rate for " FLOWRATESTR "s (5880 pulses per liter)\n"
                "Hx - heater PWM\n"
                "L  - check water level\n"
                "Px - pump PWM\n"
                "R  - reset\n"
                "Sx - change temperature setpoint\n"
                "Tx - get NTC[x] temperature\n"
                "t  - get MCU temperature (approx.)\n"
                "V  - get Vdd"
                );
#ifdef EBUG
            SEND_BLK("d -> goto debug:\n"
                 "\tAx - get raw ADCx value\n"
                 "\tF - get flow_cntr\n"
                 "\tT - show raw T values\n"
                 "\tw - test watchdog"
                 );
#endif
        break;
        case 'A': // turn alarm on/off
            if(*ptr == '1') ALARM_ON();
            else if(*ptr == '0') ALARM_OFF();
            put_string("ALRM=");
            put_char(ALARM_STATE() + '0');
        break;
        case 'C': // "CLR" - clear critical error flag, 'C' - cooler PWM - TIM14CH1
            if(ptr[0] == 'L' && ptr[1] == 'R' && ptr[2] == 0){
                crit_error = 0;
                return "CLRERR=1\n";
            }
            if(getnum(ptr, &N) && N > -1 && N < 256){
                SET_COOLER_PWM(N);
            }
            put_string("COOLERPWM=");
            put_int(GET_COOLER_PWM());
        break;
        case 'F':
            put_string("FLOWRATE=");
            put_uint(flow_rate);
        break;
        case 'H': // heater PWM - TIM16CH1
            if(getnum(ptr, &N) && N > -1 && N < 256){
                SET_HEATER_PWM(N);
            }
            put_string("HEATERPWM=");
            put_int(GET_HEATER_PWM());
        break;
        case 'L': // water level
            put_string("WATERLEVEL=");
            put_char('0' + pin_read(GPIOF, 1));
        break;
        case 'P': // pump PWM - TIM17CH1
            if(getnum(ptr, &N) && N > -1 && N < 256){
                SET_PUMP_PWM(N);
            }
            put_string("PUMPPWM=");
            put_int(GET_PUMP_PWM());
        break;
        case 'R': // reset MCU
            NVIC_SystemReset();
        break;
        case 'S':
            if(getnum(ptr, &N) && N > OUTPUT_T_L + TEMP_TOLERANCE && N < OUTPUT_T_H - TEMP_TOLERANCE){
                Tset = N;
            }
            put_string("TSET=");
            put_int(Tset);
        break;
        case 'T': // get temperature of NTC(x)
            get_ntc(ptr);
        break;
        case 't': // get mcu T
            put_string("MCUTEMP10=");
            put_int(getMCUtemp());
        break;
        case 'V': // get Vdd
            put_string("VDD100=");
            put_uint(getVdd());
        break;
#ifdef EBUG
        case 'd':
            debugging_proc(ptr);
            return NULL;
        break;
#endif
    }
    usart1_sendbuf();
    return ret;
}
