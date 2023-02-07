main=src/main

srcs=src/seam_carving.c src/grid.c src/pixel.c src/image.c


img2bin=../tools/img_to_binary.py
bin2img=../tools/binary_to_img.py

num_seams=1

.PHONY: $(main)

src/testing: src/testing.c $(srcs)
	gcc -fopenmp -Wall -pedantic -O3 -o $@ $<
compile: $(main)


$(main): src/main.c $(srcs)
	gcc -fopenmp -Wall -pedantic -O3 -o $@ $<

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
	rm -rf *-out.png
