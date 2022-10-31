readonly TOOLCHAIN_URL=http://sources.buildroot.net/toolchain-external-arm-aarch64/gcc-arm-8.3-2019.03-x86_64-aarch64-linux-gnu.tar.xz
readonly TOOLCHAIN_DIR=/toolchain
readonly TOOLCHAIN_TAR=gcc-arm-8.3-2019.03-x86_64-aarch64-linux-gnu.tar.xz

mkdir -p $TOOLCHAIN_DIR
wget $TOOLCHAIN_URL -O $TOOLCHAIN_DIR/$TOOLCHAIN_TAR
tar -xf $TOOLCHAIN_DIR/$TOOLCHAIN_TAR -C $TOOLCHAIN_DIR && rm $TOOLCHAIN_DIR/$TOOLCHAIN_TAR 
