#pragma once
// #include <cstring>
#include <stdint.h>


typedef struct {
  int   h;
  int   w;
  uint32_t *cells;
} grid_t;


grid_t *new_grid(int h, int w) {
  grid_t *out = (grid_t*) malloc(sizeof(grid_t));
  out->h = h;
  out->w = w;
  out->cells = (uint32_t*) malloc(h * w * sizeof(uint32_t));
  return out;
}

grid_t *grid_from_cells(int h, int w, uint32_t *init) {
  grid_t *out = new_grid(h, w);
  uint32_t *cells = out->cells;
  for (int i = 0; i < h; i++) {
    for (int j = 0; j < w; j++) {
      cells[i * w + j] = init[i * w + j];
    }
  }
  return out;
}

void free_grid(grid_t *g) {
  free(g->cells);
  free(g);
}

int grid_equal(grid_t *g1, grid_t *g2) {

  int h = g1->h;
  int w = g1->w;

  if (h != g2->h || w != g2->w) return 0;

  uint32_t *cells1 = g1->cells;
  uint32_t *cells2 = g2->cells;

  for (int i = 0; i < h * w; i++)
    if (cells1[i] != cells2[i])
      return 0;

  return 1;
}

void print_grid(grid_t *g) {

  int h           = g->h;
  int w           = g->w;
  uint32_t *cells = g->cells;

  printf("[\n");

  for (int i = 0; i < h - 1; i++) {
    printf(" [");
    for (int j = 0; j < w - 1; j++)
      printf("%u, ", cells[i * w + j]);
    if (w > 0) printf("%u], \n", cells[(i + 1) * w - 1]);
    else       printf("], \n");
  }
  if (h > 0) {
    printf(" [");
    for (int j = 0; j < w - 1; j++)
      printf("%u, ", cells[(h - 1) * w + j]);
    if (w > 0) printf("%u]\n", cells[h * w - 1]);
    else       printf("]\n");
  }
  printf("]\n");
}

