#pragma once

#include <AP_HAL/AP_HAL.h>
#if CONFIG_HAL_BOARD == HAL_BOARD_LINUX

#include <AP_HAL/I2CDevice.h>
#include <Filter/Filter.h>
#include <Filter/LowPassFilter2p.h>

#include "AP_InertialSensor.h"
#include "AP_InertialSensor_Backend.h"

class AP_InertialSensor_ITG3200BMA180 : public AP_InertialSensor_Backend
{
public:
    AP_InertialSensor_ITG3200BMA180(AP_InertialSensor &imu,
                               AP_HAL::OwnPtr<AP_HAL::I2CDevice> devacc,
                               AP_HAL::OwnPtr<AP_HAL::I2CDevice> devgyro);
    virtual ~AP_InertialSensor_ITG3200BMA180();

    // probe the sensors on I2C bus
    static AP_InertialSensor_Backend *probe(AP_InertialSensor &imu,
                                            AP_HAL::OwnPtr<AP_HAL::I2CDevice> devacc,
                                            AP_HAL::OwnPtr<AP_HAL::I2CDevice> devgyro);

    /* update accel and gyro state */
    bool update() override;

    void start(void) override;

private:
    bool _init_sensor();
    bool _accumulate();

    AP_HAL::OwnPtr<AP_HAL::I2CDevice> _devacc;
    AP_HAL::OwnPtr<AP_HAL::I2CDevice> _devgyro;

    // gyro and accel instances
    uint8_t _gyro_instance;
    uint8_t _accel_instance;

    uint32_t _last_gyro_timestamp;
    uint32_t _last_accel_timestamp;
};
#endif
