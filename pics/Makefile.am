KDE_ICON=AUTO

generate:
	@echo "Converting SVG files to PNG files. Sizes: 16x16; 22x22; 32x32; 48x48; 64x64; 128x128"

	@for file in *.svg*; do \
		echo "Processing: $$file..."; \
		ksvgtopng 16 16 $$file `basename $$file | sed -e 's/crsc/cr16/' | sed -e 's/svg\|svgz/png/'`; \
		ksvgtopng 22 22 $$file `basename $$file | sed -e 's/crsc/cr22/' | sed -e 's/svg\|svgz/png/'`; \
		ksvgtopng 32 32 $$file `basename $$file | sed -e 's/crsc/cr32/' | sed -e 's/svg\|svgz/png/'`; \
		ksvgtopng 48 48 $$file `basename $$file | sed -e 's/crsc/cr48/' | sed -e 's/svg\|svgz/png/'`; \
		ksvgtopng 64 64 $$file `basename $$file | sed -e 's/crsc/cr64/' | sed -e 's/svg\|svgz/png/'`; \
		ksvgtopng 128 128 $$file `basename $$file | sed -e 's/crsc/cr128/' | sed -e 's/svg\|svgz/png/'`; \
	done

