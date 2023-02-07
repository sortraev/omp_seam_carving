#include <fstream>
#include "Grid.cpp"
#include <stdlib.h>

struct Exception : public std::exception {
  protected:
    std::string msg;
  public:
  explicit Exception(std::string _msg) : msg(_msg) {}
  const char *what() const throw() {
    return msg.c_str();
  }
};


class Image : public Grid<Pixel> {

  private:
    Pixel* pixels = cells;

  public:

    Image(int h, int w) : Grid(h, w) {}

    // build Image using pixels given in a file. is a static method rather than
    // a constructor because C++ requires that base constructor is called first.
    static Image from_file(std::string in_filename);

    static Image random_image(int h, int w);

    // store image in file.
    void to_file(std::string out_filename);
};


Image Image::random_image(int h, int w) {
  Image out(h, w);
  Pixel *pixels = out.getCells();

  for (int i = 0; i < h; i++) {
    for (int j = 0; j < w; j++) {
      pixels[i * w + j] = (Pixel) rand();
    }
  }
  return out;
}


Image Image::from_file(std::string in_filename) {
  std::fstream f(in_filename, std::ios::in | std::ios::binary);

  if (!f)
    throw Exception("Failed to open \"" + in_filename + "\"");

  // the first 8 bytes of the image file is height/width of the image.
  int _h, _w;
  f.read((char*)&_h, sizeof(int));
  f.read((char*)&_w, sizeof(int));
  if (f.fail())
    throw Exception("Bad input image data: height/width header missing or ill-formed.");

  int num_pixels = _h * _w;
  uint32_t *buf  = new uint32_t[num_pixels];

  f.read((char*)buf, num_pixels * sizeof(uint32_t));
  int num_pixels_read = f.gcount() / sizeof(uint32_t);
  if (num_pixels_read != num_pixels)
    throw Exception("Expected "         + std::to_string(num_pixels) +
                    " pixels, but got " + std::to_string(num_pixels_read));

  if (f.fail())
    throw Exception("Something went wrong reading image from file.");

  f.close();

  Image out(_h, _w);
  Pixel *_pixels = out.getCells();

  for (int i = 0; i < num_pixels; i++)
    _pixels[i] = Pixel(buf[i]);

  delete[] buf;

  return out;
}

void Image::to_file(std::string out_filename) {

  if (out_filename.empty()) 
    return;

  std::fstream f(out_filename, std::ios::out | std::ios::binary);

  if (!f)
    throw Exception("Failed to open \"" + out_filename + "\" for writing"); 

  f.write((char*) &h, sizeof(int));
  f.write((char*) &w, sizeof(int));
  if (f.fail())
    throw Exception("Failed to write height/width header to file");

  f.write((char*) pixels, h * w * sizeof(Pixel));
  if (f.fail())
    throw Exception("Failed to write image pixels to file");
  f.close();

  return;
}
