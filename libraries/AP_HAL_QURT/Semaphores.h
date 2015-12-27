#pragma once

#include <AP_HAL/AP_HAL_Boards.h>

#if CONFIG_HAL_BOARD == HAL_BOARD_QURT
#include "AP_HAL_QURT.h"
#include <pthread.h>

class QURT::Semaphore : public AP_HAL::Semaphore {
public:
    Semaphore() {
        HAP_PRINTF("%s constructor", __FUNCTION__);
        pthread_mutex_init(&_lock, NULL);
        HAP_PRINTF("%s constructor2", __FUNCTION__);
    }
    bool give();
    bool take(uint32_t timeout_ms);
    bool take_nonblocking();
private:
    pthread_mutex_t _lock;
};
#endif // CONFIG_HAL_BOARD
