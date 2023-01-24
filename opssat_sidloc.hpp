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

#pragma once
#include "uio_device.hpp"
#include "descriptor.hpp"
#include "dma_device.hpp"
#include "util.hpp"
#include <vector>

#define NUM_CHAINS                  1
#define DESC_PER_CHAIN              10
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
    uint32_t*   __fifo_arbiter;

public:
    opssat_sidloc();
    ~opssat_sidloc();
    int activate_stream();
    int read_stream(uint32_t* buffer, size_t len);
    void reset_device();
};
