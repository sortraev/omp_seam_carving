#include <stdio.h>
#include "grid.c"

// struct Exception : public std::exception {
//   protected:
//     std::string msg;
//   public:
//   explicit Exception(std::string _msg) : msg(_msg) {}
//   const char *what() const throw() {
//     return msg.c_str();
//   }
// };


grid_t *from_file(char *in_filename) {

  FILE *f;

  if ((f = fopen(in_filename, "rb")) == NULL) {
    fprintf(stderr, "Failed to open \"%s\" for reading!\n", in_filename);
    exit(1);
  }

  int num_read;

  // the first 8 bytes of the image file is height/width of the image.
  int h, w;
  num_read = fread((char*)&h, sizeof(int), 1, f) +
             fread((char*)&w, sizeof(int), 1, f);
  if (num_read != 2) {
    fprintf(stderr, "Bad input image data: height/width header "
                    "missing or ill-formed. num_read == %d\n", num_read);
    exit(1);
  }

  int num_pixels = h * w;
  uint32_t *buf  = (uint32_t*) malloc(num_pixels * sizeof(uint32_t));

  num_read = fread((char*)buf, sizeof(uint32_t), num_pixels, f);
  if (num_read != h * w) {
    fprintf(stderr, "Expected %d pixels, but got %d!\n", num_pixels, num_read);
    exit(1);
  }

  if (fclose(f) != 0) {
    fprintf(stderr, "Something went wrong in closing \"%s\"\n", in_filename);
    // exit(1);
  }

  grid_t *out = new_grid(h, w);
  out->cells  = buf;

  return out;
}


void to_file(grid_t *img, char *out_filename) {

  FILE *f;

  if ((f = fopen(out_filename, "wb")) == NULL) {
    fprintf(stderr, "Failed to open \"%s\" for writing!\n", out_filename);
    exit(1);
  }

  int h = img->h;
  int w = img->w;
  pixel *pixels = img->cells;

  int num_written;
  num_written = fwrite((char*)&h, sizeof(int), 1, f) +
                fwrite((char*)&w, sizeof(int), 1, f);

  if (num_written != 2) {
    fprintf(stderr, "Failed to write height/width "
                    "header to \"%s\"!\n", out_filename);
    exit(1);
  }

  num_written = fwrite((char*) pixels, sizeof(pixel), h * w, f);
  if (num_written != h * w) {
    fprintf(stderr, "Failed to write image pixels to \"%s\"!\n", out_filename);
    exit(1);
  }

  if (fclose(f) != 0) {
    fprintf(stderr, "Failed to close file \"%s\"!\n", out_filename);
    exit(1);
  }

  return;
}
