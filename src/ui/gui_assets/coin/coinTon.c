#if defined(LV_LVGL_H_INCLUDE_SIMPLE)
#include "lvgl.h"
#else
#include "../lvgl/lvgl.h"
#endif

#ifndef LV_ATTRIBUTE_MEM_ALIGN
#define LV_ATTRIBUTE_MEM_ALIGN
#endif

#ifndef LV_ATTRIBUTE_IMG_COINTON
#define LV_ATTRIBUTE_IMG_COINTON
#endif

const LV_ATTRIBUTE_MEM_ALIGN LV_ATTRIBUTE_LARGE_CONST LV_ATTRIBUTE_IMG_COINTON uint8_t coinTon_map[] = {
#if LV_COLOR_DEPTH == 16 && LV_COLOR_16_SWAP != 0
  /*Pixel format:  Blue: 5 bit Green: 6 bit, Red: 5 bit, Alpha 8 bit  BUT the 2  color bytes are swapped*/
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x1E, 0x10, 0x04, 0xDD, 0x20, 0x04, 0xDD, 0x20, 0x05, 0x1E, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0xDD, 0x40, 0x04, 0xDD, 0x8F, 0x04, 0xDD, 0xDF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xCF, 0x04, 0xDD, 0x9F, 0x04, 0xDE, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0xDE, 0x20, 0x04, 0xDD, 0xBF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0x9F, 0x04, 0xDE, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0xDD, 0x40, 0x04, 0xDD, 0xEF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xDF, 0x04, 0xDD, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0xBE, 0x30, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0xDE, 0x20, 0x04, 0xDD, 0xDF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xEF, 0x04, 0xDE, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0xDD, 0x9F, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xBF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x04, 0xDE, 0x20, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x65, 0xFE, 0xFF, 0x86, 0x7E, 0xFF, 0x86, 0x7E, 0xFF, 0x86, 0x7E, 0xFF, 0x86, 0x7E, 0xFF, 0x86, 0x7E, 0xFF, 0x86, 0x7E, 0xFF, 0x86, 0x7E, 0xFF, 0x86, 0x7E, 0xFF, 0x65, 0xFE, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0x40, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x04, 0xDD, 0x9F, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0xA6, 0xDF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xB7, 0x1F, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0x90, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x04, 0xDD, 0xCF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0xE7, 0x9F, 0xFF, 0xA6, 0xDF, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0xC7, 0x3F, 0xFF, 0xC7, 0x3F, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0xB7, 0x1F, 0xFF, 0xE7, 0x9F, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xDF, 0x00, 0x00, 0x00, 
  0x05, 0x1E, 0x10, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0xA6, 0xDF, 0xFF, 0xF7, 0xDF, 0xFF, 0x15, 0x1D, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0xC7, 0x3F, 0xFF, 0xC7, 0x3F, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x25, 0x3E, 0xFF, 0xE7, 0x9F, 0xFF, 0xA6, 0xDF, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x05, 0x1E, 0x10, 
  0x04, 0xDD, 0x20, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x14, 0xFD, 0xFF, 0xF7, 0xDF, 0xFF, 0x96, 0x9F, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0xC7, 0x3F, 0xFF, 0xC7, 0x3F, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0xA6, 0xDF, 0xFF, 0xF7, 0xDF, 0xFF, 0x25, 0x3E, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0x20, 
  0x04, 0xDD, 0x20, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x86, 0x7E, 0xFF, 0xFF, 0xFF, 0xFF, 0x35, 0x7E, 0xFF, 0x04, 0xDD, 0xFF, 0xC7, 0x3F, 0xFF, 0xC7, 0x3F, 0xFF, 0x04, 0xDD, 0xFF, 0x25, 0x3E, 0xFF, 0xFF, 0xFF, 0xFF, 0x86, 0x7E, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0x20, 
  0x05, 0x1E, 0x10, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x14, 0xFD, 0xFF, 0xE7, 0x9F, 0xFF, 0xC7, 0x3F, 0xFF, 0x04, 0xDD, 0xFF, 0xC7, 0x3F, 0xFF, 0xC7, 0x3F, 0xFF, 0x04, 0xDD, 0xFF, 0xA6, 0xDF, 0xFF, 0xE7, 0x9F, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x05, 0x1E, 0x10, 
  0x00, 0x00, 0x00, 0x04, 0xDD, 0xDF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x45, 0x9E, 0xFF, 0xFF, 0xFF, 0xFF, 0x45, 0x9E, 0xFF, 0xC7, 0x3F, 0xFF, 0xC7, 0x3F, 0xFF, 0x45, 0x9E, 0xFF, 0xFF, 0xFF, 0xFF, 0x66, 0x1E, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xCF, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x04, 0xDD, 0x90, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0xC7, 0x3F, 0xFF, 0xC7, 0x3F, 0xFF, 0xC7, 0x3F, 0xFF, 0xC7, 0x3F, 0xFF, 0xE7, 0x9F, 0xFF, 0xC7, 0x3F, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0x9F, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x04, 0xDD, 0x40, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x45, 0x9E, 0xFF, 0xFF, 0xFF, 0xFF, 0xF7, 0xDF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x25, 0x3E, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0x20, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0xDD, 0xBF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x96, 0x9F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xA6, 0xDF, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0x9F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0xDD, 0x20, 0x04, 0xDD, 0xEF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x14, 0xFD, 0xFF, 0xF7, 0xDF, 0xFF, 0xE7, 0x9F, 0xFF, 0x25, 0x3E, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xDF, 0x04, 0xDD, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0xDD, 0x40, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x35, 0x7E, 0xFF, 0x45, 0x9E, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xBD, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0xBD, 0x30, 0x04, 0xDD, 0xDF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xEF, 0x04, 0xDD, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0xDD, 0x20, 0x04, 0xDD, 0xA0, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xBF, 0x04, 0xDD, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0xDD, 0x20, 0x04, 0xDD, 0xA0, 0x04, 0xDD, 0xCF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xFF, 0x04, 0xDD, 0xDF, 0x04, 0xDD, 0x90, 0x04, 0xDD, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x1C, 0x10, 0x04, 0xDD, 0x20, 0x04, 0xDD, 0x20, 0x05, 0x1C, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
#endif
};

const lv_img_dsc_t coinTon = {
  .header.always_zero = 0,
  .header.w = 24,
  .header.h = 24,
  .data_size = 576 * LV_IMG_PX_SIZE_ALPHA_BYTE,
  .header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA,
  .data = coinTon_map,
};
