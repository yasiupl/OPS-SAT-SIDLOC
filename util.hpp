#pragma once
enum STATUS {
    STS_BUSY                    = (1 << 4),
    STS_CHAIN_COMPLETED         = (1 << 3),
    STS_DESCRIPTOR_COMPLETED    = (1 << 2),
    STS_EOP_ENCOUNTERED         = (1 << 1),
    STS_ERROR                   = (1 << 0)
};

enum CONTROL {
    CLEAR_INTERRUPT             = (1 << 31),
    TIMEOUT_COUNTER             = (0x3ff << 20),
    DESC_POLL_EN                = (1 << 18),
    PARK                        = (1 << 17),
    SW_RESET                    = (1 << 16),
    MAX_DESC_PROCESSED          = (0xff << 8),
    IE_MAX_DESC_PROCESSED       = (1 << 7),
    STOP_DMA_ER                 = (1 << 6),
    RUN                         = (1 << 5),
    IE_GLOBAL                   = (1 << 4),
    IE_CHAIN_COMPLETED          = (1 << 3),
    IE_DESCRIPTOR_COMPLETED     = (1 << 2),
    IE_EOP_ENCOUNTERED          = (1 << 1),
    IE_ERROR                    = (1 << 0)
};

enum DESC_CTRL{
    OWNED_BY_HW                 = (1 << 7),
    AVALON_ST_CHANNEL_NUMBER    = (0xf << 3),
    WRITE_FIXED_ADDRESS         = (1 << 2),
    READ_FIXED_ADDRESS          = (1 << 1),
    GENERATE_EOP                = (1 << 0)
};

enum DESC_CS_REG{
    DESC_CONTROL            = (0xff << 24),
    DESC_STATUS             = (0xff << 16),
    ACT_BYTES_TX            = (0xffff << 0)
};

enum DMA_REG_OFFSETS{
    DMA_STATUS              = 0,
    DMA_VERSION             = 1,
    DMA_CTRL                = 4,
    DMA_NXT_DESC_PTR        = 8
};

enum DESCRIPTOR_OFFSETS{
    DESC_SOURCE             = 0,
    DESC_DESTINATION        = 2,
    DESC_NXT_PTR            = 4,
    DESC_BYTES_TO_TX        = 6,
    DESC_CS_REG             = 7
};