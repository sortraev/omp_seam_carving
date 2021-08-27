#include <iostream>
#include "seam_carving.cpp"

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
