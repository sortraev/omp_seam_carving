#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "seam_carving.c"

#define NUM_THREADS 4

typedef struct timeval timeval;

int timeval_subtract(timeval *result, timeval *x, timeval *y);
image *random_image(int h, int w);


int main() {
  srand(43);
  struct timeval start, end, diff;

  const int height    = 2000;
  const int width     = 2000;
  const int num_seams = 680;

  image *img = random_image(height, width);

  printf("carving %d seams from %d x %d image using %d threads\n",
      num_seams, height, width, NUM_THREADS);

  {
    gettimeofday(&start, NULL);
    img = carve_n_seams(img, num_seams);
    gettimeofday(&end, NULL);
  }

  timeval_subtract(&diff, &end, &start);
  printf("time taken: %ld seconds %ld microseconds\n", diff.tv_sec, diff.tv_usec);

  // img_to_file(img, NULL);

  return 0;
}


image *random_image(int h, int w) {

  image *out = new_grid(h, w);

  for (int i = 0; i < h; i++)
    for (int j = 0; j < w; j++)
      out->cells[i * w + j] = rand();
  return out;
}



int timeval_subtract(timeval *result, timeval *x, timeval *y) {
  // preserve *y
  timeval yy = *y;
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
