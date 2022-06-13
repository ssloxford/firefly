find . | entr -s "pdflatex firefly_full.tex; bibtex firefly_full && pdflatex firefly_full.tex"
