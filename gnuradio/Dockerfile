FROM sslox-gnuradio-3.8

# Install gr-satnogs dependencies
RUN apt-get -y install \
    libboost-dev \
    libitpp-dev \
    libboost-date-time-dev \
    libboost-filesystem-dev \
    libboost-program-options-dev \
    libboost-system-dev \
    libboost-thread-dev \
    libboost-regex-dev \
    libboost-test-dev \
    swig \
    cmake \
    build-essential \
    pkg-config \
    gnuradio-dev \
    libconfig++-dev \
    libgmp-dev \
    liborc-0.4-0 \
    liborc-0.4-dev \
    liborc-0.4-dev-bin \
    nlohmann-json3-dev \
    libpng++-dev \
    libvorbis-dev \
    git

# Download, build, and install gr-satnogs
WORKDIR /
RUN git clone https://gitlab.com/librespacefoundation/satnogs/gr-satnogs.git
WORKDIR gr-satnogs
RUN mkdir build
WORKDIR build
RUN cmake ..
RUN make -j $(nproc --all)
RUN make install

# Download, build, and install gr-satellites
WORKDIR /
RUN pip3 install construct requests
RUN apt install -y feh
RUN wget https://github.com/daniestevez/gr-satellites/archive/refs/tags/v3.11.0.tar.gz -O gr-satellites.tar.gz
RUN mkdir gr-satellites
RUN tar -xf gr-satellites.tar.gz -C gr-satellites --strip-components=1
RUN rm gr-satellites.tar.gz
WORKDIR gr-satellites
RUN mkdir build
WORKDIR build
RUN cmake ..
RUN make -j $(nproc --all)
RUN make install
RUN ldconfig

# Set workdir back to what it originally was
WORKDIR /radio

# Set python path correctly so we can import the new modules
ENV PYTHONPATH="/usr/local/lib/python3/dist-packages:/usr/local/lib/python3/site-packages:/usr/local/lib64/python3/site-packages"
RUN echo "export PYTHONPATH=/usr/local/lib/python3/dist-packages:/usr/local/lib/python3/site-packages:/usr/local/lib64/python3/site-packages" >> /root/.bashrc

ENTRYPOINT [ "/bin/bash" ]
