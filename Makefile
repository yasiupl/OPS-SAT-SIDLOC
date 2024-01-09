CC:=arm-linux-gnueabihf-g++

# Path to the standard image root directory
STANDARD_IMAGE_PATH:=/home/experiments/exp202/Standard_Image

LD_PATH:=$(STANDARD_IMAGE_PATH)/Linux_API/build/lib/arm-poky-linux-gnueabi
INCLUDE_PATH:=$(STANDARD_IMAGE_PATH)/Linux_API/build/include

all:
	$(CC) -O3 -std=c++11 -o launch/exp202 exp202.cpp opssat_sidloc.cpp uio_device.cpp dma_device.cpp descriptor.cpp -I $(INCLUDE_PATH) -L $(LD_PATH)  -l:libsdr_api.so -l:libsepp_api_core.so -l:libsepp_ic.so -pthread

clean:
	rm -rf launch/exp202
