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

class dma_device
{
private:
  uio_device d_uio;

public:
  dma_device();
  ~dma_device();
  void
  set_uio_device(uio_device uio);
  void
  set_nex_dext_ptr(uint32_t addr);
  bool
  get_busy();
  bool
  get_chain_cmplt();
  bool
  get_desc_cmplt();
  bool
  get_error();
  uint32_t
  get_dma_status();
  void
  reset_dma();
  uint32_t
  get_dma_ctrl();
  void
  set_dma_ctrl(uint32_t cntrl);
  void
  set_dma_status(uint32_t status);
};
