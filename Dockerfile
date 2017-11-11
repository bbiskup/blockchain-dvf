FROM ubuntu:16.04
MAINTAINER bbiskup@gmx.de

RUN apt-get -qy update && apt-get -qy install wget xz-utils

# Install recent version of clang
WORKDIR /
ENV CLANG_VERSION=5.0.0
ENV CLANG_PACKAGE=clang+llvm-${CLANG_VERSION}-linux-x86_64-ubuntu16.04
RUN wget -q http://releases.llvm.org/${CLANG_VERSION}/${CLANG_PACKAGE}.tar.xz
RUN xz -d ${CLANG_PACKAGE}.tar.xz && \
    tar xf ${CLANG_PACKAGE}.tar && \
    rm ${CLANG_PACKAGE}.tar
ENV PATH=/${CLANG_PACKAGE}/bin:$PATH
RUN ln -sf /${CLANG_PACKAGE}/bin/clang++ /usr/bin/clang++
RUN ln -sf /${CLANG_PACKAGE}/bin/clang++ /usr/bin/c++

RUN apt-get -yq install unzip cmake libstdc++-5-dev

# Install Ninja build system
RUN wget -q https://github.com/ninja-build/ninja/releases/download/v1.8.2/ninja-linux.zip  && \
    unzip ninja-linux.zip && \
    mv ninja /usr/bin && \
    chmod ugo+x /usr/bin/ninja

WORKDIR /code
