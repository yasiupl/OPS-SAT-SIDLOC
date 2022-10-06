#include "uio_device.hpp"
#include <fcntl.h>
#include <cstdlib>
#include <cstdint>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/mman.h>

uio_device::uio_device(){ 
}
uio_device::~uio_device(){}

void uio_device::set_dev_name(std::string dev_name){
    d_dev_name = dev_name;
}

int uio_device::open_dev(size_t length){
  int uiofd = open(d_dev_name.c_str(), O_RDWR);
  //size_t length = 0x00001000;
  d_length = length;
  off_t offset = 0x0;
  if (uiofd < 0) {
      return -1;
    }

  d_mem = (uint32_t*)mmap(NULL, length, PROT_READ|PROT_WRITE, MAP_SHARED, uiofd, offset);
  if (d_mem == MAP_FAILED) { 
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