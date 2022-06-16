find . | entr -s "xelatex firefly_full.tex; bibtex firefly_full && xelatex firefly_full.tex; xelatex -jobname notes firefly_full.tex"
