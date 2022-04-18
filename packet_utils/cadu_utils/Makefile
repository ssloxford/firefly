all: caduinfo cadupack cadupack-asan caduunpack cadurandomise ccsdsinfo ccsdspack ccsdsunpack

caduinfo: src/caduinfo.cpp include/cadu_constants.h
	g++ --std=c++20 -o bin/caduinfo -Wl,-rpath=/usr/local/lib -I ./include/ -I ../getsetproxy/include/ -I ../libcadu/include/ -g src/caduinfo.cpp -lfec
	
#cadupack: src/cadupack.cpp include/cadu_constants.h
#	g++ --std=c++20 -o bin/cadupack -Wl,-rpath=/usr/local/lib -I ./include/ -I ../getsetproxy/include/ -I ../libcadu/include/ -I ../libccsds/include/ -g src/cadupack.cpp -lfec
	
cadupack: src/cadupack.cpp include/cadu_constants.h
	g++ --std=c++20 -o bin/cadupack -fsanitize=address -static-libasan -fno-omit-frame-pointer -O1 -Wl,-rpath=/usr/local/lib -I ./include/ -I ../getsetproxy/include/ -I ../libcadu/include/ -I ../libccsds/include/ -g src/cadupack.cpp -lfec
	
caduunpack: src/caduunpack.cpp include/cadu_constants.h
	g++ --std=c++20 -o bin/caduunpack -Wl,-rpath=/usr/local/lib -I ./include/ -I ../getsetproxy/include/ -I ../libcadu/include/ -g src/caduunpack.cpp -lfec

cadurandomise: src/cadurandomise.cpp include/cadu_constants.h
	g++ --std=c++20 -o bin/cadurandomise -Wl,-rpath=/usr/local/lib -I ./include/ -I ../getsetproxy/include/ -I ../libcadu/include/ -g src/cadurandomise.cpp -lfec
	
ccsdsinfo: src/ccsdsinfo.cpp
	g++ --std=c++20 -o bin/ccsdsinfo -Wl,-rpath=/usr/local/lib -I ./include/ -I ../getsetproxy/include/ -I ../libcadu/include/ -I ../libccsds/include/ -g src/ccsdsinfo.cpp -lfec
	
ccsdspack: src/ccsdspack.cpp
	g++ --std=c++20 -o bin/ccsdspack -Wl,-rpath=/usr/local/lib -I ./include/ -I ../getsetproxy/include/ -I ../libcadu/include/ -I ../libccsds/include/ -g src/ccsdspack.cpp -lfec

ccsdsunpack: src/ccsdsunpack.cpp
	g++ --std=c++20 -o bin/ccsdsunpack -Wl,-rpath=/usr/local/lib -I ./include/ -I ../getsetproxy/include/ -I ../libcadu/include/ -I ../libccsds/include/ -g src/ccsdsunpack.cpp -lfec

.PHONY: install
install:
	install -D -m 755 bin/caduinfo /usr/local/bin/
	install -D -m 755 bin/cadupack /usr/local/bin/
	install -D -m 755 bin/caduunpack /usr/local/bin/
	install -D -m 755 bin/cadurandomise /usr/local/bin/
	install -D -m 755 bin/ccsdsinfo /usr/local/bin/
	install -D -m 755 bin/ccsdspack /usr/local/bin/
	install -D -m 755 bin/ccsdsunpack /usr/local/bin/
