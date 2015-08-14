/*
   BlinkM driver
*/
/*
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

*/

#include "BlinkM.h"

#define BLINKM_LED_BRIGHT 0xFF    // full brightness
#define BLINKM_LED_MEDIUM 0x7F    // medium brightness
#define BLINKM_LED_DIM    0x4F    // dim brightness
#define BLINKM_LED_OFF    0x00    // off

BlinkM::BlinkM() :
    RGBLed(BLINKM_LED_OFF, BLINKM_LED_BRIGHT, BLINKM_LED_MEDIUM, BLINKM_LED_DIM)
{

}
