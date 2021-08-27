#pragma once

typedef uint32_t Energy;


class Pixel {
  private:
    uint8_t r, g, b, _a;

  public:

    // default is simply a black Pixel.
    Pixel() : r(0), g(0), b(0), _a(255) {}

    // Pixel from RGB packed into single four-byte word.
    Pixel(uint32_t w) : r{(uint8_t) w},
                        g{(uint8_t) (w >> 8)},
                        b{(uint8_t) (w >> 16)},
                        _a{255} {}

    // "distance" between two Pixels.
    static Energy dist(Pixel p1, Pixel p2);

};

inline Energy Pixel::dist(Pixel p1, Pixel p2) {
  Energy r_d = p1.r - p2.r;
  Energy g_d = p1.g - p2.g;
  Energy b_d = p1.b - p2.b;
  return r_d*r_d + g_d*g_d + b_d*b_d;
}
