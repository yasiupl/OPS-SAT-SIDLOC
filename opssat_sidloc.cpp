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

#include "opssat_sidloc.hpp"
#include "uio_device.hpp"
#include <thread>
#include <chrono>
#include <string.h>
#include <iostream>

opssat_sidloc::opssat_sidloc()
{
    __ddr_uio.set_dev_name("/dev/uio0");
    __dma_uio.set_dev_name("/dev/uio1");
    __ddr_uio.open_dev(0x00110000);
    __dma_uio.open_dev(0x00001000);
    __dma_dev.set_uio_device(__dma_uio);
    __samples_ptr = __ddr_uio.get_ptr(0);
    __fifo_arbiter = __dma_uio.get_ptr(256);
    __desc_chains = std::vector<std::vector<descriptor>>(NUM_CHAINS);
    __current_desc = 0;
};

opssat_sidloc::~opssat_sidloc(){

}
/**
 * @brief activate_stream: Used at the beginning of execfution to setup the descriptors
 * and the device. Called once if no reset is done.
 * 
 * @return int 0 for success
 */

int opssat_sidloc::activate_stream(){

    uint32_t next_desc_ptr;

    for(int k = 0; k < NUM_CHAINS; k++){
        for(int i = 0; i < DESC_PER_CHAIN; i++){
            if((k == (NUM_CHAINS - 1)) && (i == (DESC_PER_CHAIN - 1)))
                next_desc_ptr = INITIAL_DESC_BASE_OFFSET;
            else
                next_desc_ptr = ((k + i + 1) *  DESC_WORD_SIZE * 4 + 
                INITIAL_DESC_BASE_OFFSET);
            descriptor d = descriptor(((k + i) *  DESC_WORD_SIZE), 0, 
                    INITIAL_STORAGE_OFFSET + ((k * NUM_CHAINS * DESC_PER_CHAIN + i) * LEN_PER_DESCRIPTOR),
                    next_desc_ptr, LEN_PER_DESCRIPTOR, (k * NUM_CHAINS * DESC_PER_CHAIN + i) * LEN_PER_DESCRIPTOR / 4);
            d.write_to_mem(__ddr_uio);
            __desc_chains[k].push_back(d);
        }
    }

    __dma_dev.reset_dma();
    __dma_dev.set_nex_dext_ptr(INITIAL_DESC_BASE_OFFSET);
    __dma_dev.set_dma_ctrl(IE_GLOBAL | RUN | STOP_DMA_ER | DESC_POLL_EN | (1 << 20));
    return 0;
}

/**
 * @brief read_stream: Reads the specified number of bytes from the FPGA. Data is returned as
 * 32-bit integers where 16 LSB is the real and 16 MSB is the imaginary part of the complex number.
 * 
 * @param buffer The buffer where the 32bit values will be stored
 * @param len The amount of bytes to transfer
 * @return int 0 if success, < 0 for error
 */

int opssat_sidloc::read_stream(uint32_t* buffer, size_t len){
    if(len % LEN_PER_DESCRIPTOR != 0)
        return -3;
    size_t num_desc = len / LEN_PER_DESCRIPTOR;
    size_t timeout = 0;
    *(__fifo_arbiter) = 1;
    for(int i = 0; i < num_desc ; i++){
        while(timeout < TIMEOUT){
            if(!(__desc_chains[0][__current_desc].read_status(__ddr_uio) & OWNED_BY_HW) || __dma_dev.get_error())
                break;
            std::this_thread::sleep_for(std::chrono::microseconds(640));
            timeout++;
        }
        if(timeout >= TIMEOUT){
            std::cout << "Timeout " << __current_desc << std::endl;
            return -1;
        }
        if(__dma_dev.get_error()){
            return -2;
        }
        else{
            memcpy(&buffer[i * (LEN_PER_DESCRIPTOR/4)], 
                &__samples_ptr[INITIAL_STORAGE_OFFSET_WORDS + __desc_chains[0][__current_desc].get_write_offset()], LEN_PER_DESCRIPTOR);
             __desc_chains[0][__current_desc].descriptor_reset(__ddr_uio);
        }
        timeout = 0;
        __current_desc = (__current_desc + 1 >= DESC_PER_CHAIN) ? 0 : __current_desc + 1;
    }
    return 0;
}

void opssat_sidloc::reset_device(){
    __dma_dev.set_dma_ctrl(__dma_dev.get_dma_ctrl() ^ RUN);
    __dma_dev.set_dma_status(__dma_dev.get_dma_status() ^ (STS_CHAIN_COMPLETED | STS_DESCRIPTOR_COMPLETED));
}