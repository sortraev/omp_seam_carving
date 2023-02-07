#include <stdio.h>
#include "grid.c"

typedef grid_t image;

image *img_from_file(char *in_filename) {

  FILE *f = fopen(in_filename, "rb");

  if (!f) {
    fprintf(stderr, "Failed to open \"%s\" for reading!\n", in_filename);
    return NULL;
  }


  // the first 8 bytes of the image file is height/width of the image.
  int h, w;
  if (fread((char*)&h, sizeof(int), 1, f) +
      fread((char*)&w, sizeof(int), 1, f) != 2) {
    fprintf(stderr, "Bad input image data: height/width header "
                    "missing or ill-formed.\n");
    return NULL;
  }

  int num_pixels = h * w;
  uint32_t *buf  = (uint32_t*) malloc(num_pixels * sizeof(uint32_t));

  int num_read = fread((char*)buf, sizeof(uint32_t), num_pixels, f);
  if (num_read != num_pixels) {
    free(buf);
    fprintf(stderr, "Expected %d pixels, but got %d!\n", num_pixels, num_read);
    return NULL;
  }

  if (fclose(f) != 0) {
    fprintf(stderr, "Something went wrong in closing \"%s\"\n", in_filename);
    // return NULL; // TODO: fclose() failed but image loaded correctly. what to do?
  }

  image *out = new_grid(h, w);
  out->cells = buf;

  return out;
}


int img_to_file(image *img, char *out_filename) {

  FILE *f = fopen(out_filename, "wb");

  if (!f) {
    fprintf(stderr, "Failed to open \"%s\" for writing!\n", out_filename);
    return -1;
  }

  int h = img->h;
  int w = img->w;
  pixel *pixels = (pixel*) img->cells;


  if (fwrite((char*) &h, sizeof(int), 1, f) +
      fwrite((char*) &w, sizeof(int), 1, f) != 2) {
    fprintf(stderr, "Failed to write height/width "
                    "header to \"%s\"!\n", out_filename);
    return -1;
  }

  if (fwrite((char*) pixels, sizeof(pixel), h * w, f) != h * w) {
    fprintf(stderr, "Failed to write image pixels to \"%s\"!\n", out_filename);
    return -1;
  }

  if (fclose(f) != 0) {
    fprintf(stderr, "Failed to close file \"%s\"!\n", out_filename);
    return -1;
  }

  return 0;
}
