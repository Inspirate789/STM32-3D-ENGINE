LATEX_ARGS := -shell-escape

.PHONY: build clean_trash clean

explanatory_note.pdf: tex/build.sh tex/*.tex tex/*.bib tex/*.sty tex/*.module tex/*.clo tex/*.bst tex/*.cls tex/*.inc tex/*.layout tex/listings/* tex/img/*
	make build
	make clean_trash

build:
	cd tex && sh ./build.sh && cd ..
	mv tex/rpz.pdf explanatory_note.pdf

clean_trash:
	rm -f tex/*.aux tex/*.toc tex/*.out tex/*.log tex/*.bbl tex/*.blg

clean:
	make clean_trash
	rm -f explanatory_note.pdf
