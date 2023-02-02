/*
 *  OPSSAT-SIDLOC Experiment
 *
 *  Copyright (C) 2023, Libre Space Foundation <http://libre.space>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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

    /* Buffer to store received samples */
    uint32_t buffer32[ REPS * DESC_PER_CHAIN * (LEN_PER_DESCRIPTOR/4)];
    int ret;
    /* Activate the stream */
    opssat_dev.activate_stream();
        
    /* Read REPS times from the FPGA */
    for(int i = 0; i < REPS; i++){    
    ret = opssat_dev.read_stream(&buffer32[i * DESC_PER_CHAIN * LEN_PER_DESCRIPTOR/4], DESC_PER_CHAIN * LEN_PER_DESCRIPTOR);
        if(ret == -1){
            std::cout << "Timeout" << std::endl;
            throw std::runtime_error("Timeout");
        }
        else if(ret == -2) {
            std::cout << "Error" << std::endl;
            throw std::runtime_error("Error");
        }
    }
    std::ofstream out_file;
    out_file.open ("output_samples.bin", std::ios::out  | std::ios::binary);
    out_file.write((char*)buffer32,REPS * DESC_PER_CHAIN * LEN_PER_DESCRIPTOR);
    out_file.close();

    /* Reset device */
    opssat_dev.reset_device();

}