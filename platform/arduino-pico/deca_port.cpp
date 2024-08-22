#include <Arduino.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#include "deca_device_api.h"
#include "dw3000_hw.h"
#include "dw3000_spi.h"

/* This file implements the functions required by decadriver */

void wakeup_device_with_io(void) {
  /* this would only be used if you use the dwt_wakeup_ic() API, but it's
   * easier to directly call our own dw3000_hw_wakeup() function */
  dw3000_hw_wakeup();
}

decaIrqStatus_t decamutexon(void) {
  portENTER_CRITICAL();
  return 0;
}

void decamutexoff(decaIrqStatus_t s) { portEXIT_CRITICAL(); }

void deca_sleep(unsigned int time_ms) { vTaskDelay(pdMS_TO_TICKS(time_ms)); }

void deca_usleep(unsigned long time_us) { sleep_us(time_us); }

int readfromspi(uint16_t headerLength, uint8_t* headerBuffer, uint16_t readLength, uint8_t* readBuffer) {
  return dw3000_spi_read(headerLength, headerBuffer, readLength, readBuffer);
}

int writetospi(uint16_t headerLength, const uint8_t* headerBuffer, uint16_t bodyLength, const uint8_t* bodyBuffer) {
  return dw3000_spi_write(headerLength, headerBuffer, bodyLength, bodyBuffer);
}

int writetospiwithcrc(uint16_t headerLength, const uint8_t* headerBuffer, uint16_t bodyLength, const uint8_t* bodyBuffer, uint8_t crc8) {
  return dw3000_spi_write_crc(headerLength, headerBuffer, bodyLength, bodyBuffer, crc8);
}
