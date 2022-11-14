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

#define REPS         100

uint32_t __builtin_bswap32 (uint32_t x);

/* This chunk of code tests the opssat_sidloc API. Using the test core in the FPGA
we generate a counter which counts up at every clock cycle and receive the counter values in the CPU. 
At the end of the test we check if the values are continues to determine if we've lost any counter value. */

int main(int argc,char* argv[]){
    /* Create the device handle */
    opssat_sidloc opssat_dev = opssat_sidloc();
    int actual_reps;
    int reps = (argc >= 2) ? std::atoi(argv[1]) : REPS;
    actual_reps = reps;
    /* Buffer to store received samples */
    size_t buffer32[ reps * DESC_PER_CHAIN * (LEN_PER_DESCRIPTOR/4)];
    int ret;
    /* Activate the stream */
    opssat_dev.activate_stream();

    /* Read REPS times from the FPGA */
    while(actual_reps-- < reps){
        ret = opssat_dev.read_stream(&buffer32[0 * DESC_PER_CHAIN * LEN_PER_DESCRIPTOR], DESC_PER_CHAIN * LEN_PER_DESCRIPTOR);
        if(ret == -1)
            std::cout << "Timeout" << std::endl;
        else if(ret == -2)
            std::cout << "Error" << std::endl;
    }
    
    /* Check to see if the counter values are continuous*/
    int diff;
    size_t prev = __builtin_bswap32(buffer32[0]);
    for(int i = 1 ; i < reps * DESC_PER_CHAIN * (LEN_PER_DESCRIPTOR/4) ; i++){
        diff = prev - __builtin_bswap32(buffer32[i]);
        if(std::abs(diff) > 1)
            std::cout << "Discont "<< i << " " << prev << " " << __builtin_bswap32(buffer32[i])<< std::endl;
        prev = __builtin_bswap32(buffer32[i]);
    }

    /* Reset device */
    opssat_dev.reset_device();
}