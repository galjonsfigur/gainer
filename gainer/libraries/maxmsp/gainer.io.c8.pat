max v2;#N vpatcher 49 98 994 712;#P origin 0 15;#P window setfont "Sans Serif" 9.;#P window linecount 1;#P newex 456 271 30 196617 t b s;#P message 456 294 43 196617 T0*;#P newex 456 251 95 196617 sprintf set T%1X*;#P number 489 57 35 9 0 15 19 3 0 0 0 221 221 221 222 222 222 0 0 0;#P inlet 489 37 15 0;#P comment 172 437 18 196617 ?;#P newex 672 215 55 196617 delay 100;#P newex 629 441 40 196617 itoa;#P newex 629 461 152 196617 print gainer_firmware_version;#P message 672 242 21 196617 ?*;#P message 12 198 32 196617 print;#P newex 12 174 48 196617 sel print;#P number 765 242 35 9 1 0 1 3 0 0 0 221 221 221 222 222 222 0 0 0;#P message 743 242 20 196617 10;#P newex 743 216 32 196617 sel 0;#P newex 586 174 61 196617 delay 1500;#P message 586 195 29 196617 V0*;#P newex 659 58 79 196617 t b b b b b b;#P newex 565 222 61 196617 delay 3500;#P message 565 243 101 196617 KONFIGURATION_8*;#P newex 743 195 38 196617 int $2;#P message 698 242 21 196617 Q*;#P newex 659 24 48 196617 loadbang;#P number 688 701 35 9 0 0 0 3 0 0 0 221 221 221 222 222 222 0 0 0;#P number 650 701 35 9 0 0 0 3 0 0 0 221 221 221 222 222 222 0 0 0;#P number 612 701 35 9 0 0 0 3 0 0 0 221 221 221 222 222 222 0 0 0;#P number 574 701 35 9 0 0 0 3 0 0 0 221 221 221 222 222 222 0 0 0;#P comment 688 737 36 196617 d.in.3;#P comment 650 737 36 196617 d.in.2;#P comment 612 737 36 196617 d.in.1;#P comment 574 737 36 196617 d.in.0;#P outlet 688 720 15 0;#P outlet 650 720 15 0;#P outlet 612 720 15 0;#P outlet 574 720 15 0;#P newex 59 734 38 196617 s pack;#P newex 405 591 43 196617 r pack;#P newex 59 709 217 196617 pack 0 0 0 0 0 0 0 0;#P newex 59 667 27 196617 / 2;#P newex 59 686 27 196617 % 2;#P newex 88 646 27 196617 / 2;#P newex 88 665 27 196617 % 2;#P newex 117 625 27 196617 / 2;#P newex 117 644 27 196617 % 2;#P newex 146 605 27 196617 / 2;#P newex 146 624 27 196617 % 2;#P newex 353 77 27 196617 t b i;#P toggle 353 57 15 0;#P comment 352 22 36 196617 d.out.3;#P inlet 353 37 15 0;#P newex 278 77 27 196617 t b i;#P newex 240 77 27 196617 t b i;#P newex 316 77 27 196617 t b i;#P toggle 316 57 15 0;#P toggle 278 57 15 0;#P toggle 240 57 15 0;#P comment 315 22 36 196617 d.out.3;#P comment 278 22 36 196617 d.out.2;#P comment 241 22 36 196617 d.out.1;#P inlet 316 37 15 0;#P inlet 278 37 15 0;#P inlet 240 37 15 0;#P newex 90 367 38 196617 sel 81;#P comment 154 438 18 196617 Q;#P comment 12 23 36 196617 start;#P newex 175 586 27 196617 / 2;#P newex 204 563 27 196617 / 2;#P newex 233 540 27 196617 / 2;#P newex 175 605 27 196617 % 2;#P newex 204 585 27 196617 % 2;#P newex 233 562 27 196617 % 2;#P newex 262 540 27 196617 % 2;#P toggle 402 57 15 0;#P comment 403 22 76 196617 din start/stop;#P inlet 402 37 15 0;#P message 402 286 21 196617 R*;#P newex 405 613 112 196617 unpack 0 0 0 0 0 0 0 0;#P newex 90 272 30 196617 t b s;#P message 90 295 71 196617 D00000000*;#P comment 103 438 18 196617 F;#P comment 82 438 18 196617 N;#P newex 175 517 70 196617 sprintf 0x%s;#P window linecount 2;#P newex 175 496 110 196617 sprintf %0c%0c%0c%0c;#P window linecount 1;#P newex 175 475 110 196617 unpack 0 0 0 0;#P newex 175 454 55 196617 zl group 4;#P number 536 701 35 9 0 0 0 3 0 0 0 221 221 221 222 222 222 0 0 0;#P number 498 701 35 9 0 0 0 3 0 0 0 221 221 221 222 222 222 0 0 0;#P number 460 701 35 9 0 0 0 3 0 0 0 221 221 221 222 222 222 0 0 0;#P number 422 701 35 9 0 0 0 3 0 0 0 221 221 221 222 222 222 0 0 0;#P newex 422 677 282 196617 gate 8;#N counter 0 1 8;#X flags 0 0;#P newobj 422 656 71 196617 counter 0 1 8;#P newex 405 637 27 196617 t i b;#P comment 536 737 36 196617 d.in.3;#P comment 498 737 36 196617 d.in.2;#P comment 460 737 36 196617 d.in.1;#P comment 422 737 36 196617 d.in.0;#P outlet 536 720 15 0;#P outlet 498 720 15 0;#P outlet 460 720 15 0;#P outlet 422 720 15 0;#P comment 132 438 18 196617 I;#P newex 76 417 124 196617 route 78 70 82 73 81 63;#P newex 76 396 67 196617 zl group 100;#P newex 51 367 35 196617 sel 42;#P comment 117 438 18 196617 R;#P newex 166 77 27 196617 t b i;#P newex 128 77 27 196617 t b i;#P newex 204 77 27 196617 t b i;#P user radiogroup 90 122 18 128;#X size 8;#X offset 16;#X inactive 0;#X itemtype 1;#X flagmode 1;#X set 0 0 0 0 0 0 0 0;#X done;#P newex 90 252 101 196617 sprintf set D%08X*;#P newex 90 101 293 196617 pack 0 0 0 0 0 0 0 0;#P toggle 204 57 15 0;#P toggle 166 57 15 0;#P toggle 128 57 15 0;#P toggle 90 57 15 0;#P toggle 32 295 15 0;#P comment 203 22 36 196617 d.out.3;#P comment 166 22 36 196617 d.out.2;#P comment 129 22 36 196617 d.out.1;#P newex 90 320 31 196617 spell;#P newex 180 366 62 196617 print status;#P newex 180 345 53 196617 print data;#P newex 90 344 85 196617 serial $1 38400;#P newex 32 321 52 196617 metro 10;#P inlet 204 37 15 0;#P inlet 166 37 15 0;#P inlet 128 37 15 0;#P inlet 90 37 15 0;#P inlet 12 39 15 0;#P comment 90 22 36 196617 d.out.0;#P comment 489 22 59 196617 sensitivity;#P connect 2 0 119 0;#P connect 119 0 120 0;#P fasten 113 4 15 0 716 315 37 315;#P connect 15 0 7 0;#P fasten 8 0 27 0 95 364 56 364;#P fasten 90 0 92 0 93 664 64 664;#P connect 92 0 91 0;#P connect 91 0 93 0;#P connect 93 0 95 0;#P fasten 118 0 7 1 770 315 79 315;#P fasten 117 0 7 1 748 315 79 315;#P fasten 27 0 28 0 56 389 81 389;#P connect 27 1 28 0;#P connect 28 0 29 0;#P fasten 88 0 90 0 122 643 93 643;#P connect 90 0 89 0;#P connect 89 0 93 1;#P connect 3 0 16 0;#P fasten 24 0 20 0 133 96 95 96;#P fasten 23 0 20 0 209 96 95 96;#P fasten 25 0 20 0 171 96 95 96;#P fasten 16 0 20 0 95 85 95 85;#P fasten 84 0 20 0 358 96 95 96;#P fasten 78 0 20 0 321 96 95 96;#P fasten 80 0 20 0 283 96 95 96;#P fasten 79 0 20 0 245 96 95 96;#P connect 20 0 22 0;#P connect 22 0 21 0;#P connect 21 0 53 0;#P connect 53 0 52 0;#P fasten 53 1 52 0 115 292 95 292;#P fasten 129 0 11 0 461 315 95 315;#P fasten 121 0 11 0 677 315 95 315;#P connect 52 0 11 0;#P fasten 55 0 11 0 407 315 95 315;#P fasten 111 0 11 0 570 315 95 315;#P fasten 114 0 11 0 591 315 95 315;#P fasten 109 0 11 0 703 315 95 315;#P fasten 11 0 8 0 95 329 95 329;#P fasten 120 0 8 0 17 343 95 343;#P fasten 7 0 8 0 37 340 95 340;#P connect 8 0 68 0;#P fasten 86 0 88 0 151 623 122 623;#P connect 88 0 87 0;#P connect 87 0 93 2;#P connect 4 0 17 0;#P connect 17 0 24 0;#P fasten 24 1 20 1 150 96 135 96;#P fasten 65 0 86 0 180 604 151 604;#P connect 86 0 85 0;#P connect 85 0 93 3;#P connect 5 0 18 0;#P connect 18 0 25 0;#P fasten 25 1 20 2 188 96 175 96;#P fasten 29 2 46 0 119 452 180 452;#P connect 46 0 47 0;#P connect 47 0 48 0;#P connect 48 0 49 0;#P fasten 64 0 65 0 209 583 180 583;#P connect 65 0 62 0;#P connect 62 0 93 4;#P connect 6 0 19 0;#P connect 19 0 23 0;#P fasten 63 0 64 0 238 560 209 560;#P connect 64 0 61 0;#P connect 61 0 93 5;#P connect 47 1 48 1;#P fasten 23 1 20 3 226 96 215 96;#P fasten 49 0 63 0 180 535 238 535;#P connect 63 0 60 0;#P connect 60 0 93 6;#P connect 69 0 75 0;#P connect 75 0 79 0;#P connect 47 2 48 2;#P fasten 79 1 20 4 262 97 255 97;#P fasten 49 0 59 0 180 535 267 535;#P connect 59 0 93 7;#P connect 47 3 48 3;#P connect 70 0 76 0;#P connect 76 0 80 0;#P fasten 80 1 20 5 300 97 295 97;#P connect 71 0 77 0;#P connect 77 0 78 0;#P fasten 78 1 20 6 338 97 335 97;#P connect 81 0 83 0;#P connect 83 0 84 0;#P connect 84 1 20 7;#P connect 56 0 58 0;#P connect 58 0 55 0;#P connect 94 0 54 0;#P fasten 54 0 39 0 410 633 410 633;#P fasten 54 1 39 0 424 633 410 633;#P fasten 54 2 39 0 438 633 410 633;#P fasten 54 3 39 0 452 633 410 633;#P fasten 54 4 39 0 466 633 410 633;#P fasten 54 5 39 0 480 633 410 633;#P fasten 54 6 39 0 494 633 410 633;#P fasten 54 7 39 0 508 633 410 633;#P connect 39 1 40 0;#P connect 40 0 41 0;#P connect 41 0 42 0;#P connect 42 0 31 0;#P fasten 127 0 128 0 494 161 461 161;#P connect 128 0 130 0;#P fasten 130 1 129 0 481 291 461 291;#P connect 130 0 129 0;#P connect 41 1 43 0;#P connect 43 0 32 0;#P connect 126 0 127 0;#P connect 41 2 44 0;#P connect 44 0 33 0;#P connect 41 3 45 0;#P connect 45 0 34 0;#P fasten 113 0 112 0 664 148 570 148;#P connect 112 0 111 0;#P connect 41 4 104 0;#P connect 104 0 96 0;#P fasten 113 1 115 0 677 156 591 156;#P connect 115 0 114 0;#P connect 41 5 105 0;#P connect 105 0 97 0;#P fasten 29 5 123 0 176 435 634 435;#P connect 123 0 122 0;#P connect 41 6 106 0;#P connect 106 0 98 0;#P connect 108 0 113 0;#P fasten 113 2 124 0 690 162 677 162;#P connect 124 0 121 0;#P connect 41 7 107 0;#P connect 107 0 99 0;#P fasten 39 0 41 1 410 675 699 675;#P connect 113 3 109 0;#P fasten 113 5 110 0 729 135 748 135;#P connect 110 0 116 0;#P connect 116 0 117 0;#P connect 116 1 118 0;#P pop;