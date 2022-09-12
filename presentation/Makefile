pdf: main.tex
	latexmk -pdf main.tex
	latexmk -pdf -jobname=notes main.tex

clean:
	rm -rf *.aux *.bcf *.lof *.log *.out *.run.xml *.toc *.bbl *.blg *.fdb_latexmk *.fls *.nav *.snm

.PHONY: pdf clean
