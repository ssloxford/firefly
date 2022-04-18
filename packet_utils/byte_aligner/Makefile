all: byte_aligner header_counter denoiser checksum_corrector

byte_aligner: byte_aligner.cpp
	g++ -o byte_aligner byte_aligner.cpp

header_counter: header_counter.cpp
	g++ -o header_counter header_counter.cpp

denoiser: denoiser.cpp
	g++ -o denoiser denoiser.cpp
	
checksum_corrector: checksum_corrector.cpp
	g++ -o checksum_corrector checksum_corrector.cpp
