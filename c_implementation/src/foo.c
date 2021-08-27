#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct {
  uint8_t r, g, b;
} pixel;

pixel from_u32(uint32_t a) {
  return *(pixel*) &a;
}

int main() {

  pixel p2 = from_u32(127 | (107 << 8) | (87 << 16));
  printf("%d, %d, %d\n", p2.r, p2.g, p2.b);
}
