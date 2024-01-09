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

class descriptor
{
private:
  uint32_t d_source;
  uint32_t d_res1;
  uint32_t d_destination;
  uint32_t d_res2;
  uint32_t d_next_desc_ptr;
  uint32_t d_res3;
  uint32_t d_bytes_to_transfer;
  uint32_t d_cs_reg;
  uint32_t d_write_offset;
  size_t   d_base;

public:
  descriptor(size_t base, uint32_t source, uint32_t destination,
             uint32_t next_desc_ptr, uint32_t bytes_to_transfer,
             uint32_t write_offset);
  ~descriptor();

  void
  descriptor_reset(uio_device uio_dev);
  void
  write_to_mem(uio_device uio_dev);
  uint16_t
  read_bytes_txed(uio_device uio_dev);
  uint8_t
  read_status(uio_device uio_dev);
  bool
  descriptor_finished();
  uint32_t
  get_write_offset();
  uint32_t
  get_mem_offset();
};
