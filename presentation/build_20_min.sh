find . | entr -s "xelatex firefly_20_min.tex; bibtex firefly_20_min && xelatex firefly_20_min.tex; xelatex -jobname notes firefly_20_min.tex"
