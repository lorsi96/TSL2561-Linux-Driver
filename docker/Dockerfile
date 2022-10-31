FROM ubuntu:22.04 
ENV ARCH=arm64
ENV KDIR=/lib/modules/5.15.74-sunxi64/build
ENV CROSS_COMPILE=aarch64-linux-gnu-

RUN apt update -y && apt install --fix-missing -y  \
  build-essential libncurses-dev flex bison openssl libssl-dev dkms libelf-dev \
  libudev-dev libpci-dev libiberty-dev autoconf libmpc-dev libgmp3-dev wget \
  gcc-aarch64-linux-gnu
COPY scripts/ /usr/bin/
COPY kernel/.config  $KDIR/
RUN get_sunxi_headers.sh
RUN cd $KDIR && make scripts  

WORKDIR /work
