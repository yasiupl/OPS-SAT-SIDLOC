#pragma once
#include "uio_device.hpp"
#include "descriptor.hpp"
#include "dma_device.hpp"
#include "util.hpp"
#include <vector>

#define NUM_CHAINS                  1
#define DESC_PER_CHAIN              100
#define LEN_PER_DESCRIPTOR          (1024 * 10)
#define INITIAL_DESC_BASE_OFFSET    0x0C000000
#define DESC_WORD_SIZE              8
#define TIMEOUT                     10
#define INITIAL_STORAGE_OFFSET      (INITIAL_DESC_BASE_OFFSET + \
                            NUM_CHAINS * DESC_PER_CHAIN * DESC_WORD_SIZE * 4)
#define INITIAL_STORAGE_OFFSET_WORDS (NUM_CHAINS * DESC_PER_CHAIN * DESC_WORD_SIZE)

class opssat_sidloc
{
private:
    uio_device  __ddr_uio;
    uio_device  __dma_uio;
    dma_device  __dma_dev;
    std::vector<std::vector<descriptor>> 
                __desc_chains;
    uint32_t*   __samples_ptr;
    size_t      __current_desc;

public:
    opssat_sidloc();
    ~opssat_sidloc();
    int activate_stream();
    int read_stream(uint8_t* buffer, size_t len);
    void reset_device();
};
