main=src/main

srcs=src/seam_carving.cpp src/Grid.cpp src/Pixel.cpp src/Image.cpp


test_img_dir=./images
img2bin=tools/img_to_binary.py
bin2img=tools/binary_to_img.py

num_seams=1

.PHONY: %-out.png

compile: $(main)

$(main): src/main.cpp $(srcs)
	g++ -fopenmp -O3 -o $@ $<

%-in.dat: %.png $(img2bin)
	$(img2bin) $< $@

%-out.dat: %-in.dat $(main)
	$(main) $(num_seams) $< $@

%-out.png: %-out.dat $(bin2img)
	$(bin2img) $< $@


clean:
	rm -rf a.out $(main) $(testprog)
	rm -rf vgcore*
	rm -rf $(test_img_dir)/*-in.dat $(test_img_dir)/*-out.*
