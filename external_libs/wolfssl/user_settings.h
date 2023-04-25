#ifndef wolfcrypt_user_settings_h
#define wolfcrypt_user_settings_h

#ifdef ESP_PLATFORM

#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <esp_system.h>
#include <esp_random.h>

static inline int hwrand_generate_block(uint8_t *buf, size_t len) {
    int i;
    for (i=0; i+4 < len; i+=4) {
        *((uint32_t*)buf) = esp_random();
        buf += 4;
    }
    if (i < len) {
        uint32_t r = esp_random();
        while (i < len) {
            *buf++ = r;
            r >>= 8;
            i++;
        }
    }

    return 0;
}

#else   // ESP_OPEN_RTOS

#include <esp/hwrand.h>

static inline int hwrand_generate_block(uint8_t *buf, size_t len) {
    hwrand_fill(buf, len);
    return 0;
}

#endif

#define FREERTOS
#define WC_NO_HARDEN
#define NO_WOLFSSL_DIR
#define SINGLE_THREADED
#define WOLFSSL_LWIP
#define NO_INLINE

#define NO_WOLFSSL_MEMORY
#define NO_WOLFSSL_SMALL_STACK
#define MP_LOW_MEM

#define CUSTOM_RAND_GENERATE_BLOCK hwrand_generate_block

#endif
