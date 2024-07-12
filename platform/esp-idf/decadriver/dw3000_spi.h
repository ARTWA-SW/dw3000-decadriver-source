#ifndef DW3000_SPI_H
#define DW3000_SPI_H

#include <stdint.h>
#include <dw3000_hw.h>

#if ESP_PLATFORM
#include <sdkconfig.h>
#endif

#ifndef CONFIG_DW3000_SPI_TRACE
#define CONFIG_DW3000_SPI_TRACE 0
#endif

int dw3000_spi_init(const struct dw3000_hw_cfg* cfg);
void dw3000_spi_fini(void);
void dw3000_spi_wakeup(void);
void dw3000_spi_speed_slow(void);
void dw3000_spi_speed_fast(void);
int dw3000_spi_read(uint16_t headerLength, uint8_t* headerBuffer,
					uint16_t readLength, uint8_t* readBuffer);
int dw3000_spi_write(uint16_t headerLength, const uint8_t* headerBuffer,
					 uint16_t bodyLength, const uint8_t* bodyBuffer);
int dw3000_spi_write_crc(uint16_t headerLength, const uint8_t* headerBuffer,
						 uint16_t bodyLength, const uint8_t* bodyBuffer,
						 uint8_t crc8);

void dw3000_spi_trace_output(void);

#endif
