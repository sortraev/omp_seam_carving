#include <omp.h>

#include "grid.c"
#include "pixel.c"
#include "image.c"

#define ENERGY_MAX INT32_MAX

#define NUM_THREADS    8

typedef grid_t image;
typedef grid_t energymap;

static inline int mod(int a, int b) {
  return (a + b) % b;
}
static inline energy e_min(energy a, energy b) {
  return a < b ? a : b;
}
static inline energy e_min3(energy a, energy b, energy c) {
  return e_min(e_min(a, b), c);
}


grid_t *compute_energies_omp_unroll_inner(grid_t *img_in) {

  int h = img_in->h;
  int w = img_in->w;
  pixel *pixels = img_in->cells;

  energymap *emap     = new_grid(h, w);
  energy    *energies = emap->cells;

  #pragma omp parallel num_threads(NUM_THREADS)
  {
    #pragma omp for
    for (int i = 0; i < h; i++) {
      pixel c  = pixels[i * w],
            ul = pixels[mod(i - 1, h)*w + w - 1],
            u  = pixels[mod(i - 1, h)*w],
            ur = pixels[mod(i - 1, h)*w + 1],
            l  = pixels[i*w + w - 1],
            r  = pixels[i*w + 1],
            dl = pixels[mod(i + 1, h)*w + w - 1],
            d  = pixels[mod(i + 1, h)*w],
            dr = pixels[mod(i + 1, h)*w + 1];

      energies[i * w] =
        pdist(c, ul) + pdist(c,  u) +
        pdist(c, ur) + pdist(c,  l) +
        pdist(c,  r) + pdist(c, dl) +
        pdist(c,  d) + pdist(c, dr);

      for (int j = 1; j < w - 1; j++) {
        c  = pixels[i * w + j];
        ul = pixels[mod(i - 1, h)*w + j - 1];
        u  = pixels[mod(i - 1, h)*w + j];
        ur = pixels[mod(i - 1, h)*w + j + 1];
        l  = pixels[i*w + j - 1];
        r  = pixels[i*w + j + 1];
        dl = pixels[mod(i + 1, h)*w + j - 1];
        d  = pixels[mod(i + 1, h)*w + j];
        dr = pixels[mod(i + 1, h)*w + j + 1];

        energies[i * w + j] =
          pdist(c, ul) + pdist(c,  u) +
          pdist(c, ur) + pdist(c,  l) +
          pdist(c,  r) + pdist(c, dl) +
          pdist(c,  d) + pdist(c, dr);
      }
      c  = pixels[i * w + w - 1];
      ul = pixels[mod(i - 1, h)*w + w - 2];
      u  = pixels[mod(i - 1, h)*w + w - 1];
      ur = pixels[mod(i - 1, h)*w];
      l  = pixels[i*w + w - 2];
      r  = pixels[i*w];
      dl = pixels[mod(i + 1, h)*w + w - 2];
      d  = pixels[mod(i + 1, h)*w + w - 1];
      dr = pixels[mod(i + 1, h)*w];

      energies[i * w + w - 1] =
        pdist(c, ul) + pdist(c,  u) +
        pdist(c, ur) + pdist(c,  l) +
        pdist(c,  r) + pdist(c, dl) +
        pdist(c,  d) + pdist(c, dr);

    }
  }
  return emap;
}

energy *compute_local_minseams(grid_t *emap) {
  /* compute the map of locally minimal seams using bottom-up DP:
   * M[i, j] = E[i, j] + min(M[i-1, j-1], M[i-1, j], M[i-1, j+1]),
   * where E and M are the energy map and local min seams, respectively. this is
   * done in-place since the original energy map is not needed afterwards. 
   */

  int h = emap->h;
  int w = emap->w;
  energy *energies = emap->cells;

  for (int i = 0; i < h - 1; i++) {
    int prev_row = i * w;
    int this_row = (i+1) * w;

    energies[this_row] +=
      e_min(energies[prev_row], energies[prev_row + 1]);


      // TODO: why does this not benefit from parallelization? again, probably
      //       because of oversaturation.
      // #pragma omp parallel for num_threads(NUM_THREADS)
      for (int j = 1; j < w - 1; j++) {
        energies[this_row + j] +=
          e_min3(energies[prev_row + j - 1],
                 energies[prev_row + j],
                 energies[prev_row + j + 1]);
      }

    energies[this_row + w - 1] +=
      e_min(energies[prev_row + w - 1], energies[prev_row + w - 2]);
  }
  return energies;
}







