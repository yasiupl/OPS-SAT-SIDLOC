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

#define REPS         1

uint32_t __builtin_bswap32 (uint32_t x);

int main(int argc,char* argv[]){
    opssat_sidloc opssat_dev = opssat_sidloc();
    int reps = (argc >= 2) ? std::atoi(argv[1]) : REPS;
    uint8_t buffer[ reps * DESC_PER_CHAIN * LEN_PER_DESCRIPTOR];
    size_t buffer32[ reps * DESC_PER_CHAIN * (LEN_PER_DESCRIPTOR/4)];
    int ret;
    //opssat_dev.reset_device();
    opssat_dev.activate_stream();

    while(1){
    //for(int i = 0 ; i < reps ; i++){
        //ret = opssat_dev.read_stream((uint8_t*) &buffer[i * DESC_PER_CHAIN * LEN_PER_DESCRIPTOR], DESC_PER_CHAIN * LEN_PER_DESCRIPTOR);
        ret = opssat_dev.read_stream((uint8_t*) &buffer[0 * DESC_PER_CHAIN * LEN_PER_DESCRIPTOR], DESC_PER_CHAIN * LEN_PER_DESCRIPTOR);
        if(ret == -1)
            std::cout << "Timeout" << std::endl;
        else if(ret == -2)
            std::cout << "Error" << std::endl;
    }
    
    memcpy(buffer32, buffer, reps * DESC_PER_CHAIN * LEN_PER_DESCRIPTOR);
    int diff;
    size_t prev = __builtin_bswap32(buffer32[0]);
    for(int i = 1 ; i < reps * DESC_PER_CHAIN * (LEN_PER_DESCRIPTOR/4) ; i++){
        diff = prev - __builtin_bswap32(buffer32[i]);
        if(std::abs(diff) > 1)
            std::cout << "Discont "<< i << " " << prev << " " << __builtin_bswap32(buffer32[i])<< std::endl;
        prev = __builtin_bswap32(buffer32[i]);
    }
    // opssat_dev.reset_device();
}