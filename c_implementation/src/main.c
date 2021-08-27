#include <stdio.h>
#include <stdlib.h>

#include "seam_carving.c"

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

  image *img = from_file(in_filename);

  img = carve_n_seams(img, num_seams);
  to_file(img, out_filename);
  free_grid(img);


  return 0;
}
