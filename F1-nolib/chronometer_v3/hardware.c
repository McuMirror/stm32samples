/*
 *                                                                                                  geany_encoding=koi8-r
 * hardware.c - hardware-dependent macros & functions
 *
 * Copyright 2018 Edward V. Emelianov <eddy@sao.ru, edward.emelianoff@gmail.com>
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
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 */

#include "adc.h"
#include "hardware.h"
#include "flash.h"
#include "lidar.h"
#include "str.h"
#include "time.h"
#include "usart.h"

#include <string.h> // memcpy

uint8_t buzzer_on = 1; // buzzer ON by default
uint8_t LEDSon = 1; // LEDS are working
// ports of triggers: PB0, PB1, PB3
static GPIO_TypeDef *trigport[DIGTRIG_AMOUNT] = {GPIOB, GPIOB, GPIOB};
// pins of triggers: 0, 1, 3
static uint16_t trigpin[DIGTRIG_AMOUNT] = {1<<0, 1<<1, 1<<3};
// value of pin in `triggered` state
static uint8_t trigstate[DIGTRIG_AMOUNT];
// time of triggers shot
trigtime shottime[TRIGGERS_AMOUNT];
// Tms value when they shot
uint32_t shotms[TRIGGERS_AMOUNT];
// trigger length (-1 if > MAX_TRIG_LEN)
int16_t triglen[TRIGGERS_AMOUNT];
// if trigger[N] shots, the bit N will be 1
uint8_t trigger_shot = 0;

static inline void gpio_setup(){
    BUZZER_OFF(); // turn off buzzer @start
    LED_on(); // turn ON LED0 @start
    LED1_off(); // turn off LED1 @start
    USBPU_OFF(); // turn off USB pullup @start
    // Enable clocks to the GPIO subsystems (PB for ADC), turn on AFIO clocking enable EXTI & so on
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPCEN | RCC_APB2ENR_AFIOEN;
    // turn off JTAG (PA15, PB3)
    AFIO->MAPR = AFIO_MAPR_SWJ_CFG_JTAGDISABLE;
    // AFIO->MAPR = AFIO_MAPR_SWJ_CFG_DISABLE;
// PORTA
    // pullups: PA1 - PPS, PA15 - USB pullup
    GPIOA->ODR = (1<<1)|(1<<15);
    // PPS pin (PA1) - input with weak pullup, PA6 - SCLK of LED screen
    GPIOA->CRL = CRL(1, CNF_PUDINPUT|MODE_INPUT) | CRL(6, CNF_PPOUTPUT|MODE_SLOW);
    EXTI->IMR = EXTI_IMR_MR1; // mask PA1
    // interrupt on pulse front: buttons - 1->0, PPS - 0->1
    EXTI->RTSR = EXTI_RTSR_TR1; // rising trigger for PPS
    NVIC_EnableIRQ(EXTI1_IRQn); // enable PPS interrupt
    // Set USB pullup (PA15) - opendrain output
    GPIOA->CRH = CRH(15, CNF_ODOUTPUT|MODE_SLOW);
// PORTB
    // Set leds (PB8/9) as opendrain output
    GPIOB->CRH = CRH(8, CNF_ODOUTPUT|MODE_SLOW) | CRH(9, CNF_ODOUTPUT|MODE_SLOW);
    // TRIGGERS: PB0,1,3; SCREEN pins: A,B - PB6,PB7;
    GPIOB->CRL = CRL(0, CNF_PUDINPUT|MODE_INPUT) | CRL(1, CNF_PUDINPUT|MODE_INPUT) | CRL(3, CNF_PUDINPUT|MODE_INPUT) |
            CRL(6, CNF_PPOUTPUT|MODE_SLOW) | CRL(7, CNF_PPOUTPUT|MODE_SLOW);
// PORTC
    // buzzer (PC13): pushpull output
    GPIOC->CRH = CRH(13, CNF_PPOUTPUT|MODE_SLOW);
    // exti: PB0, 3; PA1
    AFIO->EXTICR[0] = AFIO_EXTICR1_EXTI0_PB | AFIO_EXTICR1_EXTI1_PA | AFIO_EXTICR1_EXTI3_PB;
    // PB0/1/3 - triggers
    for(int i = 0; i < DIGTRIG_AMOUNT; ++i){
        uint16_t pin = trigpin[i];
        if(pin == 1<<1) continue; // omit PB1
        // fill trigstate array
        uint8_t trgs = (the_conf.trigstate & (1<<i)) ? 1 : 0;
        trigstate[i] = trgs;
        trigport[i]->ODR |= pin; // turn on pullups
        if(i != 1){ // turn interrupts on
            EXTI->IMR |= pin;
            if(trgs){ // triggered @1 -> rising interrupt
                EXTI->RTSR |= pin;
            }else{ // falling interrupt
                EXTI->FTSR |= pin;
            }
        }
    }
    // ---------------------> config-depengent block, interrupts & pullup inputs:
    // !!! change AFIO_EXTICRx if some triggers not @GPIOA
    NVIC_EnableIRQ(EXTI0_IRQn); // PB0
    NVIC_EnableIRQ(EXTI3_IRQn); // PB3
    // <---------------------
}

