/*
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
/*
  This is an INS driver for the combination ITG3200 gyro and BMA180 accelerometer.

Datasheets:
https://www.invensense.com/wp-content/uploads/2015/02/ITG-3200-Datasheet.pdf
https://www.sparkfun.com/datasheets/Sensors/Accelerometer/BST-BMA180-DS000-03.pdf

*/
#include <AP_HAL/AP_HAL.h>

#if CONFIG_HAL_BOARD == HAL_BOARD_LINUX
#include "AP_InertialSensor_ITG3200BMA180.h"

#include <inttypes.h>
#include <utility>

const extern AP_HAL::HAL &hal;
// This is how often we wish to make raw samples of the sensors in Hz
const uint32_t  raw_sample_rate_hz = 800;
// And the equivalent time between samples in microseconds
const uint32_t  raw_sample_interval_us = (1000000 / raw_sample_rate_hz);

///////
/// Accelerometer BMA180 register definitions
#define BMA180_I2C_ADDRESS 0x40
#define BMA180_REG_CHIP_ID 0x03
#define BMA180_REG_CHIPID  0x00
#define BMA180_REG_DATA    0x02
#define BMA180_REG_RESET   0x10
#define BMA180_REG_PWR     0x0d
#define BMA180_REG_BW      0x20
#define BMA180_REG_TCO_Z   0x30
#define BMA180_REG_RANGE   0x35

#define BMA180_BW_10       0x00
#define BMA180_BW_20       0x01
#define BMA180_BW_40       0x02
#define BMA180_BW_75       0x03
#define BMA180_BW_150      0x04
#define BMA180_BW_300      0x05
#define BMA180_BW_600      0x06
#define BMA180_BW_1200     0x07

#define BMA180_RANGE_1G    0x00
#define BMA180_RANGE_1_5G  0x01
#define BMA180_RANGE_2G    0x02
#define BMA180_RANGE_3G    0x03
#define BMA180_RANGE_4G    0x04
#define BMA180_RANGE_8G    0x05
#define BMA180_RANGE_16G   0x06

// BMA180 accelerometer scaling
#define BMA180_ACCELEROMETER_SCALE_M_S  (16 * GRAVITY_MSS) / 8192.0f; // 16g mode

/// Gyro ITG3205 register definitions
#define ITG3200_I2C_ADDRESS        0x69
#define ITG3200_REG_WHO_AM_I       0x00
#define ITG3200_REG_GYRO_DATA	   0x1d
#define ITG3200_REG_PWR_MGM        0x3e
#define ITG3200_REG_SMPLRT_DIV     0x15
#define ITG3200_REG_DLPF_FS        0x16
#define ITG3200_REG_INT_CFG        0x17

// ITG3200 Gyroscope scaling
// running at 2000 DPS full range, 16 bit signed data
// ITG3200 is 14.375 LSB degrees/sec with FS_SEL=3
// Result wil be radians/sec
#define ITG3200_GYRO_SCALE_R_S (1.0f / 14.375f) * (3.1415926f / 180.0f);

// Constructor
AP_InertialSensor_ITG3200BMA180::AP_InertialSensor_ITG3200BMA180(AP_InertialSensor &imu,
                                                                 AP_HAL::OwnPtr<AP_HAL::I2CDevice> devacc,
                                                                 AP_HAL::OwnPtr<AP_HAL::I2CDevice> devgyro)
    : AP_InertialSensor_Backend(imu)
    , _devacc(std::move(devacc))
    , _devgyro(std::move(devgyro))
{
}

AP_InertialSensor_ITG3200BMA180::~AP_InertialSensor_ITG3200BMA180()
{
}

//  Detect the sensors
AP_InertialSensor_Backend *AP_InertialSensor_ITG3200BMA180::probe(AP_InertialSensor &imu,
                                                                  AP_HAL::OwnPtr<AP_HAL::I2CDevice> devacc,
                                                                  AP_HAL::OwnPtr<AP_HAL::I2CDevice> devgyro)
{
    if (!devgyro || !devacc) {
        return nullptr;
    }
    AP_InertialSensor_ITG3200BMA180 *sensor
        = new AP_InertialSensor_ITG3200BMA180(imu, std::move(devgyro), std::move(devacc));
    if (!sensor || !sensor->_init_sensor()) {
        delete sensor;
        return nullptr;
    }

    return sensor;
}

