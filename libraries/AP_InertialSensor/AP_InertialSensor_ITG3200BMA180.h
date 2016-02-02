/// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-
#pragma once

#include <AP_HAL/AP_HAL.h>
#if CONFIG_HAL_BOARD == HAL_BOARD_LINUX
#include <Filter/Filter.h>
#include <Filter/LowPassFilter2p.h>
#include "AP_InertialSensor.h"
#include "AP_InertialSensor_Backend.h"

class AP_InertialSensor_ITG3200BMA180 : public AP_InertialSensor_Backend
{
public:
    AP_InertialSensor_ITG3200BMA180(AP_InertialSensor &imu);

    /* update accel and gyro state */
    bool update();

    // accumulate samples
    void accumulate(void) override;

    // detect the sensor
    static AP_InertialSensor_Backend *detect(AP_InertialSensor &imu);

private:
    bool        _init_sensor(void);

    uint8_t _gyro_instance;
    uint8_t _accel_instance;

    uint32_t _last_gyro_timestamp;
    uint32_t _last_accel_timestamp;

    enum Rotation _default_rotation;
};
#endif
