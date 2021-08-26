#pragma once

typedef uint32_t Energy;


class Pixel {
  private:
    uint8_t r, g, b, _a;

  public:

    // default is simply a black Pixel.
    Pixel() : r(0), g(0), b(0), _a(255) {}

    // Pixel from RGB.
    Pixel(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b), _a(255) {}

    // Pixel from RGB packed into single four-byte word.
    Pixel(uint32_t w) : r{(uint8_t) w},
                        g{(uint8_t) (w >> 8)},
                        b{(uint8_t) (w >> 16)},
                        _a{255} {}

    // pack this Pixel into a four-byte word.
    uint32_t to_uint32();

    // "distance" between two Pixels.
    static Energy dist(Pixel p1, Pixel p2);

    std::string toString();
};


inline Energy Pixel::dist(Pixel p1, Pixel p2) {
  Energy r_d = p1.r - p2.r;
  Energy g_d = p1.g - p2.g;
  Energy b_d = p1.b - p2.b;
  return r_d*r_d + g_d*g_d + b_d*b_d;
}


inline uint32_t Pixel::to_uint32() {
  return r | g << 8 | b << 16 /* | a << 24 */;
}


std::string Pixel::toString() {
  return std::to_string(to_uint32());
  return "p(" + std::to_string(r) + ", "
              + std::to_string(g) + ", "
              + std::to_string(b) + ")";
}


std::ostream& operator <<(std::ostream &os, Pixel &p) {
  os << p.toString();
  return os;
}
