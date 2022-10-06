#pragma once
#include "uio_device.hpp"

class dma_device 
{
private:
    uio_device d_uio;
public:
    dma_device ();
    ~dma_device ();
    void set_uio_device(uio_device uio);
    void set_nex_dext_ptr(uint32_t addr);
    bool get_busy();
    bool get_chain_cmplt();
    bool get_desc_cmplt();
    bool get_error();
    uint32_t get_dma_status();
    void reset_dma();
    uint32_t get_dma_ctrl();
    void set_dma_ctrl(uint32_t cntrl);
    void set_dma_status(uint32_t status);
};


