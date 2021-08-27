#include <omp.h>

#include "Grid.cpp"
#include "Pixel.cpp"
#include "Image.cpp"

#define ENERGY_MAX INT32_MAX
#define NUM_THREADS 8

typedef Grid<Energy> EnergyMap;

static inline int mod(int a, int b) {
  return (a + b) % b;
}
static inline Energy e_min(Energy a, Energy b) {
  return a < b ? a : b;
}
static inline Energy e_min3(Energy a, Energy b, Energy c) {
  return e_min(e_min(a, b), c);
}

EnergyMap compute_energies(Image &img) {

  int h = img.getHeight();
  int w = img.getWidth();
  Pixel *pixels = img.getCells();

  EnergyMap emap(h, w);
  Energy *energies = emap.getCells();

  for (int i = 0; i < h; i++) {
    for (int j = 0; j < w; j++) {
      Pixel c  = pixels[i * w + j],
            ul = pixels[mod(i - 1, h)*w + mod(j - 1, w)],
            u  = pixels[mod(i - 1, h)*w + j],
            ur = pixels[mod(i - 1, h)*w + mod(j + 1, w)],
            l  = pixels[i*w + mod(j - 1, w)],
            r  = pixels[i*w + mod(j + 1, w)],
            dl = pixels[mod(i + 1, h)*w + mod(j - 1, w)],
            d  = pixels[mod(i + 1, h)*w + j],
            dr = pixels[mod(i + 1, h)*w + mod(j + 1, w)];

      energies[i * w + j] =
        Pixel::dist(c, ul) + Pixel::dist(c,  u) +
        Pixel::dist(c, ur) + Pixel::dist(c,  l) +
        Pixel::dist(c,  r) + Pixel::dist(c, dl) +
        Pixel::dist(c,  d) + Pixel::dist(c, dr);
    }
  }
  return emap;
}

EnergyMap compute_energies_omp_unroll_inner(Image &img) {

  int h = img.getHeight();
  int w = img.getWidth();
  Pixel *pixels = img.getCells();

  EnergyMap emap(h, w);
  Energy *energies = emap.getCells();

  #pragma omp parallel num_threads(NUM_THREADS)
  {
    #pragma omp for
    for (int i = 0; i < h; i++) {
      Pixel c  = pixels[i * w],
            ul = pixels[mod(i - 1, h)*w + w - 1],
            u  = pixels[mod(i - 1, h)*w],
            ur = pixels[mod(i - 1, h)*w + 1],
            l  = pixels[i*w + w - 1],
            r  = pixels[i*w + 1],
            dl = pixels[mod(i + 1, h)*w + w - 1],
            d  = pixels[mod(i + 1, h)*w],
            dr = pixels[mod(i + 1, h)*w + 1];

      energies[i * w] =
        Pixel::dist(c, ul) + Pixel::dist(c,  u) +
        Pixel::dist(c, ur) + Pixel::dist(c,  l) +
        Pixel::dist(c,  r) + Pixel::dist(c, dl) +
        Pixel::dist(c,  d) + Pixel::dist(c, dr);

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
          Pixel::dist(c, ul) + Pixel::dist(c,  u) +
          Pixel::dist(c, ur) + Pixel::dist(c,  l) +
          Pixel::dist(c,  r) + Pixel::dist(c, dl) +
          Pixel::dist(c,  d) + Pixel::dist(c, dr);
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
        Pixel::dist(c, ul) + Pixel::dist(c,  u) +
        Pixel::dist(c, ur) + Pixel::dist(c,  l) +
        Pixel::dist(c,  r) + Pixel::dist(c, dl) +
        Pixel::dist(c,  d) + Pixel::dist(c, dr);

    }
  }
  return emap;
}


Energy *compute_local_minseams(EnergyMap &emap) {
  /* compute the map of locally minimal seams using bottom-up DP:
   * M[i, j] = E[i, j] + min(M[i-1, j-1], M[i-1, j], M[i-1, j+1]),
   * where E and M are the energy map and local min seams, respectively. this is
   * done in-place since the original energy map is not needed afterwards. 
   */

  int h = emap.getHeight();
  int w = emap.getWidth();
  Energy *energies = emap.getCells();

  for (int i = 0; i < h - 1; i++) {
    int prev_row = i * w;
    int this_row = (i+1) * w;

    energies[this_row] +=
      e_min(energies[prev_row], energies[prev_row + 1]);


      // this is parallelizable but it is not beneficial, most likely due to
      // oversaturation.
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



int *compute_global_minseam(EnergyMap &emap) {

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

  int h = emap.getHeight();
  int w = emap.getWidth();

  Energy *lseams = compute_local_minseams(emap);

  Energy *last_row = lseams + (h - 1) * w;
  Energy min       = ENERGY_MAX;
  int    argmin    = 0;

  // compute bottom-row endpoint of global minseam (the argmin mentioned above).
#define PAR_ARGMIN_IMAGE_WIDTH_THRESHOLD 2048
  if (w >= PAR_ARGMIN_IMAGE_WIDTH_THRESHOLD) {
    static Energy mins[NUM_THREADS];
    static int    argmins[NUM_THREADS];
    #pragma omp parallel num_threads(NUM_THREADS)
    {
      Energy my_min = min;
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
    for (int i = 0; i < w; i++)
      if (last_row[i] < min)
        min = last_row[argmin = i];

  // now, backtrack up the map of computed local minseams as explained above.
  int *gseam_is = new int[h];

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


Image carve_one_seam(Image &img_in) {

  EnergyMap emap = compute_energies_omp_unroll_inner(img_in);
  int *gseam_is  = compute_global_minseam(emap);

  int h = img_in.getHeight();
  int w = img_in.getWidth();
  int new_w = w - 1;

  Image img_out(h, new_w);

  Pixel *p_in  = img_in.getCells();
  Pixel *p_out = img_out.getCells();

  /* 
   * carve out the global minseam by concatenating the arrays 
   * p_in[i, :gseam_is[i]] and p_in[i, gseam_is[i]+1:] for each i. 
   *
   * this loop is parallelizable in either dimension but it is not beneficial,
   * again probably due to oversaturation.
   *
   */
  for (int i = 0; i < h; i++) {
    int split = gseam_is[i];
    for (int j = 0; j < split; j++)
      p_out[i * new_w + j] = p_in[i * w + j];

    for (int j = split + 1; j < w; j++)
      p_out[i * new_w + j - 1] = p_in[i * w + j];
  }

  delete[] gseam_is;

  return img_out;
}


Image carve_n_seams(Image &img, int n) {
  if (n < 0 || n > img.getWidth())
    throw std::invalid_argument("Invalid number of seams to remove "
                                "(expected 0 <= n < image width).");
  Image out = img;
  for (int i = 0; i < n; i++)
    out = carve_one_seam(out);
  return out;
}

