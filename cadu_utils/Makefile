all: caduinfo cadupack caduextract ccsdsinfo ccsdsextract

caduinfo: src/caduinfo.cpp include/cadu_constants.h
	g++ --std=c++20 -o bin/caduinfo -I ./include/ -g src/caduinfo.cpp -lfec
	
cadupack: src/cadupack.cpp include/cadu_constants.h
	g++ --std=c++20 -o bin/cadupack -I ./include/ -g src/cadupack.cpp -lfec
	
caduextract: src/caduextract.cpp include/cadu_constants.h
	g++ --std=c++20 -o bin/caduextract -I ./include/ -g src/caduextract.cpp -lfec
	
ccsdsinfo: src/ccsdsinfo.cpp
	g++ --std=c++20 -o bin/ccsdsinfo -I ./include/ -I ../libccsds/include/ -g src/ccsdsinfo.cpp -lfec
	
ccsdspack: src/ccsdspack.cpp
	g++ --std=c++20 -o bin/ccsdspack -I ./include/ -I ../libccsds/include/ -g src/ccsdspack.cpp -lfec
ccsdsextract: src/ccsdsextract.cpp
	g++ --std=c++20 -o bin/ccsdsextract -I ./include/ -I ../libccsds/include/ -g src/ccsdsextract.cpp -lfec

