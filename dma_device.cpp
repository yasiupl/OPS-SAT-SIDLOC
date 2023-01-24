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

#include "dma_device.hpp"
#include "util.hpp"

dma_device ::dma_device ()
{}

dma_device ::~dma_device ()
{}

void dma_device::set_uio_device(uio_device uio){
    d_uio = uio;
}

void dma_device ::set_nex_dext_ptr(uint32_t addr){
    d_uio.uio_write(DMA_NXT_DESC_PTR, addr);
}

bool dma_device ::get_busy(){
    return d_uio.uio_read(DMA_STATUS) & STS_BUSY;
}

bool dma_device ::get_chain_cmplt(){
    return d_uio.uio_read(DMA_STATUS) & STS_CHAIN_COMPLETED;
}

bool dma_device ::get_desc_cmplt(){
    return d_uio.uio_read(DMA_STATUS) & STS_DESCRIPTOR_COMPLETED;
}

bool dma_device ::get_error(){
    return d_uio.uio_read(DMA_STATUS) & STS_ERROR;  
}

uint32_t dma_device ::get_dma_status(){
    return d_uio.uio_read(DMA_STATUS);  
}

void dma_device ::reset_dma(){
    d_uio.uio_write(DMA_CTRL, SW_RESET);
    d_uio.uio_write(DMA_CTRL, SW_RESET);
}
void dma_device ::set_dma_ctrl(uint32_t cntrl){
    d_uio.uio_write(DMA_CTRL, cntrl);
}
void dma_device ::set_dma_status(uint32_t status){
    d_uio.uio_write(DMA_STATUS, status);
}

uint32_t dma_device ::get_dma_ctrl(){
    return d_uio.uio_read(DMA_CTRL);
}