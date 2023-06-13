/*
 * usart.c
 *
 * Copyright 2018 Edward V. Emelianoff <eddy@sao.ru, edward.emelianoff@gmail.com>
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
 */
#include "stm32f1.h"
#include "usart.h"

extern volatile uint32_t Tms;
static volatile int idatalen[2] = {0,0}; // received data line length (including '\n')
static volatile int odatalen[2] = {0,0};

volatile int usart_linerdy = 0,        // received data ready
    dlen = 0,           // length of data (including '\n') in current buffer
    usart_bufovr = 0,         // input buffer overfull
    usart_txrdy = 1           // transmission done
;


int rbufno = 0, tbufno = 0; // current rbuf/tbuf numbers
static char rbuf[2][UARTBUFSZI], tbuf[2][UARTBUFSZO]; // receive & transmit buffers
static char *recvdata = NULL;

/**
 * return length of received data (without trailing zero)
 */
int usart_getline(char **line){
    if(usart_bufovr){
        usart_bufovr = 0;
        usart_linerdy = 0;
        return 0;
    }
    *line = recvdata;
    usart_linerdy = 0;
    return dlen;
}

// transmit current tbuf and swap buffers
void usart_transmit(){
    uint32_t tmout = 16000000;
    while(!usart_txrdy){if(--tmout == 0) return;}; // wait for previos buffer transmission
    register int l = odatalen[tbufno];
    if(!l) return;
    usart_txrdy = 0;
    odatalen[tbufno] = 0;
    DMA1_Channel4->CCR &= ~DMA_CCR_EN;
    DMA1_Channel4->CMAR = (uint32_t) tbuf[tbufno]; // mem
    DMA1_Channel4->CNDTR = l;
    DMA1_Channel4->CCR |= DMA_CCR_EN;
    tbufno = !tbufno;
}

void usart_putchar(const char ch){
    if(odatalen[tbufno] == UARTBUFSZO) usart_transmit();
    tbuf[tbufno][odatalen[tbufno]++] = ch;
}

void usart_send(const char *str){
    uint32_t x = 512;
    while(*str && --x){
        if(odatalen[tbufno] == UARTBUFSZO) usart_transmit();
        tbuf[tbufno][odatalen[tbufno]++] = *str++;
    }
}

/*
 * USART speed: baudrate = Fck/(USARTDIV)
 * USARTDIV stored in USART->BRR
 *
 * for 72MHz USARTDIV=72000/f(kboud); so for 115200 USARTDIV=72000/115.2=625 -> BRR=0x271
 *         9600: BRR = 7500 (0x1D4C)
 */

void usart_setup(){
    uint32_t tmout = 16000000;
    // PA9 - Tx, PA10 - Rx 
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_USART1EN | RCC_APB2ENR_AFIOEN;
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;
    GPIOA->CRH |= CRH(9, CNF_AFPP|MODE_NORMAL) | CRH(10, CNF_FLINPUT|MODE_INPUT);

    // USART1 Tx DMA - Channel4 (Rx - channel 5)
    DMA1_Channel4->CPAR = (uint32_t) &USART1->DR; // periph
    DMA1_Channel4->CCR |= DMA_CCR_MINC | DMA_CCR_DIR | DMA_CCR_TCIE; // 8bit, mem++, mem->per, transcompl irq
    // Tx CNDTR set @ each transmission due to data size
    NVIC_SetPriority(DMA1_Channel4_IRQn, 3);
    NVIC_EnableIRQ(DMA1_Channel4_IRQn);
    NVIC_SetPriority(USART1_IRQn, 0);
    // setup usart1
    USART1->BRR = 72000000 / 115200;
    USART1->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_UE; // 1start,8data,nstop; enable Rx,Tx,USART
    while(!(USART1->SR & USART_SR_TC)){if(--tmout == 0) break;} // polling idle frame Transmission
    USART1->SR = 0; // clear flags
    USART1->CR1 |= USART_CR1_RXNEIE; // allow Rx IRQ
    USART1->CR3 = USART_CR3_DMAT; // enable DMA Tx
    NVIC_EnableIRQ(USART1_IRQn);
}


void usart1_isr(){
    if(USART1->SR & USART_SR_RXNE){ // RX not emty - receive next char
        uint8_t rb = USART1->DR;
        if(idatalen[rbufno] < UARTBUFSZI){ // put next char into buf
            rbuf[rbufno][idatalen[rbufno]++] = rb;
            if(rb == '\n'){ // got newline - line ready
                usart_linerdy = 1;
                dlen = idatalen[rbufno];
                recvdata = rbuf[rbufno];
                // prepare other buffer
                rbufno = !rbufno;
                idatalen[rbufno] = 0;
            }
        }else{ // buffer overrun
            usart_bufovr = 1;
            idatalen[rbufno] = 0;
        }
    }
}

void dma1_channel4_isr(){
    if(DMA1->ISR & DMA_ISR_TCIF4){ // Tx
        DMA1->IFCR = DMA_IFCR_CTCIF4; // clear TC flag
        usart_txrdy = 1;
    }
}
