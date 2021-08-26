#!/bin/bash

set -e

if [ $# -ne 3 ]; then
  printf "Usage: $0 <input filename> <.png output filename> <num_seams>\n"
  exit 1
fi

img_in=$1
img_out=$2
num_seams=$3

if [ "${img_out##*.}" != "png" ]; then
  printf "Error: Provided output filename must have a .png extension!\n"
  exit 1
fi

number_re="^[0-9]+$"
if ! [[ $num_seams =~ $number_re ]] ; then
  printf "Error: Provided num_seams is not integer!\n"
  exit 1
fi

main=src/main

make $main || exit 1

img2bin=tools/img_to_binary.py
bin2img=tools/binary_to_img.py

bin_in=$(mktemp)
bin_out=$(mktemp)


printf "Converting image to binary data ...\n"
$img2bin $img_in $bin_in
if [ $? -ne 0 ]; then
  printf "Error: Failed to convert image to binary data.\n"
else
  printf "Carving $num_seams seams from image ...\n"
  $main $bin_in $bin_out $num_seams
  if [ $? -ne 0 ]; then
    printf "Error: Seam carving failed.\n"
  else
    printf "Storing carved image ...\n"
    $bin2img $bin_out $img_out
    if [ $? -ne 0 ]; then
      printf "Error: Failed to convert binary data back to image.\n"
    else
      printf "Done! Carved image stored in $img_out.\n"
    fi
  fi
fi

rm -f $bin_in $bin_out

exit 0
