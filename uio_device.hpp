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
#include <string>

class uio_device
{
private:
  std::string d_dev_name;
  uint32_t   *d_mem;
  size_t      d_length;

public:
  uio_device();
  ~uio_device();
  void
  set_dev_name(std::string dev_name);
  int
  open_dev(size_t length);
  void
  uio_write(size_t offset, size_t value);
  size_t
  uio_read(size_t offset);
  uint32_t *
  get_ptr(size_t offset);
  void
  close_dev();
};
