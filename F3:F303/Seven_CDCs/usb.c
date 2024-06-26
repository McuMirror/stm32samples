/*
 * This file is part of the SevenCDCs project.
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

#include <string.h>

#include "hardware.h"
#include "strfunc.h"
#include "usb.h"
#include "usb_lib.h"

static volatile uint8_t usbbuff[USB_TXBUFSZ]; // temporary buffer for sending data
// ring buffers for incoming and outgoing data
static uint8_t obuf[MAX_IDX][RBOUTSZ], ibuf[MAX_IDX][RBINSZ];
#define OBUF(N)  {.data = obuf[N], .length = RBOUTSZ, .head = 0, .tail = 0}
volatile ringbuffer rbout[MAX_IDX] = {OBUF(0), OBUF(1), OBUF(2), OBUF(3), OBUF(4), OBUF(5), OBUF(6)};
#define IBUF(N)  {.data = ibuf[N], .length = RBINSZ, .head = 0, .tail = 0}
volatile ringbuffer rbin[MAX_IDX] = {IBUF(0), IBUF(1), IBUF(2), IBUF(3), IBUF(4), IBUF(5), IBUF(6)};
// transmission is succesfull
volatile uint8_t bufisempty[MAX_IDX] = {1,1,1,1,1,1,1};
volatile uint8_t bufovrfl[MAX_IDX] = {0};

// here and later: ifNo is index of buffers, i.e. ifNo = epno-1 !!!
void send_next(int ifNo){
    if(bufisempty[ifNo]) return;
    static uint8_t lastdsz[MAX_EPNO] = {0};
    int buflen = RB_read((ringbuffer*)&rbout[ifNo], (uint8_t*)usbbuff, USB_TXBUFSZ);
    if(!buflen){
        if(lastdsz[ifNo] == USB_TXBUFSZ) EP_Write(ifNo+1, NULL, 0); // send ZLP after 64 bits packet when nothing more to send
        lastdsz[ifNo] = 0;
        bufisempty[ifNo] = 1;
        return;
    }
    EP_Write(ifNo+1, (uint8_t*)usbbuff, buflen);
    lastdsz[ifNo] = buflen;
}

// blocking send full content of ring buffer
int USB_sendall(int ifNo){
    while(!bufisempty[ifNo]){
        if(!USBON(ifNo)) return 0;
    }
    return 1;
}

// put `buf` into queue to send
int USB_send(int ifNo, const uint8_t *buf, int len){
    if(!buf || !USBON(ifNo) || !len) return 0;
    uint32_t T = Tms;
    while(len){
        int a = RB_write((ringbuffer*)&rbout[ifNo], buf, len);
        if(a == 0 && Tms - T > 5){ // buffer overflow && timeout -> interface disconnected
            usbON &= ~(1<<ifNo);
            bufisempty[ifNo] = 0;
            RB_clearbuf((ringbuffer*)&rbout[ifNo]);
            return 0; // timeout - interface is down
        }
        len -= a;
        buf += a;
        if(bufisempty[ifNo]){
            bufisempty[ifNo] = 0;
            send_next(ifNo);
        }
    }
    return 1;
}

int USB_putbyte(int ifNo, uint8_t byte){
    if(!USBON(ifNo)) return 0;
    while(0 == RB_write((ringbuffer*)&rbout[ifNo], &byte, 1)) send_next(ifNo);
    if(bufisempty[ifNo]){
        bufisempty[ifNo] = 0;
        send_next(ifNo);
    }
    return 1;
}

int USB_sendstr(int ifNo, const char *string){
    if(!string) return 0;
    return USB_send(ifNo, (const uint8_t*)string, mystrlen(string));
}

/**
 * @brief USB_receive - get binary data from receiving ring-buffer
 * @param buf (i) - buffer for received data
 * @param len - length of `buf`
 * @return amount of received bytes (negative, if overfull happened)
 */
int USB_receive(int ifNo, uint8_t *buf, int len){
    if(bufovrfl[ifNo]){
        RB_clearbuf((ringbuffer*)&rbin[ifNo]);
        bufovrfl[ifNo] = 0;
        return -1;
    }
    int sz = RB_read((ringbuffer*)&rbin[ifNo], buf, len);
    return sz;
}

/**
 * @brief USB_receivestr - get string up to '\n' and replace '\n' with 0
 * @param buf - receiving buffer
 * @param len - its length
 * @return strlen or negative value indicating overflow (if so, string won't be ends with 0 and buffer should be cleared)
 */
int USB_receivestr(int ifNo, char *buf, int len){
    if(bufovrfl[ifNo]){
        RB_clearbuf((ringbuffer*)&rbin[ifNo]);
        bufovrfl[ifNo] = 0;
        return -1;
    }
    int l = RB_readto((ringbuffer*)&rbin[ifNo], '\n', (uint8_t*)buf, len);
    if(l < 1) return 0;
    buf[--l] = 0; // replace '\n' with strend
    return l;
}
