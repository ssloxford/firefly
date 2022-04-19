DIRS=bin

all: caduinfo cadupack caduunpack cadurandomise

caduinfo: src/caduinfo.cpp include/cadu_constants.h
	g++ --std=c++20 -o bin/caduinfo -Wl,-rpath=/usr/local/lib -I ./include/ -I ../getsetproxy/include/ -I ../libcadu/include/ -g src/caduinfo.cpp -lfec

cadupack: src/cadupack.cpp include/cadu_constants.h
	g++ --std=c++20 -o bin/cadupack -Wl,-rpath=/usr/local/lib -I ./include/ -I ../getsetproxy/include/ -I ../libcadu/include/ -I ../libccsds/include/ -g src/cadupack.cpp -lfec

caduunpack: src/caduunpack.cpp include/cadu_constants.h
	g++ --std=c++20 -o bin/caduunpack -Wl,-rpath=/usr/local/lib -I ./include/ -I ../getsetproxy/include/ -I ../libcadu/include/ -g src/caduunpack.cpp -lfec

cadurandomise: src/cadurandomise.cpp include/cadu_constants.h
	g++ --std=c++20 -o bin/cadurandomise -Wl,-rpath=/usr/local/lib -I ./include/ -I ../getsetproxy/include/ -I ../libcadu/include/ -g src/cadurandomise.cpp -lfec

.PHONY: install
install:
	install -D -m 755 bin/caduinfo /usr/local/bin/
	install -D -m 755 bin/cadupack /usr/local/bin/
	install -D -m 755 bin/caduunpack /usr/local/bin/
	install -D -m 755 bin/cadurandomise /usr/local/bin/

$(shell mkdir -p $(DIRS))
