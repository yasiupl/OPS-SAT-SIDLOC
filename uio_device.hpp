#pragma once
#include <string>

class uio_device
{
private:
    std::string d_dev_name;
    uint32_t* d_mem;
    size_t d_length;
public:
    uio_device();
    ~uio_device();
    void set_dev_name(std::string dev_name);
    int open_dev(size_t length);
    void uio_write(size_t offset, size_t value);
    size_t uio_read(size_t offset);
    uint32_t* get_ptr(size_t offset);
    void close_dev();
};
