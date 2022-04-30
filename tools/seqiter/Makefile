all: main test

test: src/test.cpp include/seqiter/seqiter.h
	g++ --std=c++20 -o bin/test -I ./include/ -g src/test.cpp

.PHONY: install
install:
	install -Dm 755 -t /usr/local/include/seqiter/ ./include/seqiter/*