static inline void adc_setup(){
    GPIOB->CRL |= CRL(0, CNF_ANALOG|MODE_INPUT);
    uint32_t ctr = 0;
    // Enable clocking
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
    RCC->CFGR &= ~(RCC_CFGR_ADCPRE);
    RCC->CFGR |= RCC_CFGR_ADCPRE_DIV8; // ADC clock = RCC / 8
    // sampling time - 239.5 cycles for channels 16 and 17
    ADC1->SMPR1 = ADC_SMPR1_SMP16 | ADC_SMPR1_SMP17;
    // we have three conversions in group -> ADC1->SQR1[L] = 2, order: 16->17
    // Tsens == 16, Vdd == 17
    ADC1->SQR3 = (16<<0) | (17<<5);
    ADC1->SQR1 = ADC_SQR1_L_0; // 2 conversions
    ADC1->CR1 |= ADC_CR1_SCAN; // scan mode
    // DMA configuration
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;
    DMA1_Channel1->CPAR = (uint32_t) (&(ADC1->DR));
    DMA1_Channel1->CMAR = (uint32_t)(ADC_array);
    DMA1_Channel1->CNDTR = NUMBER_OF_ADC_CHANNELS * 9;
    DMA1_Channel1->CCR |= DMA_CCR_MINC | DMA_CCR_MSIZE_0 | DMA_CCR_PSIZE_0
                          | DMA_CCR_CIRC | DMA_CCR_PL | DMA_CCR_EN;
    // continuous mode & DMA; enable vref & Tsens; wake up ADC
    ADC1->CR2 |= ADC_CR2_DMA | ADC_CR2_TSVREFE | ADC_CR2_CONT | ADC_CR2_ADON;
    // wait for Tstab - at least 1us
    while(++ctr < 0xff) nop();
    // calibration
    ADC1->CR2 |= ADC_CR2_RSTCAL;
    ctr = 0; while((ADC1->CR2 & ADC_CR2_RSTCAL) && ++ctr < 0xfffff);
    ADC1->CR2 |= ADC_CR2_CAL;
    ctr = 0; while((ADC1->CR2 & ADC_CR2_CAL) && ++ctr < 0xfffff);
    // turn ON ADC
    ADC1->CR2 |= ADC_CR2_ADON;
}

void hw_setup(){
    gpio_setup();
    adc_setup();
}

static trigtime trgtm;
void savetrigtime(){
    trgtm.millis = Timer;
    memcpy(&trgtm.Time, &current_time, sizeof(curtime));
}

/**
 * @brief fillshotms - save trigger shot time
 * @param i - trigger number
 */
void fillshotms(int i){
    if(i < 0 || i >= TRIGGERS_AMOUNT) return;
    if(Tms - shotms[i] > (uint32_t)the_conf.trigpause[i] || i == LIDAR_TRIGGER){
        memcpy(&shottime[i], &trgtm, sizeof(trigtime));
        shotms[i] = Tms;
        trigger_shot |= 1<<i;
        BUZZER_ON();
    }
}

