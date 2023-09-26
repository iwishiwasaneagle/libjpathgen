FROM ubuntu:lunar

RUN apt-get update \
    && apt-get install -y \
      git build-essential cmake \
      python3.11 python3-pip \
      libeigen3-dev pybind11-dev \
      libgeos++-dev libgeos-dev libgeos3.11.1

WORKDIR /opt
RUN git clone --depth 1 --branch v3.4.0 https://github.com/catchorg/Catch2.git \
    && cd Catch2 \
    && cmake -Bbuild -H. -DBUILD_TESTING=OFF \
    && cmake --build build/ --target install

WORKDIR /src
COPY . .
RUN pip install .[test] --break-system-package
