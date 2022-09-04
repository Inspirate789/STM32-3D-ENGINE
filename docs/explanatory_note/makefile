LATEX_ARGS := -shell-escape

.PHONY: build clean_trash clean

explanatory_note.pdf: tex/build.sh tex/*.tex tex/*.bib tex/*.sty tex/*.module tex/*.clo tex/*.bst tex/*.cls tex/*.inc tex/*.layout tex/listings/* tex/img/*
	make build
	make clean_trash

build:
	cd tex && sh ./build.sh && cd ..
	pdftk A=tex/rpz.pdf B=../statement_of_work/SW_Sapozhkow_ICS7-43B.pdf cat A1 B1 A3-end output explanatory_note.pdf
	rm -f tex/rpz.pdf

clean_trash:
	rm -f tex/*.aux tex/*.toc tex/*.out tex/*.log tex/*.bbl tex/*.blg

clean:
	make clean_trash
	rm -f explanatory_note.pdf