int *compute_global_minseam(grid_t *emap) {

  /* 
   * given an emap of local minseams, we want to find the *globally* minimal
   * seam. this will be an array of h indices (where h is the size of emap)
   * whose i'th element is the row i component of the global minseam.
   *
   * first, find the bottom-row end-point of this global minseam (essentially an
   * argmin). this step is parallelized if the image is wide enough.
   *
   * second, starting from this bottom-row end-point, backtrack up the image to
   * compute the array of global minseam indices. return as is.
   *
   */

  int h = emap->h;
  int w = emap->w;

  energy *lseams = compute_local_minseams(emap);

  energy *last_row = lseams + (h - 1) * w;
  energy min       = ENERGY_MAX;
  int    argmin    = 0;

  // compute bottom-row endpoint of global minseam (the argmin mentioned above).
#define PAR_ARGMIN_IMAGE_WIDTH_THRESHOLD 2048
  if (w >= PAR_ARGMIN_IMAGE_WIDTH_THRESHOLD) {
    static energy mins[NUM_THREADS];
    static int    argmins[NUM_THREADS];
    #pragma omp parallel num_threads(NUM_THREADS)
    {
      energy my_min = min;
      int my_argmin = argmin;

      #pragma omp for
      for (int i = 0; i < w; i++)
        if (last_row[i] < my_min)
          my_min = last_row[my_argmin = i];

      mins[omp_get_thread_num()]    = my_min;
      argmins[omp_get_thread_num()] = my_argmin;
    }

    for (int i = 0; i < NUM_THREADS; i++) {
      if (mins[i] < min) {
        min    = mins[i];
        argmin = argmins[i];
      }
    }
  }
  else
    // sequential argmin
    for (int i = 0; i < w; i++)
      if (last_row[i] < min)
        min = last_row[argmin = i];

  // now, backtrack up the map of computed local minseams as explained above.
  int *gseam_is = (int*) malloc(h * sizeof(int));

  int prev = gseam_is[h - 1] = argmin;

  // we already have the last index, so start iteration at second last row.
  for (int i = h - 2; i >= 0; i--) {
    int l = prev - (prev > 0);
    int r = prev + (prev < w - 1);

    int tmp  = lseams[i * w + l]   < lseams[i * w + r]    ? l : r;
    int next = lseams[i * w + tmp] < lseams[i * w + prev] ? tmp : prev;
    gseam_is[i] = prev = next;
  }

  return gseam_is;
}

grid_t *carve_one_seam(grid_t *img_in) {

  grid_t *emap  = compute_energies_omp_unroll_inner(img_in);
  int *gseam_is = compute_global_minseam(emap);
  free_grid(emap);

  int h = img_in->h;
  int w = img_in->w;
  int new_w = w - 1;

  grid_t *img_out = new_grid(h, new_w);

  pixel *p_in  = img_in->cells;
  pixel *p_out = img_out->cells;

  /* 
   * carve out the global minseam by concatenating the arrays 
   * p_in[i, :gseam_is[i]] and p_in[i, gseam_is[i]+1:] for each i. 
   *
   * TODO: why does this not benefit from omp? probably due to oversaturation
   */
  for (int i = 0; i < h; i++) {
    int split = gseam_is[i];
    for (int j = 0; j < split; j++)
      p_out[i * new_w + j] = p_in[i * w + j];

    for (int j = split + 1; j < w; j++)
      p_out[i * new_w + j - 1] = p_in[i * w + j];
  }

  free(gseam_is);
  free_grid(img_in);

  return img_out;
}

grid_t *carve_n_seams(grid_t *img, int n) {

  if (n < 0 || n > img->w) {
    fprintf(stderr, "Invalid number of seams to remove "
                    "(expected 0 <= n < image width).");
    exit(1);
  }

  for (int i = 0; i < n; i++)
    img = carve_one_seam(img);
  return img;
}
