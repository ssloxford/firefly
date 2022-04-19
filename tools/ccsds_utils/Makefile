DIRS=bin/

all: ccsdsinfo ccsdspack ccsdsunpack

ccsdsinfo: src/ccsdsinfo.cpp
	g++ --std=c++20 -o bin/ccsdsinfo -Wl,-rpath=/usr/local/lib -I ./include/ -I ../getsetproxy/include/ -I ../libcadu/include/ -I ../libccsds/include/ -g src/ccsdsinfo.cpp -lfec
	
ccsdspack: src/ccsdspack.cpp
	g++ --std=c++20 -o bin/ccsdspack -Wl,-rpath=/usr/local/lib -I ./include/ -I ../getsetproxy/include/ -I ../libcadu/include/ -I ../libccsds/include/ -g src/ccsdspack.cpp -lfec

ccsdsunpack: src/ccsdsunpack.cpp
	g++ --std=c++20 -o bin/ccsdsunpack -Wl,-rpath=/usr/local/lib -I ./include/ -I ../getsetproxy/include/ -I ../libcadu/include/ -I ../libccsds/include/ -g src/ccsdsunpack.cpp -lfec

.PHONY: install
install:
	install -D -m 755 bin/ccsdsinfo /usr/local/bin/
	install -D -m 755 bin/ccsdspack /usr/local/bin/
	install -D -m 755 bin/ccsdsunpack /usr/local/bin/

$(shell mkdir -p $(DIRS))
