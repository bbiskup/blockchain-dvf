FROM ubuntu:17.10
MAINTAINER bbiskup@gmx.de

RUN apt-get -qy update && \
    apt-get -qy install \
        clang \
        cmake \
        curl \
        gdb \
        less \
        libboost-system1.63-dev \
        libboost1.63-dev \
        libcurl4-gnutls-dev \
        ninja-build \
        unzip \
        valgrind \
        wget \
        xz-utils

RUN wget -q https://github.com/JosephP91/curlcpp/releases/download/1.1/curlcpp-1.1.zip && \
    unzip curlcpp-1.1.zip && \
    cd curlcpp-master && \
    mkdir -p build && \
    cd build && \
    cmake .. && \
    make -j 4 && \
    make install


WORKDIR /code
