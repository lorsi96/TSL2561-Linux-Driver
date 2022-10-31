docker run --rm -it \
    -v $(pwd)/src:/work \
    -e ARCH=arm64 \
    -e KDIR=/lib/modules/5.15.74-sunxi64/build \
    -e CROSS_COMPILE=aarch64-linux-gnu- \
    lorsi/sunxi-kernel-dev:1.0.0