/**
 * @brief fillunshotms - calculate trigger length time
 */
void fillunshotms(){
    if(!trigger_shot) return;
    uint8_t X = 1;
    for(int i = 0; i < TRIGGERS_AMOUNT; ++i, X<<=1){
        IWDG->KR = IWDG_REFRESH;
        // check whether trigger is OFF but shot recently
        if(trigger_shot & X){
            uint32_t len = Tms - shotms[i];
            uint8_t rdy = 0;
            if(len > MAX_TRIG_LEN){
                triglen[i] = -1;
                rdy = 1;
            }else triglen[i] = (int16_t) len;
            if(i == LIDAR_TRIGGER){
                if(!parse_lidar_data(NULL)) rdy = 1;
            }else{
                uint8_t pinval = (trigport[i]->IDR & trigpin[i]) ? 1 : 0;
                if(pinval != trigstate[i]) rdy = 1; // trigger is OFF
            }
            if(rdy){
                if(i != LIDAR_TRIGGER) shotms[i] = Tms;
                show_trigger_shot(X);
                trigger_shot &= ~X;
                // now trigger data is in lastLog -> store it in flash
                if(the_conf.defflags & FLAG_SAVE_EVENTS){
                    if(store_log(&lastLog)) sendstring("\n\nError saving event!\n\n");
                }
            }
        }
    }
}

void exti0_isr(){ // PB0 - trig0
    EXTI->PR = EXTI_PR_PR0;
    if(!chkshtr) return;
    savetrigtime();
    fillshotms(0);
}

void exti1_isr(){ // PPS - PA1
    systick_correction();
    LED_off(); // turn off LED0 @ each PPS
    EXTI->PR = EXTI_PR_PR1;
}

void chkTrig1(){
    // ================> HARD HARDWARE DEPENDENT: PB1 trigger
    if(!chkshtr) return;
    static uint8_t oldstate = 0;
    uint8_t curstate = GPIOB->IDR & 2;
    if(Tms < 100){ // don't mind for first 100ms
        oldstate = curstate;
        return;
    }
    if(curstate == oldstate){ // state didn't changed
        return;
    }
    oldstate = curstate;
    if(curstate != (the_conf.trigstate & 2)) return; // relax from trigger
    savetrigtime();
    fillshotms(1);
    // <================
}

void exti3_isr(){ // PB3 - trig2
    EXTI->PR = EXTI_PR_PR3;
    if(!chkshtr) return;
    savetrigtime();
    fillshotms(2);
}

/**
 * @brief gettrig - get trigger state
 * @return 1 if trigger active or 0
 */
uint8_t gettrig(uint8_t N){
    if(N >= TRIGGERS_AMOUNT) return 0;
    uint8_t curval = (trigport[N]->IDR & trigpin[N]) ? 1 : 0;
    if(curval == trigstate[N]) return 1;
    else return 0;
}

static uint32_t Ton = 0; // Time of first buzzer check
void chk_buzzer(){
    if(!BUZZER_GET()) return; // buzzer if OFF
    if(!trigger_shot){ // should we turn off buzzer?
        uint8_t notrg = 1;
        for(int i = 0; i < DIGTRIG_AMOUNT; ++i){
            uint8_t curval = (trigport[i]->IDR & trigpin[i]) ? 1 : 0;
            if(curval == trigstate[i]){
                notrg = 0; // cheep while digital trigger is ON
                break;
            }
        }
        if(notrg){ // turn off buzzer when there's no trigger events & timeout came
            if(Tms - Ton < BUZZER_CHEEP_TIME) return;
            Ton = 0;
            BUZZER_OFF();
        }
    }else{ // buzzer is ON - check timer
        if(Ton == 0){
            Ton = Tms;
            if(!Ton) Ton = 1;
        }
    }
}

// make a short squeak
void buzzer_squeak(){
    Ton = Tms;
    if(!Ton) Ton = 1;
    BUZZER_ON();
}
