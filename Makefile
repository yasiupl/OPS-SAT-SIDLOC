all: 
	arm-linux-gnueabihf-g++ -o cmain main.cpp opssat_sidloc.cpp uio_device.cpp dma_device.cpp descriptor.cpp -pthread
