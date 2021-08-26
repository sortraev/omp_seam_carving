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

    file_in = sys.argv[1]
    if not os.path.exists(file_in):
        error("Error: Input filename \"" + file_in + "\" does not exist!")

    file_out = sys.argv[2]

    try:
        img = PIL.Image.open(file_in).convert("RGBA")
    except PIL.UnidentifiedImageError:
        error("Error: Failed to read image file! Most likely the "
              "given input filename is not an image file.")

    rgb_arr = np.array(img, dtype = np.uint8)
    h, w, *_ = rgb_arr.shape

    with open(file_out, "wb") as f:
        f.write(np.uint32(h))
        f.write(np.uint32(w))
        rgb_arr.tofile(f)
