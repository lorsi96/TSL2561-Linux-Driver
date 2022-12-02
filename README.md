# TSL256X-Linux-Driver
TSL2561 I2C Light Sensor driver implemented as a Kernel Module for ARM64
architecture.

## Overview
This project was build to display how to develop a simple I2C Subsystem Driver
with a concrete I2C device connected to an SBC.

## Hardware
For this project the following hardware has been used
1. **SBC**: Libretech Tritium H3 (Allwinner H5 version)
1. **Sensor**: TSL2561 Light Sensor Adafruit Kit 

Essentially, the connection diagram is as follows:
```mermaid
flowchart LR
A --> B
```

```plantuml
A --> B
```

## Development Environment
To compile and build the module, this project features a Dockerfile to easily
build a suitable development environment. There are multiple reasons for this:
1. The exact toolchain and Linux headers are downloaded and installed in the
environment when the image is build. This leaves no room for the user to 
obtain the wrong toolchain or headers version.
1. The host computer doesn't need to natively install headers or crosscompilers
that will (probably) be only useful for this project. It also reduces the 
chances of breaking anything in the host computer.

To get it working, simply run `scripts/docker_build.sh` the first time and 
`scripts/docker_run.sh` each time compilation of the module is needed. The
second command will open up a terminal ready to run `make` and build the module.
If curious about what's that doing, everything related to the environment built
is within the [docker](./docker/) directory.


## Embedded Linux
The SBC has been flashed with a cutom `Armbian` image. The configuration of the
Linux Kernel used to build such image can be found [here](./docker/kernel/.config).  
Some other relevant details of the build distribution are listed below:
1. **Kernel**: linux-5.15.74-sunxi64
1. **Armbian**: sunxi64_22.08.6_arm64

### Some Tips on building the Armbian Image
Building armbian is pretty much trivial, but customizing it or trying to learn
the details of the intermediate steps ran to make it happen may not. To shed 
some light on these steps, it's a good idea to set up the `compile.sh` command
(provided by Armbian) to store cache artifacts somewhere accesible.  

### Toolchain
The Linux toolchain used to build the module present in this project must match 
the one used to build the `Armbian` image. It may not be easy to learn which
toolchain was used by `Armbian` to build the image, given that more than one
toolchain is downloaded during the image construction. Following the steps
described [above](#some-tips-on-building-the-armbian-image) should help

### Headers
Ngl, to find the exact headers needed to get this to work, y redirected the 
stdoutput of Armbians's compile.sh command to a text file, and searched through
the logs until I found which headers were used. Is there a better way? Probably,
but that's what worked for me.

## Driver Implementation









