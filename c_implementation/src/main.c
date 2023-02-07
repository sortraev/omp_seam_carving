#include <stdio.h>
#include <stdlib.h>

#include "seam_carving.c"

#define NUM_THREADS 8

int main(int argc, char **argv) {

  if (argc != 4) {
    fprintf(stderr, "Usage: %s <input filename> "
                    "<output filename> <num seams>.\n", argv[0]);
    exit(1);
  }

  char *in_filename  = argv[1];
  char *out_filename = argv[2];

  char *endptr;
  int num_seams = (int) strtol(argv[3], &endptr, 10);
  if (*endptr != '\0') {
    fprintf(stderr, "Third argument (num of seams to carve) must be int!\n");
    exit(1);
  }

  image *img;

  if ((img = img_from_file(in_filename)) == NULL) {
    fprintf(stderr, "Failed to open input file.\n");
    exit(1);
  }

  if ((img = carve_n_seams(img, num_seams)) == NULL) {
    fprintf(stderr, "Seam carving failed.\n");
    exit(1);
  }

  if (img_to_file(img, out_filename) != 0) {
    fprintf(stderr, "Failed to store carved image to file.\n");
    exit(1);
  }

  return 0;
}
