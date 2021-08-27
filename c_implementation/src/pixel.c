#pragma once

typedef uint32_t energy;
typedef uint32_t pixel;

static inline void unpack_rgb(pixel p1, uint8_t *r, uint8_t *g, uint8_t *b) {
  *r = (uint8_t) p1;
  *g = (uint8_t) p1 >> 8;
  *b = (uint8_t) p1 >> 16;
}

energy pdist(pixel p1, pixel p2) {
  uint8_t r1, g1, b1;
  uint8_t r2, g2, b2;
  unpack_rgb(p1, &r1, &g1, &b1);
  unpack_rgb(p2, &r2, &g2, &b2);
  energy r_d = r1 - r2;
  energy g_d = g1 - g2;
  energy b_d = b1 - b2;
  return r_d*r_d + g_d*g_d + b_d*b_d;
}
