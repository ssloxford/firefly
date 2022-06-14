find . | entr -s "pdflatex firefly_20_min.tex; bibtex firefly_20_min && pdflatex firefly_20_min.tex; pdflatex -jobname notes firefly_20_min.tex"
