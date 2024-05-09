#if defined(LV_LVGL_H_INCLUDE_SIMPLE)
#include "lvgl.h"
#else
#include "../lvgl/lvgl.h"
#endif

#ifndef LV_ATTRIBUTE_MEM_ALIGN
#define LV_ATTRIBUTE_MEM_ALIGN
#endif

#ifndef LV_ATTRIBUTE_IMG_IMGCIRCULAR
#define LV_ATTRIBUTE_IMG_IMGCIRCULAR
#endif

const LV_ATTRIBUTE_MEM_ALIGN LV_ATTRIBUTE_LARGE_CONST LV_ATTRIBUTE_IMG_IMGCIRCULAR uint8_t imgCircular_map[] = {
#if LV_COLOR_DEPTH == 16 && LV_COLOR_16_SWAP != 0
  /*Pixel format:  Blue: 5 bit Green: 6 bit, Red: 5 bit, Alpha 8 bit  BUT the 2  color bytes are swapped*/
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFC, 0x41, 0x40, 0xFC, 0x41, 0xBF, 0xF4, 0x41, 0xEF, 0xFC, 0x41, 0xFF, 0xFC, 0x41, 0xBF, 0xFC, 0x41, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0xFC, 0x21, 0x5F, 0xFC, 0x41, 0xFF, 0xFC, 0x41, 0xFF, 0xFC, 0x41, 0xFF, 0xFC, 0x41, 0xFF, 0xFC, 0x41, 0xFF, 0xFC, 0x41, 0xFF, 0xFC, 0x21, 0x5F, 0x00, 0x00, 0x00, 
  0xFC, 0x41, 0x20, 0xFC, 0x41, 0xDF, 0xFC, 0x41, 0xFF, 0xFC, 0x41, 0xFF, 0xFC, 0x41, 0xFF, 0xFC, 0x41, 0xFF, 0xFC, 0x41, 0xFF, 0xFC, 0x41, 0xFF, 0xFC, 0x41, 0xEF, 0xF4, 0x02, 0x10, 
  0xF4, 0x41, 0x60, 0xFC, 0x41, 0xFF, 0xFC, 0x41, 0xFF, 0xFC, 0x41, 0xFF, 0xFC, 0x41, 0xFF, 0xFC, 0x41, 0xFF, 0xFC, 0x41, 0xFF, 0xFC, 0x41, 0xFF, 0xFC, 0x41, 0xFF, 0xF4, 0x41, 0x60, 
  0xFC, 0x41, 0x80, 0xFC, 0x41, 0xFF, 0xFC, 0x41, 0xFF, 0xFC, 0x41, 0xFF, 0xFC, 0x41, 0xFF, 0xFC, 0x41, 0xFF, 0xFC, 0x41, 0xFF, 0xFC, 0x41, 0xFF, 0xFC, 0x41, 0xFF, 0xFC, 0x41, 0x80, 
  0xF4, 0x41, 0x60, 0xFC, 0x41, 0xFF, 0xFC, 0x41, 0xFF, 0xFC, 0x41, 0xFF, 0xFC, 0x41, 0xFF, 0xFC, 0x41, 0xFF, 0xFC, 0x41, 0xFF, 0xFC, 0x41, 0xFF, 0xFC, 0x41, 0xFF, 0xF4, 0x41, 0x60, 
  0xF4, 0x02, 0x10, 0xFC, 0x41, 0xEF, 0xFC, 0x41, 0xFF, 0xFC, 0x41, 0xFF, 0xFC, 0x41, 0xFF, 0xFC, 0x41, 0xFF, 0xFC, 0x41, 0xFF, 0xFC, 0x41, 0xFF, 0xFC, 0x41, 0xDF, 0xFC, 0x42, 0x20, 
  0x00, 0x00, 0x00, 0xF4, 0x41, 0x60, 0xFC, 0x41, 0xFF, 0xFC, 0x41, 0xFF, 0xFC, 0x41, 0xFF, 0xFC, 0x41, 0xFF, 0xFC, 0x41, 0xFF, 0xFC, 0x41, 0xFF, 0xF4, 0x41, 0x60, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF4, 0x21, 0x30, 0xF4, 0x41, 0xBF, 0xFC, 0x41, 0xFF, 0xFC, 0x41, 0xEF, 0xFC, 0x41, 0xBF, 0xF4, 0x41, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
#endif
};

const lv_img_dsc_t imgCircular = {
  .header.always_zero = 0,
  .header.w = 10,
  .header.h = 9,
  .data_size = 90 * LV_IMG_PX_SIZE_ALPHA_BYTE,
  .header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA,
  .data = imgCircular_map,
};
