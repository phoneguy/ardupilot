/// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#ifndef __AP_INERTIAL_SENSOR_ITG3200BMA180_H__
#define __AP_INERTIAL_SENSOR_ITG3200BMA180_H__

#include <AP_HAL/AP_HAL.h>
#include "AP_InertialSensor.h"
#include <Filter/Filter.h>
#include <Filter/LowPassFilter2p.h>

class AP_InertialSensor_ITG3200BMA180 : public AP_InertialSensor_Backend
{
public:
    AP_InertialSensor_ITG3200BMA180(AP_InertialSensor &imu);

    // update accel and gyro state
    bool update();

    bool gyro_sample_available(void) { _accumulate(); return _have_gyro_sample; }
    bool accel_sample_available(void) { _accumulate(); return _have_accel_sample; }

    // detect the sensor
    static AP_InertialSensor_Backend *detect(AP_InertialSensor &imu);

private:
    bool        _init_sensor(void);
    void        _accumulate(void);
    Vector3f	_accel_filtered;
    Vector3f	_gyro_filtered;
    bool        _have_gyro_sample;
    bool        _have_accel_sample;

    // support for updating filter at runtime
    int16_t _last_gyro_filter_hz;
    int16_t _last_accel_filter_hz;

    // change the filter frequency
    void _set_accel_filter(uint8_t filter_hz);
    void _set_gyro_filter(uint8_t filter_hz);

    // low Pass filters for gyro and accel
    LowPassFilter2pVector3f _accel_filter;
    LowPassFilter2pVector3f _gyro_filter;

    uint8_t _gyro_instance;
    uint8_t _accel_instance;

    uint32_t _last_gyro_timestamp;
    uint32_t _last_accel_timestamp;
};
#endif // __AP_INERTIAL_SENSOR_ITG3200BMA180_H__
