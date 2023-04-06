#pragma once

void tt_pio_init();
uint8_t tt_send_byte(uint8_t b, bool latch, bool scan);

// On these calls if read is set then they do not block and the write continues asynchronously
uint8_t tt_send_byte_repeat(uint8_t b, uint32_t repeats, bool read, bool latch, bool scan);
uint8_t tt_send_bytes(uint8_t* data, uint32_t len, bool read, bool latch, bool scan);
