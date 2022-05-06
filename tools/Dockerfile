FROM ubuntu:22.04
WORKDIR /root

RUN apt update
RUN export DEBIAN_FRONTEND=noninteractive
# Install build dependencies
RUN ["apt", "-y", "install", "make", "gcc-12", "g++-12"]

# Install custom dependencies
RUN apt -y install libcxxopts-dev

# Install libfec
RUN apt -y install curl gcc
RUN curl -L https://github.com/quiet/libfec/tarball/master -o libfec.tar.gz
RUN tar -xzvf libfec.tar.gz
RUN cd quiet-libfec-9750ca0 && ./configure && make && make install


RUN ln -s /usr/bin/g++-12 /usr/local/bin/g++

RUN apt -y install libstdc++-12-dev

CMD make -C /root/src
