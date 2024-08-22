#include <Arduino.h>
#include "color.h"

#define CRLF "\r\n"

#define UINT64_FMT "%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X"
#define UINT16_FMT "%02X:%02X"

#ifndef DEBUG_RP2040_PORT
  #define LOG_BASE(...) \
    do {                \
    } while (0)
#else
  #define LOG_BASE(level, color, tag, fmt, ...)                                                \
    do {                                                                                       \
      DEBUG_RP2040_PORT.printf(color level " [%s] " fmt COLOR_RESET CRLF, tag, ##__VA_ARGS__); \
      DEBUG_RP2040_PORT.flush();                                                               \
    } while (0)
#endif

#define LOG_ERR(fmt, ...)  LOG_BASE("E", COLOR_RED, LOG_TAG, fmt, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...) LOG_BASE("W", COLOR_YELLOW, LOG_TAG, fmt, ##__VA_ARGS__)
#define LOG_INF(fmt, ...)  LOG_BASE("I", COLOR_GREEN, LOG_TAG, fmt, ##__VA_ARGS__)
#define LOG_DBG(fmt, ...)  LOG_BASE("D", COLOR_BLUE, LOG_TAG, fmt, ##__VA_ARGS__)

/* TODO */
#define LOG_HEXDUMP(...) \
  do {                   \
  } while (0)

/* TODO */
#define DBG_UWB(...) \
  do {               \
  } while (0)