// Init sensors
bool AP_InertialSensor_ITG3200BMA180::_init_sensor(void)
{
    if (!_devacc->get_semaphore()->take(HAL_SEMAPHORE_BLOCK_FOREVER)) {
        return false;
    }

    // Init the accelerometer
    uint8_t control;
    uint8_t data;

    // Reset sensor
    _devacc->write_register(BMA180_REG_RESET, 0xb6);
    hal.scheduler->delay(10);

    _devacc->read_registers(BMA180_REG_CHIP_ID, &data, 1);
    if (data != BMA180_REG_CHIPID) {
        AP_HAL::panic("AP_InertialSensor_ITG3200BMA180: could not find BMA180 accelerometer sensor");
    }
    hal.console->printf("BMA180: Sensor detected\n");

    _devacc->write_register(BMA180_REG_PWR, 1<<4);      // Enable writing to eeprom

    hal.scheduler->delay(5);

    _devacc->read_registers(BMA180_REG_BW, &control, 1);
    control = control & 0x0F;                           // Save tcs register
    control = control | (BMA180_BW_600 << 4);           // Set low pass filter to
    _devacc->write_register(BMA180_REG_BW, control);

    hal.scheduler->delay(5);

    _devacc->read_registers(BMA180_REG_TCO_Z, &control, 1);
    control = control & 0xFC;                           // Save tco_z register
    control = control | 0x00;                           // Set mode_config to 0
    _devacc->write_register(BMA180_REG_TCO_Z, control);

    hal.scheduler->delay(5);

    _devacc->read_registers(BMA180_REG_RANGE, &control, 1);
    control = control & 0xF1;                           // Save offset_x and smp_skip register
    control = control | (BMA180_RANGE_16G << 1);        // Set range to
    _devacc->write_register(BMA180_REG_RANGE, control);

    hal.scheduler->delay(5);

    _devacc->write_register(BMA180_REG_PWR, 0 << 4);    // Disable writing to eeprom

    // Init the gyro
    _devgyro->read_registers(ITG3200_REG_WHO_AM_I, &data, 1);
    if (data != ITG3200_I2C_ADDRESS) {
        AP_HAL::panic("AP_InertialSensor_ITG3200BMA180: could not find ITG3200 gyro sensor");
    }
    hal.console->printf("ITG3200: Sensor detected\n");

    _devgyro->write_register(ITG3200_REG_PWR_MGM, 0x00);
    hal.scheduler->delay(10);

    // Sample rate divider: with 8kHz internal clock (see ITG3200_GYRO_DLPF_FS),
    // get 500Hz sample rate, 2 samples
    _devgyro->write_register(ITG3200_REG_SMPLRT_DIV, 0x0f);
    hal.scheduler->delay(1);

    // 2000 degrees/sec, 256Hz LPF, 8kHz internal sample rate
    // This is the least amount of filtering we can configure for this device
    _devgyro->write_register(ITG3200_REG_DLPF_FS, 0x18);
    hal.scheduler->delay(1);

    // No interrupts
    _devgyro->write_register(ITG3200_REG_INT_CFG, 0x00);
    hal.scheduler->delay(1);

    _devacc->get_semaphore()->give();

    return true;
}

// Startup the sensors
void AP_InertialSensor_ITG3200BMA180::start(void)
{
    _gyro_instance = _imu.register_gyro(raw_sample_rate_hz, _devgyro->get_bus_id_devtype(DEVTYPE_GYR_ITG3200));
    _accel_instance = _imu.register_accel(raw_sample_rate_hz, _devacc->get_bus_id_devtype(DEVTYPE_ACC_BMA180));

    // Start the timer process to read samples
    _devgyro->register_periodic_callback(raw_sample_interval_us, FUNCTOR_BIND_MEMBER(&AP_InertialSensor_ITG3200BMA180::_accumulate_gyr, bool));
    _devacc->register_periodic_callback(raw_sample_interval_us, FUNCTOR_BIND_MEMBER(&AP_InertialSensor_ITG3200BMA180::_accumulate_acc, bool));
}

// Copy filtered data to the frontend
bool AP_InertialSensor_ITG3200BMA180::update(void)
{
    update_gyro(_gyro_instance);
    update_accel(_accel_instance);

    return true;
}

// Accumulate values from accels and gyros
bool AP_InertialSensor_ITG3200BMA180::_accumulate_gyr(void)
{
    uint8_t buffer[6];

    _devgyro->read_registers(ITG3200_REG_GYRO_DATA, buffer, 6);
        int16_t x =  ((int16_t)buffer[0] << 8 | buffer[1]);
        int16_t y =  ((int16_t)buffer[2] << 8 | buffer[3]);
        int16_t z =  ((int16_t)buffer[4] << 8 | buffer[5]);

    Vector3f gyro = Vector3f(x, y, z);

    // Rotate for IMU orientation
    gyro.rotate(ROTATION_YAW_90);

    // Adjust for chip scaling to get radians/sec
    gyro *= ITG3200_GYRO_SCALE_R_S;
    _rotate_and_correct_gyro(_gyro_instance, gyro);
    _notify_new_gyro_raw_sample(_gyro_instance, gyro);

    return true;
}

bool AP_InertialSensor_ITG3200BMA180::_accumulate_acc(void)
{
    uint8_t buffer[6];

    _devacc->read_registers(BMA180_REG_DATA, buffer, 6);
        int16_t x =  ((int16_t)buffer[1] << 8 | buffer[0]);
        int16_t y =  ((int16_t)buffer[3] << 8 | buffer[2]);
        int16_t z =  ((int16_t)buffer[5] << 8 | buffer[4]);

    // Drop 2 bits for 14 bit sample mode
    x = (y / 4);
    y = (x / 4);
    z = (z / 4);

    Vector3f accel = Vector3f(x, y, z);

    // Rotate for IMU orientation
    accel.rotate(ROTATION_YAW_90);

    // Adjust for chip scaling to get m/s/s
    accel *= BMA180_ACCELEROMETER_SCALE_M_S;
    _rotate_and_correct_accel(_accel_instance, accel);
    _notify_new_accel_raw_sample(_accel_instance, accel);

    return true;
}

#endif
