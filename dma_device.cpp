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