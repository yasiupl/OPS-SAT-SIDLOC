#include "descriptor.hpp"
#include "uio_device.hpp"
#include "dma_device.hpp"
#include "util.hpp"
#include <vector>
#include <chrono>
#include <thread>
#include <iostream>
#include <thread>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include "opssat_sidloc.hpp"
#include<unistd.h>
#include <iostream>
#include <fstream>

#define REPS         3

/* This chunk of code tests the opssat_sidloc API. Using the test core in the FPGA
we generate a counter which counts up at every clock cycle and receive the counter values in the CPU.  */

int main(int argc,char* argv[]){
    /* Create the device handle */
    opssat_sidloc opssat_dev = opssat_sidloc();
    int actual_reps;
    int reps = (argc >= 2) ? std::atoi(argv[1]) : REPS;
    actual_reps = reps;

    /* Buffer to store received samples */
    uint32_t buffer32[ reps * DESC_PER_CHAIN * (LEN_PER_DESCRIPTOR/4)];
    int ret;
    /* Activate the stream */
    opssat_dev.activate_stream();
    
    /* Read REPS times from the FPGA */
    for(int i = 0; i < reps; i++){    
       ret = opssat_dev.read_stream(&buffer32[i * DESC_PER_CHAIN * LEN_PER_DESCRIPTOR/4], DESC_PER_CHAIN * LEN_PER_DESCRIPTOR);
        if(ret == -1)
            std::cout << "Timeout" << std::endl;
        else if(ret == -2)
            std::cout << "Error" << std::endl;
    }
    std::ofstream out_file;
    out_file.open ("output_samples.bin", std::ios::out  | std::ios::binary);
    out_file.write((char*)buffer32,reps * DESC_PER_CHAIN * LEN_PER_DESCRIPTOR);
    out_file.close();

    /* Reset device */
    opssat_dev.reset_device();
}