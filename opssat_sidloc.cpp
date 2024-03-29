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
#include <chrono>
#include <iostream>
#include <string.h>
#include <thread>

opssat_sidloc::opssat_sidloc(const char *ddr_uio_name, const char *dma_uio_name)
{
  int ret;
  __ddr_uio.set_dev_name(ddr_uio_name);
  __dma_uio.set_dev_name(dma_uio_name);
  ret = __ddr_uio.open_dev(0x00110000);
  if (ret < 0) {
    std::runtime_error("DDR map failed");
  }
  ret = __dma_uio.open_dev(0x00001000);
  if (ret < 0) {
    std::cout << "DMA map failed" << std::endl;
  }
  __dma_dev.set_uio_device(__dma_uio);

  __samples_ptr = __ddr_uio.get_ptr(0);
  __desc_chains = std::vector<std::vector<descriptor>>(NUM_CHAINS);
};

opssat_sidloc::~opssat_sidloc() {}
/**
 * @brief activate_stream: Used at the beginning of execfution to setup the
 * descriptors and the device. Called once if no reset is done.
 *
 * @return int 0 for success
 */

int
opssat_sidloc::activate_stream()
{

  uint32_t next_desc_ptr;
  __current_desc = 0;

  for (int k = 0; k < NUM_CHAINS; k++) {
    for (int i = 0; i < DESC_PER_CHAIN; i++) {
      if ((k == (NUM_CHAINS - 1)) && (i == (DESC_PER_CHAIN - 1)))
        next_desc_ptr = INITIAL_DESC_BASE_OFFSET;
      else
        next_desc_ptr = ((k * DESC_PER_CHAIN + i + 1) * DESC_WORD_SIZE * 4 +
                         INITIAL_DESC_BASE_OFFSET);
      descriptor d = descriptor(
          ((k * DESC_PER_CHAIN + i) * DESC_WORD_SIZE), 0,
          INITIAL_STORAGE_OFFSET +
              ((k * NUM_CHAINS * DESC_PER_CHAIN + i) * LEN_PER_DESCRIPTOR),
          next_desc_ptr, LEN_PER_DESCRIPTOR,
          (k * NUM_CHAINS * DESC_PER_CHAIN + i) * LEN_PER_DESCRIPTOR / 4);
      d.write_to_mem(__ddr_uio);
      __desc_chains[k].push_back(d);
    }
  }

  __dma_dev.reset_dma();
  __dma_dev.set_nex_dext_ptr(INITIAL_DESC_BASE_OFFSET);
  __dma_dev.set_dma_ctrl(IE_GLOBAL | RUN | STOP_DMA_ER | DESC_POLL_EN |
                         (1 << 20));
  return 0;
}

/**
 * @brief read_stream: Reads the specified number of bytes from the FPGA. Data
 * is returned as 32-bit integers where 16 LSB is the real and 16 MSB is the
 * imaginary part of the complex number.
 *
 * @param buffer The buffer where the 32bit values will be stored
 * @param len The amount of bytes to transfer
 * @return int 0 if success, < 0 for error
 */

int
opssat_sidloc::read_stream(uint32_t *buffer, size_t len)
{
  if (len % LEN_PER_DESCRIPTOR != 0)
    return -3;
  size_t num_desc = len / LEN_PER_DESCRIPTOR;
  size_t timeout  = 0;
  while (timeout < TIMEOUT) {
    if (!(__desc_chains[__current_desc][DESC_PER_CHAIN - 1].read_status(
              __ddr_uio) &
          OWNED_BY_HW) ||
        __dma_dev.get_error()) {
      break;
    }
    std::this_thread::sleep_for(std::chrono::microseconds(170));
    timeout++;
  }
  if (timeout >= TIMEOUT) {
    std::cout << "Timeout " << __current_desc << std::endl;
    return -1;
  }
  if (__dma_dev.get_error()) {
    return -2;
  } else {
    memcpy(&buffer[0],
           &__samples_ptr[INITIAL_STORAGE_OFFSET_WORDS +
                          __desc_chains[__current_desc][0].get_write_offset()],
           DESC_PER_CHAIN * LEN_PER_DESCRIPTOR);
    for (int j = 0; j < DESC_PER_CHAIN; j++) {
      __desc_chains[__current_desc][j].descriptor_reset(__ddr_uio);
    }
    if (__current_desc == NUM_CHAINS - 1)
      __current_desc = 0;
    else
      __current_desc++;
  }
  timeout = 0;
  return 0;
}

void
opssat_sidloc::reset_device()
{
  for (int k = 0; k < NUM_CHAINS; k++) {
    __desc_chains[k].clear();
  }
  memset(&__samples_ptr[0], 0, INITIAL_STORAGE_OFFSET_WORDS * 4);

  __dma_dev.set_dma_ctrl(__dma_dev.get_dma_ctrl() ^ RUN);
  __dma_dev.set_dma_status(__dma_dev.get_dma_status() ^
                           (STS_CHAIN_COMPLETED | STS_DESCRIPTOR_COMPLETED));
  __dma_dev.reset_dma();
}