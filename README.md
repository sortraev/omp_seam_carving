# seam carving

C++ implementation of seam carving (content-aware resizing of images). 

The implementation:

* uses index wrapping to compute energies at image boundaries.
* is parallelized using OpenMP (where applicable and beneficial) but not 
    particularly optimized.
* currently only carves vertical seams, and, due to uint32 overflow, handles
    only images which are less than 22000 pixels in the height.
* takes (almost) any image file type as input, but only outputs PNG.

--- 

Usage:

```
./run.sh <input filename> <output filename> <num seams to carve>
```


To see an example, take `pano.jpg`<sup>[[1]](#pano)</sup> from `images/`,
carve 400 seams from it, and store the result in `images/pano-out.png` like so:

```
./run.sh images/pano.jpg images/pano-out.png 400
```


<a name="pano">[1]</a> https://shwestrick.github.io/2020/07/29/seam-carve.html
