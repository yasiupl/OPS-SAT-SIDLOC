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
    __desc_chains = std::vector<std::vector<descriptor>>(NUM_CHAINS);
    __current_desc = 0;
};

opssat_sidloc::~opssat_sidloc(){

}

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

int opssat_sidloc::read_stream(uint8_t* buffer, size_t len){
    if(len % LEN_PER_DESCRIPTOR != 0)
        return -1;
    size_t num_desc = len / LEN_PER_DESCRIPTOR;
    size_t timeout = 0;
    for(int i = 0; i < num_desc ; i++){
            while(timeout < TIMEOUT){
                if(!(__desc_chains[0][__current_desc].read_status(__ddr_uio) & OWNED_BY_HW) || __dma_dev.get_error())
                    break;
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
                timeout++;
            }
            if(timeout >= 10){
                std::cout << "Timeout " << __current_desc << std::endl;
                return -1;
            }
            if(__dma_dev.get_error()){
                return -2;
            }
            else{
                memcpy(&buffer[i * (LEN_PER_DESCRIPTOR)], 
                    &__samples_ptr[INITIAL_STORAGE_OFFSET_WORDS + __desc_chains[0][__current_desc].get_write_offset()], LEN_PER_DESCRIPTOR);
                    __desc_chains[0][__current_desc].descriptor_reset(__ddr_uio);
            }
            timeout = 0;
            __current_desc = (__current_desc + 1 >= DESC_PER_CHAIN) ? 0 : __current_desc + 1;
        }
}

void opssat_sidloc::reset_device(){
    __dma_dev.set_dma_ctrl(__dma_dev.get_dma_ctrl() ^ RUN);
    __dma_dev.set_dma_status(__dma_dev.get_dma_status() ^ (STS_CHAIN_COMPLETED | STS_DESCRIPTOR_COMPLETED));
}