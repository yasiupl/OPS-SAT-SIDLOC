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
#include <stdint.h>
#include "util.hpp"
#include <thread>

descriptor::descriptor(size_t off_from_base, uint32_t source, uint32_t destination,
    uint32_t next_desc_ptr, uint32_t bytes_to_transfer, uint32_t write_offset) {
        d_base = off_from_base;
        d_source = source;
        d_destination = destination;
        d_next_desc_ptr = next_desc_ptr;
        d_bytes_to_transfer = bytes_to_transfer;
        d_write_offset = write_offset;
    };

descriptor::~descriptor(){}

void descriptor::descriptor_reset(uio_device uio_dev){
    uio_dev.uio_write(d_base + DESC_CS_REG, 0x80000000);
}

void descriptor::write_to_mem(uio_device uio_dev){
    uio_dev.uio_write(d_base + DESC_SOURCE, d_source);
    uio_dev.uio_write(d_base + DESC_DESTINATION, d_destination);
    uio_dev.uio_write(d_base + DESC_NXT_PTR, d_next_desc_ptr);
    uio_dev.uio_write(d_base + DESC_BYTES_TO_TX, d_bytes_to_transfer);
    uio_dev.uio_write(d_base + DESC_CS_REG, 0x80000000);
}

uint8_t descriptor::read_status(uio_device uio_dev){
    return  uio_dev.uio_read(d_base + DESC_CS_REG) >> 24;
}

uint16_t descriptor::read_bytes_txed(uio_device uio_dev){
    return  uio_dev.uio_read(d_base + DESC_CS_REG) && 0xff;
}

uint32_t descriptor::get_write_offset(){
    return d_write_offset;
}

