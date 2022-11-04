# Works with Tritium H5 Armbian.
# Built from commit 037248761dc78601015bf0f4a4e779de53384d7d of armbian repo.
FROM ubuntu:22.04 
ENV ARCH=arm64
ENV KDIR=/lib/modules/5.15.74-sunxi64/build
ENV CROSS_COMPILE=/toolchain/gcc-arm-8.3-2019.03-x86_64-aarch64-linux-gnu/bin/aarch64-linux-gnu-

RUN apt update -y && apt install --fix-missing -y  \
  build-essential libncurses-dev flex bison openssl libssl-dev dkms libelf-dev \
  libudev-dev libpci-dev libiberty-dev autoconf libmpc-dev libgmp3-dev wget

COPY scripts/ /usr/bin/

# Toolchain
RUN get_toolchain.sh

# Linux Headers
COPY kernel/.config  $KDIR/
RUN get_sunxi_headers.sh

WORKDIR /work
