FROM ubuntu:17.10
MAINTAINER bbiskup@gmx.de

RUN apt-get -qy update && apt-get -qy install wget xz-utils clang unzip cmake ninja-build gdb
RUN apt-get -qy install libboost1.63-dev

WORKDIR /code
