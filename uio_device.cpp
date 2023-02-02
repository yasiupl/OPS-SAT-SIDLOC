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

#include "uio_device.hpp"
#include <fcntl.h>
#include <cstdlib>
#include <cstdint>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>

uio_device::uio_device(){ 
}
uio_device::~uio_device(){}

void uio_device::set_dev_name(std::string dev_name){
    d_dev_name = dev_name;
}

int uio_device::open_dev(size_t length){
  int uiofd = open(d_dev_name.c_str(), O_RDWR);
  d_length = length;
  off_t offset = 0x0;
  if (uiofd < 0) {
      return -1;
    }

  d_mem = (uint32_t*)mmap(NULL, length, PROT_READ|PROT_WRITE, MAP_SHARED, uiofd, offset);
  if (d_mem == MAP_FAILED) { 
        std::cout << d_dev_name.c_str() <<  " Error is " <<  errno << std::endl;
      return -1;
  }
  return 0;
}

void uio_device::uio_write(size_t offset, size_t value){
    *((volatile uint32_t *)(d_mem + offset)) = value;
}

size_t uio_device::uio_read(size_t offset){
    return *((volatile uint32_t *)(d_mem + offset));
}

uint32_t* uio_device::get_ptr(size_t offset){
    return ((uint32_t *)(d_mem + offset));
}

void uio_device::close_dev(){
    munmap(d_mem, d_length);
}