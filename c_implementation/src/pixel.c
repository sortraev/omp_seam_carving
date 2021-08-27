#pragma once

typedef uint32_t energy;

typedef union {
  struct rgb {
    uint8_t r, g, b;
  } rgb;
  uint32_t u32;
} pixel;


energy pdist(pixel p1, pixel p2) {
  energy r_d = p1.rgb.r - p2.rgb.r;
  energy g_d = p1.rgb.g - p2.rgb.g;
  energy b_d = p1.rgb.b - p2.rgb.b;
  return r_d*r_d + g_d*g_d + b_d*b_d;
}
