readonly HEADERS_URL='https://fi.mirror.armbian.de/apt/pool/main/l/linux-5.15.74-sunxi64/linux-headers-current-sunxi64_22.08.6_arm64.deb'
readonly HEADERS_DIR=/lib/modules/5.15.74-sunxi64/build
readonly HEADERS_DEB=/tmp/sunxi-headers.deb 

mkdir -p $HEADERS_DIR
wget $HEADERS_URL -O $HEADERS_DEB
dpkg-deb -xv $HEADERS_DEB /tmp && rm $HEADERS_DEB
cd /tmp && mv ./usr/src/linux-headers-5.15.74-sunxi64/* $HEADERS_DIR/
cd $HEADERS_DIR && make scripts