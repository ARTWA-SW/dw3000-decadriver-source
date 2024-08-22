#include <Arduino.h>
#include <SPI.h>

#include "deca_device_api.h"
#include "dw3000_spi.h"
// #include "log.h"

/* This file implements the SPI functions used by deca_port.c*/

static uint32_t operator"" _mhz(unsigned long long int val)
{
  return val * 1'000'000;
}

#define DW3000_SPI_INSTANCE SPI

const SPISettings _fastSPI = SPISettings(8_mhz, MSBFIRST, SPI_MODE0);
const SPISettings _slowSPI = SPISettings(2_mhz, MSBFIRST, SPI_MODE0);

static const char *LOG_TAG = "DW3000";
static SPIClassRP2040 *dw_spi = &DW3000_SPI_INSTANCE;
static const struct dw3000_hw_cfg *dw_hw_cfg;

const SPISettings *_currentSPI = &_fastSPI;

#if CONFIG_DW3000_SPI_TRACE
void dw3000_spi_trace_in(bool rw, const uint8_t *headerBuffer,
                         uint16_t headerLength, const uint8_t *bodyBuffer,
                         uint16_t bodyLength);
#endif

int dw3000_spi_init(const struct dw3000_hw_cfg *cfg)
{
  dw_hw_cfg = cfg;

  // LOG_INF("SPI Init (MOSI:%d MISO:%d CLK:%d CS:%d)", cfg->spi_mosi_pin,
  //         cfg->spi_miso_pin, cfg->spi_clk_pin, cfg->spi_cs_pin);

  dw_spi->setSCK(cfg->spi_clk_pin);
  dw_spi->setMOSI(cfg->spi_mosi_pin);
  dw_spi->setMISO(cfg->spi_miso_pin);
  dw_spi->setCS(cfg->spi_cs_pin);

  dw_spi->begin(false);

  /*
   * Slave select must be set as high before setting it as output, otherwise it can cause glitches
   */
  pinMode(cfg->spi_cs_pin, OUTPUT);
  digitalWrite(cfg->spi_cs_pin, HIGH);

  return DWT_SUCCESS;
}

int dw3000_spi_reinit(void) { return dw3000_spi_init(dw_hw_cfg); }

void dw3000_spi_speed_slow(void)
{
  _currentSPI = &_slowSPI;
}

void dw3000_spi_speed_fast(void)
{
  _currentSPI = &_fastSPI;
}

void dw3000_spi_fini(void)
{
  dw_spi->end();
}

int dw3000_spi_write_crc(uint16_t headerLength, const uint8_t *headerBuffer,
                         uint16_t bodyLength, const uint8_t *bodyBuffer,
                         uint8_t crc8)
{
  // LOG_ERR("WRITE WITH CRC NOT IMPLEMENTED!");
  return DWT_ERROR;
}

int dw3000_spi_write(uint16_t headerLength, const uint8_t *headerBuffer,
                     uint16_t bodyLength, const uint8_t *bodyBuffer)
{
  decaIrqStatus_t stat = decamutexon();

#if CONFIG_DW3000_SPI_TRACE
  dw3000_spi_trace_in(false, headerBuffer, headerLength, bodyBuffer,
                      bodyLength);
#endif

  dw_spi->beginTransaction(*_currentSPI);
  digitalWrite(dw_hw_cfg->spi_cs_pin, LOW);

  dw_spi->transfer((void *)headerBuffer, headerLength);
  dw_spi->transfer((void *)bodyBuffer, nullptr, bodyLength);
  deca_usleep(5);

  digitalWrite(dw_hw_cfg->spi_cs_pin, HIGH);
  dw_spi->endTransaction();

  decamutexoff(stat);
  return DWT_SUCCESS;
}

int dw3000_spi_read(uint16_t headerLength, uint8_t *headerBuffer,
                    uint16_t readLength, uint8_t *readBuffer)
{
  decaIrqStatus_t stat = decamutexon();

  dw_spi->beginTransaction(*_currentSPI);
  digitalWrite(dw_hw_cfg->spi_cs_pin, LOW);

  dw_spi->transfer((void *)headerBuffer, headerLength);
  dw_spi->transfer(nullptr, (void *)readBuffer, readLength);
  deca_usleep(5);

#if CONFIG_DW3000_SPI_TRACE
  dw3000_spi_trace_in(true, headerBuffer, headerLength, readBuffer,
                      readLength);
#endif

  digitalWrite(dw_hw_cfg->spi_cs_pin, HIGH);
  dw_spi->endTransaction();

  decamutexoff(stat);
  return DWT_SUCCESS;
}
