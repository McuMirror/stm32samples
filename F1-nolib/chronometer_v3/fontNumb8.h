/*
 * This file is part of the LED_screen project.
 * Copyright 2019 Edward V. Emelianov <eddy@sao.ru, edward.emelianoff@gmail.com>
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

// this file should be included JUST ONCE!
// only in fonts.c

#define FONTNUMB8BYTES         8
#define FONTNUMB8HEIGHT        8
#define FONTNUMB8BASELINE      0

// this array shows position of every koi8-r symbol in font10_table
const uint8_t fontNumb8_encoding[256] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 0..31
      1,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, // 47
      2,   3,   4,   5,   6,   7,   8,   9,  10,  11,   0,   0,   0,   0,   0,   0, // 63
      0,  12,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  13, // 79
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, // 95
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, // 111
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, // 127
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, // 143
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, // 159
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, // 175
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, // 191
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, // 207
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, // 223
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, // 239
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, // 255
};

const uint8_t fontNumb8_table[] = {
    // 0 0x00 - empty
        0,
        ________,
        ________,
        ________,
        ________,
        ________,
        ________,
        ________,
        ________,
    // 1 0x20 - ' '
        4,
        ________,
        ________,
        ________,
        ________,
        ________,
        ________,
        ________,
        ________,
    // 2 0x30 - '0'
        6,
        _XXX____,
        XX_XX___,
        X___X___,
        X___X___,
        X___X___,
        X___X___,
        XX_XX___,
        _XXX____,
    // 3 0x31 - '1'
        6,
        _XX_____,
        X_X_____,
        __X_____,
        __X_____,
        __X_____,
        __X_____,
        __X_____,
        XXXXX___,
    // 4 0x32 - '2'
        6,
        _XXX____,
        X___X___,
        ____X___,
        ____X___,
        ___X____,
        __X_____,
        _X______,
        XXXXX___,
    // 5 0x33 - '3'
        6,
        XXXX____,
        ____X___,
        ____X___,
        _XXX____,
        ____X___,
        ____X___,
        ____X___,
        XXXX____,
    // 6 0x34 - '4'
        7,
        ___XX___,
        ___XX___,
        __X_X___,
        _X__X___,
        _X__X___,
        XXXXXX__,
        ____X___,
        ____X___,
    // 7 0x35 - '5'
        5,
        XXXX____,
        X_______,
        X_______,
        XXX_____,
        ___X____,
        ___X____,
        ___X____,
        XXX_____,
    // 8 0x36 - '6'
        6,
        _XXX____,
        X___X___,
        X_______,
        XXXX____,
        X___X___,
        X___X___,
        X___X___,
        _XXX____,
    // 9 0x37 - '7'
        5,
        XXXX____,
        ___X____,
        __X_____,
        __X_____,
        __X_____,
        _X______,
        _X______,
        _X______,
    // 10 0x38 - '8'
        6,
        _XXX____,
        X___X___,
        X___X___,
        _XXX____,
        X___X___,
        X___X___,
        X___X___,
        _XXX____,
    // 11 0x39 - '9'
        6,
        _XXX____,
        X___X___,
        X___X___,
        X___X___,
        _XXXX___,
        ____X___,
        X___X___,
        _XXX____,
    // 12 0x41 - 'A'
        6,
        __X_____,
        _X_X____,
        _X_X____,
        _X_X____,
        X___X___,
        XXXXX___,
        X___X___,
        X___X___,
    // 13 0x4f - 'O'
        7,
        __XX____,
        _X__X___,
        X____X__,
        X____X__,
        X____X__,
        X____X__,
        _X__X___,
        __XX____,
};

/*
32 (0x20) -     64 (0x40) - @   96 (0x60) - `  128 (0x80) - �  160 (0xa0) - �  192 (0xc0) - �  224 (0xe0) - �
33 (0x21) - !   65 (0x41) - A   97 (0x61) - a  129 (0x81) - �  161 (0xa1) - �  193 (0xc1) - �  225 (0xe1) - �
34 (0x22) - "   66 (0x42) - B   98 (0x62) - b  130 (0x82) - �  162 (0xa2) - �  194 (0xc2) - �  226 (0xe2) - �
35 (0x23) - #   67 (0x43) - C   99 (0x63) - c  131 (0x83) - �  163 (0xa3) - �  195 (0xc3) - �  227 (0xe3) - �
36 (0x24) - $   68 (0x44) - D  100 (0x64) - d  132 (0x84) - �  164 (0xa4) - �  196 (0xc4) - �  228 (0xe4) - �
37 (0x25) - %   69 (0x45) - E  101 (0x65) - e  133 (0x85) - �  165 (0xa5) - �  197 (0xc5) - �  229 (0xe5) - �
38 (0x26) - &   70 (0x46) - F  102 (0x66) - f  134 (0x86) - �  166 (0xa6) - �  198 (0xc6) - �  230 (0xe6) - �
39 (0x27) - '   71 (0x47) - G  103 (0x67) - g  135 (0x87) - �  167 (0xa7) - �  199 (0xc7) - �  231 (0xe7) - �
40 (0x28) - (   72 (0x48) - H  104 (0x68) - h  136 (0x88) - �  168 (0xa8) - �  200 (0xc8) - �  232 (0xe8) - �
41 (0x29) - )   73 (0x49) - I  105 (0x69) - i  137 (0x89) - �  169 (0xa9) - �  201 (0xc9) - �  233 (0xe9) - �
42 (0x2a) - *   74 (0x4a) - J  106 (0x6a) - j  138 (0x8a) - �  170 (0xaa) - �  202 (0xca) - �  234 (0xea) - �
43 (0x2b) - +   75 (0x4b) - K  107 (0x6b) - k  139 (0x8b) - �  171 (0xab) - �  203 (0xcb) - �  235 (0xeb) - �
44 (0x2c) - ,   76 (0x4c) - L  108 (0x6c) - l  140 (0x8c) - �  172 (0xac) - �  204 (0xcc) - �  236 (0xec) - �
45 (0x2d) - -   77 (0x4d) - M  109 (0x6d) - m  141 (0x8d) - �  173 (0xad) - �  205 (0xcd) - �  237 (0xed) - �
46 (0x2e) - .   78 (0x4e) - N  110 (0x6e) - n  142 (0x8e) - �  174 (0xae) - �  206 (0xce) - �  238 (0xee) - �
47 (0x2f) - /   79 (0x4f) - O  111 (0x6f) - o  143 (0x8f) - �  175 (0xaf) - �  207 (0xcf) - �  239 (0xef) - �
48 (0x30) - 0   80 (0x50) - P  112 (0x70) - p  144 (0x90) - �  176 (0xb0) - �  208 (0xd0) - �  240 (0xf0) - �
49 (0x31) - 1   81 (0x51) - Q  113 (0x71) - q  145 (0x91) - �  177 (0xb1) - �  209 (0xd1) - �  241 (0xf1) - �
50 (0x32) - 2   82 (0x52) - R  114 (0x72) - r  146 (0x92) - �  178 (0xb2) - �  210 (0xd2) - �  242 (0xf2) - �
51 (0x33) - 3   83 (0x53) - S  115 (0x73) - s  147 (0x93) - �  179 (0xb3) - �  211 (0xd3) - �  243 (0xf3) - �
52 (0x34) - 4   84 (0x54) - T  116 (0x74) - t  148 (0x94) - �  180 (0xb4) - �  212 (0xd4) - �  244 (0xf4) - �
53 (0x35) - 5   85 (0x55) - U  117 (0x75) - u  149 (0x95) - �  181 (0xb5) - �  213 (0xd5) - �  245 (0xf5) - �
54 (0x36) - 6   86 (0x56) - V  118 (0x76) - v  150 (0x96) - �  182 (0xb6) - �  214 (0xd6) - �  246 (0xf6) - �
55 (0x37) - 7   87 (0x57) - W  119 (0x77) - w  151 (0x97) - �  183 (0xb7) - �  215 (0xd7) - �  247 (0xf7) - �
56 (0x38) - 8   88 (0x58) - X  120 (0x78) - x  152 (0x98) - �  184 (0xb8) - �  216 (0xd8) - �  248 (0xf8) - �
57 (0x39) - 9   89 (0x59) - Y  121 (0x79) - y  153 (0x99) - �  185 (0xb9) - �  217 (0xd9) - �  249 (0xf9) - �
58 (0x3a) - :   90 (0x5a) - Z  122 (0x7a) - z  154 (0x9a) -    186 (0xba) - �  218 (0xda) - �  250 (0xfa) - �
59 (0x3b) - ;   91 (0x5b) - [  123 (0x7b) - {  155 (0x9b) - �  187 (0xbb) - �  219 (0xdb) - �  251 (0xfb) - �
60 (0x3c) - <   92 (0x5c) - \  124 (0x7c) - |  156 (0x9c) - �  188 (0xbc) - �  220 (0xdc) - �  252 (0xfc) - �
61 (0x3d) - =   93 (0x5d) - ]  125 (0x7d) - }  157 (0x9d) - �  189 (0xbd) - �  221 (0xdd) - �  253 (0xfd) - �
62 (0x3e) - >   94 (0x5e) - ^  126 (0x7e) - ~  158 (0x9e) - �  190 (0xbe) - �  222 (0xde) - �  254 (0xfe) - �
63 (0x3f) - ?   95 (0x5f) - _  127 (0x7f) -    159 (0x9f) - �  191 (0xbf) - �  223 (0xdf) - �  255 (0xff) - �
*/
