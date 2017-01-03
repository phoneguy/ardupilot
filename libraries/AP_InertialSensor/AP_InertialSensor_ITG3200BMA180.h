#pragma once

#include <AP_HAL/AP_HAL.h>
#if CONFIG_HAL_BOARD == HAL_BOARD_LINUX

#include <AP_HAL/I2CDevice.h>
#include "AP_InertialSensor.h"
#include "AP_InertialSensor_Backend.h"

class AP_InertialSensor_ITG3200BMA180 : public AP_InertialSensor_Backend
{
public:
    virtual ~AP_InertialSensor_ITG3200BMA180();
    // Update accel and gyro state
    void start(void) override;
    bool update() override;

    static AP_InertialSensor_Backend *probe(AP_InertialSensor &imu,
		                            AP_HAL::OwnPtr<AP_HAL::I2CDevice> devgyro,
                		            AP_HAL::OwnPtr<AP_HAL::I2CDevice> devacc,
                               		    enum Rotation rotation_g = ROTATION_NONE,
                                            enum Rotation rotation_a = ROTATION_NONE);

private:
    // Probe the sensors on I2C bus
    AP_InertialSensor_ITG3200BMA180(AP_InertialSensor &imu,
                                            AP_HAL::OwnPtr<AP_HAL::I2CDevice> devgyro,
                                            AP_HAL::OwnPtr<AP_HAL::I2CDevice> devacc,
                                            enum Rotation rotation_g,
                                            enum Rotation rotation_a);
    bool _init_sensor();
    bool _accumulate_gyr();
    bool _accumulate_acc();

    enum Rotation _rotation_g;
    enum Rotation _rotation_a;

    AP_HAL::OwnPtr<AP_HAL::I2CDevice> _devgyro;
    AP_HAL::OwnPtr<AP_HAL::I2CDevice> _devacc;

    // Gyro and accel instances
    uint8_t _gyro_instance;
    uint8_t _accel_instance;
};
#endif
