all: caduinfo cadupack caduunpack cadurandomise ccsdsinfo ccsdspack ccsdsunpack

caduinfo: src/caduinfo.cpp include/cadu_constants.h
	g++ --std=c++20 -o bin/caduinfo -I ./include/ -I ../libcadu/include/ -g src/caduinfo.cpp -lfec
	
cadupack: src/cadupack.cpp include/cadu_constants.h
	g++ --std=c++20 -o bin/cadupack -I ./include/ -I ../libcadu/include/ -g src/cadupack.cpp -lfec
	
caduunpack: src/caduunpack.cpp include/cadu_constants.h
	g++ --std=c++20 -o bin/caduunpack -I ./include/ -I ../libcadu/include/ -g src/caduunpack.cpp -lfec

cadurandomise: src/cadurandomise.cpp include/cadu_constants.h
	g++ --std=c++20 -o bin/cadurandomise -I ./include/ -I ../libcadu/include/ -g src/cadurandomise.cpp -lfec
	
ccsdsinfo: src/ccsdsinfo.cpp
	g++ --std=c++20 -o bin/ccsdsinfo -I ./include/ -I ../libcadu/include/ -I ../libccsds/include/ -g src/ccsdsinfo.cpp -lfec
	
ccsdspack: src/ccsdspack.cpp
	g++ --std=c++20 -o bin/ccsdspack -I ./include/ -I ../libcadu/include/ -I ../libccsds/include/ -g src/ccsdspack.cpp -lfec

ccsdsunpack: src/ccsdsunpack.cpp
	g++ --std=c++20 -o bin/ccsdsunpack -I ./include/ -I ../libcadu/include/ -I ../libccsds/include/ -g src/ccsdsunpack.cpp -lfec
