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
    size_t d_base;
public:
    descriptor(size_t base, uint32_t source, uint32_t destination,
            uint32_t next_desc_ptr, uint32_t bytes_to_transfer, uint32_t write_offset);
    ~descriptor();

    void descriptor_reset(uio_device uio_dev);
    void write_to_mem(uio_device uio_dev);
    uint16_t read_bytes_txed(uio_device uio_dev);
    uint8_t read_status(uio_device uio_dev);
    bool descriptor_finished();
    uint32_t get_write_offset();
};

