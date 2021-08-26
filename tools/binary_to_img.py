#!/usr/bin/env python3
import sys
import os.path
import re
import PIL.Image
import numpy as np

def error(msg): print(msg); sys.exit(1)

if __name__ == "__main__":

    if len(sys.argv) != 3:
        error("Usage:", sys.argv[0], "<input filename> <output filename>")

    file_in  = sys.argv[1]
    file_out = sys.argv[2]

    try:
        data = np.fromfile(file_in, dtype=np.uint32)
    except FileNotFoundError:
        error("Error: Input filename \"" + file_in + "\" does not exist!")

    try:
        h, w, *rest = data
    except ValueError:
        error("Bad input data: height/width header missing (or file is empty)!")

    pixels_flat = np.array(rest)
    num_pixels  = len(pixels_flat)

    if num_pixels != h * w:
        error("Bad input data: expected", h * w, "pixels, but found", num_pixels)

    pixels = pixels_flat.view(np.uint8).reshape((h, w, 4))

    PIL.Image.fromarray(pixels).save(file_out)
