DIRS=bin/

all: modismaskfires

modismaskfires: src/modismaskfires.cpp
	g++ -static -g --std=c++20 -o bin/modismaskfires -Wl,-rpath=/usr/local/lib -I ./include/ -I ../getsetproxy/include/ -I ../libccsds/include/ -I ../libgiis/include/ -I ../seqiter/include/ -g src/modismaskfires.cpp -lfec

.PHONY: install
install:
	install -D -m 755 bin/modismaskfires /usr/local/bin/

$(shell mkdir -p $(DIRS))
