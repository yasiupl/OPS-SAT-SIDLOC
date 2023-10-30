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


/*
 Arguments:
    output_filename : the name of the output file to be created
    is_constrained: indicates whether the capture will run indefinately or for a specific
                    number of samples. If 0, it will run forever, if 1 it will run for the
                    indicated number of samples
    number_of_samples: The number of 4 byte (interleaved 16-bit I/Q) samples to store.
 */
int main(int argc,char* argv[]){
    /* Create the device handle */
    const char * output_filename;
    bool is_constrained;
    int64_t nitems;
    size_t reps;
    int ret;
    std::fstream out_file;

    /* Buffer to store received samples */
    uint32_t buffer32[ DESC_PER_CHAIN * LEN_PER_DESCRIPTOR];
    if(argc < 4)
        throw(std::invalid_argument("Invalid number of arguments given. 3 required: output_filename, is_constrained, number_of_samples "));
    else{
        output_filename = argv[1];
        is_constrained = atoi(argv[2]);
        nitems = atol(argv[3]);
    }
    if(!is_constrained){
        nitems = 1;     /*To trick the while statement to run idefinately*/
    }
    opssat_sidloc opssat_dev = opssat_sidloc("/dev/uio0", "/dev/uio1");

    /* Activate the stream */
    opssat_dev.activate_stream();

    out_file.open (output_filename, std::ios::out  | std::ios::binary);

    while(nitems > 0){
        ret = opssat_dev.read_stream(buffer32, DESC_PER_CHAIN * LEN_PER_DESCRIPTOR);
        if(ret < 0){
            continue;
        }
        out_file.write((char*)buffer32,DESC_PER_CHAIN * LEN_PER_DESCRIPTOR);
        if(is_constrained)
            nitems -= (DESC_PER_CHAIN * LEN_PER_DESCRIPTOR)/sizeof(uint32_t);
    }

    out_file.close();

}
