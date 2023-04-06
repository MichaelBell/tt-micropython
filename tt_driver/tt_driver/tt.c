#include <stdint.h>

#include "tt.pio.h"
#include "hardware/dma.h"

static uint tt_out_sm, tt_in_sm;
static uint tt_out_dma_ch, tt_in_dma_ch;

#define MAX_TRANSFERS 256
static uint16_t tt_out_data[MAX_TRANSFERS];

static uint8_t tt_in_data;

void tt_pio_init() {
    uint offset = pio_add_program(pio0, &tt_out_program);
    tt_out_sm = pio_claim_unused_sm(pio0, true);
    tt_out_program_init(pio0, tt_out_sm, offset, 13, 10);

    offset = pio_add_program(pio0, &tt_in_program);
    tt_in_sm = pio_claim_unused_sm(pio0, true);
    tt_in_program_init(pio0, tt_in_sm, offset, 8);

    tt_out_dma_ch = dma_claim_unused_channel(true);
    tt_in_dma_ch = dma_claim_unused_channel(true);

    dma_channel_config c = dma_channel_get_default_config(tt_out_dma_ch);
    channel_config_set_read_increment(&c, true);
    channel_config_set_write_increment(&c, false);
    channel_config_set_dreq(&c, pio_get_dreq(pio0, tt_out_sm, true));
    channel_config_set_transfer_data_size(&c, DMA_SIZE_16);

    dma_channel_configure(
        tt_out_dma_ch, &c,
        &pio0->txf[tt_out_sm],
        tt_out_data,
        0,
        false
    );

    c = dma_channel_get_default_config(tt_in_dma_ch);
    channel_config_set_read_increment(&c, false);
    channel_config_set_write_increment(&c, false);
    channel_config_set_dreq(&c, pio_get_dreq(pio0, tt_in_sm, false));
    channel_config_set_transfer_data_size(&c, DMA_SIZE_8);

    dma_channel_configure(
        tt_in_dma_ch, &c,
        &tt_in_data,
        &pio0->rxf[tt_in_sm],
        0,
        false
    );
}

uint8_t tt_send_byte(uint8_t b, bool latch, bool scan) {
    dma_channel_wait_for_finish_blocking(tt_in_dma_ch);

    uint32_t val = b;
    if (latch) val |= 0x100;
    if (scan) val |= 0x200;
    pio_sm_put_blocking(pio0, tt_out_sm, val << 22);
    return (uint8_t)pio_sm_get_blocking(pio0, tt_in_sm);
}

static uint8_t send_data_internal(uint32_t len, bool read) {
    dma_channel_transfer_from_buffer_now(tt_out_dma_ch, tt_out_data, len);

    dma_channel_wait_for_finish_blocking(tt_in_dma_ch);
    dma_hw->ch[tt_in_dma_ch].al1_transfer_count_trig = len;

    if (read) {
        dma_channel_wait_for_finish_blocking(tt_in_dma_ch);
        return tt_in_data;
    }

    return 0;
}

uint8_t tt_send_byte_repeat(uint8_t b, uint32_t repeats, bool read, bool latch, bool scan) {
    dma_channel_wait_for_finish_blocking(tt_out_dma_ch);

    uint16_t val = b;
    if (latch) val |= 0x100;
    val <<= 6;
    if (scan) {
        tt_out_data[0] = val | 0x8000;
    }
    else {
        tt_out_data[0] = val;
    }

    if (repeats > MAX_TRANSFERS) repeats = MAX_TRANSFERS;
    for (uint32_t i = 1; i < repeats; ++i) {
        tt_out_data[i] = val;
    }

    return send_data_internal(repeats, read);
}

uint8_t tt_send_bytes(uint8_t* data, uint32_t len, bool read, bool latch, bool scan) {
    dma_channel_wait_for_finish_blocking(tt_out_dma_ch);

    uint16_t val = data[0];
    if (latch) val |= 0x100;
    val <<= 6;
    if (scan) {
        tt_out_data[0] = val | 0x8000;
    }
    else {
        tt_out_data[0] = val;
    }

    val = (latch ? 0x100 : 0);

    if (len > MAX_TRANSFERS) len = MAX_TRANSFERS;
    for (uint32_t i = 1; i < len; ++i) {
        tt_out_data[i] = ((uint16_t)data[i] | val) << 6;
    }

    return send_data_internal(len, read);
}
