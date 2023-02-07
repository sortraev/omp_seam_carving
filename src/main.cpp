#include <iostream>
#include <stdlib.h>
#include <sys/time.h>

#include "seam_carving.cpp"


int timeval_subtract(struct timeval *result, struct timeval *x, struct timeval *y) {
  // preserve *y
  struct timeval yy = *y;
  y = &yy;

  /* Perform the carry for the later subtraction by updating y. */
  if (x->tv_usec < y->tv_usec) {
    int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
    y->tv_usec -= 1000000 * nsec;
    y->tv_sec += nsec;
  }
  if (x->tv_usec - y->tv_usec > 1000000) {
    int nsec = (y->tv_usec - x->tv_usec) / 1000000;
    y->tv_usec += 1000000 * nsec;
    y->tv_sec -= nsec;
  }

  /* Compute the time remaining to wait.
     tv_usec is certainly positive. */
  result->tv_sec = x->tv_sec - y->tv_sec;
  result->tv_usec = x->tv_usec - y->tv_usec;

  /* Return 1 if result is negative. */
  return x->tv_sec < y->tv_sec;
}

int main(int argc, char **argv) {

  if (argc != 4) {
    std::cerr << "Usage: " << argv[0] <<
                 " <input filename> <output filename> <num seams>.\n";
    exit(1);
  }

  char *in_filename  = argv[1];
  char *out_filename = argv[2];

  char *endptr;
  int num_seams = (int) std::strtol(argv[3], &endptr, 10);
  if (*endptr != '\0') {
    std::cerr << "Third argument (num of seams to carve) must be int!\n";
    exit(1);
  }

  Image img1 = Image::from_file(in_filename);
  Image img2 = carve_n_seams(img1, num_seams);
  img2.to_file(out_filename);

  return 0;
}
