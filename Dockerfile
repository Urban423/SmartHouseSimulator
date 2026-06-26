FROM ubuntu:24.04

ARG TARGET=windows
ENV TARGET=${TARGET}

RUN apt-get update && apt-get install -y \
	build-essential \
	cmake \
	ninja-build \
	curl \
	tar \
	unzip \
	rsync \
	mingw-w64 \
	libgl1-mesa-dev \
	python3 python3-pip \
	&& rm -rf /var/lib/apt/lists/*

WORKDIR /opt

RUN curl -fL -o zlib-1.3.1.tar.gz \
    https://zlib.net/fossils/zlib-1.3.1.tar.gz && \
    tar -xzf zlib-1.3.1.tar.gz && \
    rm zlib-1.3.1.tar.gz


WORKDIR /opt/zlib-1.3.1

RUN CC=x86_64-w64-mingw32-gcc \
    AR=x86_64-w64-mingw32-ar \
    RANLIB=x86_64-w64-mingw32-ranlib \
    ./configure --static --prefix=/opt/zlib-windows && \
    make && make install

WORKDIR /opt
RUN pip install glad --break-system-packages

WORKDIR /opt/glad-gen
RUN glad --api gl=4.6 --generator=c --spec=gl --out-path=.

RUN x86_64-w64-mingw32-gcc -c src/glad.c -Iinclude -o glad.o && \
	ar rcs libglad.a glad.o && \
    mkdir -p /opt/glad-windows/lib /opt/glad-windows/include && \
    mv libglad.a /opt/glad-windows/lib/ && \
    mv glad.o /opt/glad-windows/lib/ && \
    cp -r include/glad /opt/glad-windows/include/ && \
    cp -r include/KHR /opt/glad-windows/include/


WORKDIR /workspace