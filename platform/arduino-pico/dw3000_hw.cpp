#include <Arduino.h>
#include <FreeRTOS.h>
#include <task.h>

#include "deca_device_api.h"
#include "dw3000_hw.h"
#include "dw3000_spi.h"
// #include "log.h"

static const char* LOG_TAG = "DW3000";
static const struct dw3000_hw_cfg* dw_hw_cfg;

int dw3000_hw_init(const struct dw3000_hw_cfg* cfg) {
  // LOG_INF("HW Init (RESET:%d WAKEUP:%d IRQ:%d)", cfg->reset_pin, cfg->wakeup_pin, cfg->irq_pin);

  dw_hw_cfg = cfg;

  /*
   * RESET: output low, open drain, no pull-up
   * normally used as input to see when DW3000 is ready
   */
  if (cfg->reset_pin != -1) {
    pinMode(cfg->reset_pin, INPUT);

    /* check reset state */
    int timeout = 1000;
    while (digitalRead(cfg->reset_pin) == LOW && --timeout > 0) {
      vTaskDelay(1);
    }
    if (timeout <= 0) {
      return DWT_ERROR;
    }
  }

  /*
   * WAKEUP: output high
   */
  if (cfg->wakeup_pin != -1) {
    pinMode(cfg->wakeup_pin, OUTPUT);
    digitalWrite(cfg->wakeup_pin, LOW);
  }

  return dw3000_spi_init(cfg);
}

int dw3000_hw_reinit(void) { return dw3000_hw_init(dw_hw_cfg); }

// TODO !!!!!!!!!!!!!!!
static void dw3000_isr(void* args) {
  while (digitalRead(dw_hw_cfg->irq_pin)) {
    dwt_isr();
  }
}

int dw3000_hw_init_interrupt(void) {
  if (dw_hw_cfg->irq_pin == -1) {
    // LOG_ERR("IRQ pin is not defined");
    return DWT_ERROR;
  }

  pinMode(dw_hw_cfg->irq_pin, INPUT);
  attachInterruptParam(digitalPinToInterrupt(dw_hw_cfg->irq_pin), dw3000_isr, CHANGE, nullptr);

  return DWT_SUCCESS;
}

void dw3000_hw_interrupt_enable(void) {
  if (dw_hw_cfg->irq_pin != -1) {
    irq_set_enabled(dw_hw_cfg->irq_pin, true);
  }
}

void dw3000_hw_interrupt_disable(void) {
  if (dw_hw_cfg->irq_pin != -1) {
    irq_set_enabled(dw_hw_cfg->irq_pin, false);
  }
}

bool dw3000_hw_interrupt_is_enabled(void) {
  if (dw_hw_cfg->irq_pin != -1) {
    return irq_is_enabled(dw_hw_cfg->irq_pin);
  } else {
    return false;
  }
}

void dw3000_hw_fini(void) {
  // LOG_INF("HW fini");

  if (dw_hw_cfg->irq_pin != -1) {
    detachInterrupt(dw_hw_cfg->irq_pin);
  }

  dw3000_spi_fini();
}

void dw3000_hw_reset(void) {
  if (dw_hw_cfg->reset_pin == -1) {
    // LOG_ERR("Reset pin is not defined");
    return;
  }

  // LOG_INF("HW reset");
  pinMode(dw_hw_cfg->reset_pin, OUTPUT);
  digitalWrite(dw_hw_cfg->reset_pin, LOW);
  vTaskDelay(1);
  digitalWrite(dw_hw_cfg->reset_pin, HIGH);
  vTaskDelay(2);
  pinMode(dw_hw_cfg->reset_pin, INPUT);
  vTaskDelay(10);
}

/** wakeup either using the WAKEUP pin or SPI CS */
void dw3000_hw_wakeup(void) {
  if (dw_hw_cfg->wakeup_pin != -1) {
    /* Use WAKEUP pin if available */
    // LOG_INF("WAKEUP PIN");
    digitalWrite(dw_hw_cfg->wakeup_pin, HIGH);
    vTaskDelay(1); // 500 usec
    digitalWrite(dw_hw_cfg->wakeup_pin, LOW);
  } else {
    /* Use SPI CS pin */
    // LOG_INF("WAKEUP CS");
    digitalWrite(dw_hw_cfg->spi_cs_pin, LOW);
    vTaskDelay(1); // 500 usec
    digitalWrite(dw_hw_cfg->spi_cs_pin, HIGH);
  }
  vTaskDelay(1);
}

/** set WAKEUP pin low if available */
void dw3000_hw_wakeup_pin_low(void) {
  if (dw_hw_cfg->wakeup_pin != -1) {
    digitalWrite(dw_hw_cfg->wakeup_pin, LOW);
  }
}
