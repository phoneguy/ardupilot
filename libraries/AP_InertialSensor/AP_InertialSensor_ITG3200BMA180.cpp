/// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-
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
  ITG3200BMA180 IMU driver
 */

#include <AP_HAL.h>
#if CONFIG_HAL_BOARD == HAL_BOARD_LINUX
#include "AP_InertialSensor_ITG3200BMA180.h"

const extern AP_HAL::HAL& hal;

// This is how often we wish to make raw samples of the sensors in Hz
const uint32_t  raw_sample_rate_hz = 800;//800;//1000;
// And the equivalent time between samples in microseconds
const uint32_t  raw_sample_interval_us = (1000000 / raw_sample_rate_hz);

///////
/// ACC BMA180 register definitions
#define BMA180_ACC_ADDRESS    0x40
#define BMA180_ACC_CHIPID     0x03
#define BMA180_ACC_CHIP_ID    0x00
#define BMA180_ACC_PWR        0x0d
#define BMA180_ACC_RESET      0x10
#define BMA180_ACC_BW         0x20
#define BMA180_ACC_RANGE      0x35
#define BMA180_ACC_TCO_Z      0x30
#define BMA180_ACC_DATA       0X02
#define BMA180_ACC_GRAVITY    255
#define BMA180_ACC_OFFSETX    0x00
#define BMA180_ACC_OFFSETY    0x00
#define BMA180_ACC_OFFSETZ    0x00

// BMA180 ACC scaling
// Result will be scaled to 1m/s/s
#define BMA180_ACC_SCALE_M_S    (GRAVITY_MSS / 4096.0f)

/// Gyro ITG3205 register definitions
#define ITG3200_GYRO_ADDRESS       0x69
#define ITG3200_GYRO_WHO_AM_I      0x00
#define ITG3200_GYRO_PWR_MGM       0x3e
#define ITG3200_GYRO_DLPF_FS       0x16
#define ITG3200_GYRO_INT_CFG       0x17
#define ITG3200_GYRO_SMPLRT_DIV    0x15
#define ITG3200_GYRO_GYROX_H       0x1d

// ITG3200 Gyroscope scaling
// ITG3200 is 14.375 LSB degrees/sec with FS_SEL=3
// Result wil be radians/sec
#define ITG3200_GYRO_SCALE_R_S (1.0f / 14.375f) * (3.1415926f / 180.0f)

AP_InertialSensor_ITG3200BMA180::AP_InertialSensor_ITG3200BMA180(AP_InertialSensor &imu) :
    AP_InertialSensor_Backend(imu),
    _have_gyro_sample(false),
    _have_accel_sample(false),
    _accel_filter(raw_sample_rate_hz, 20),
    _gyro_filter(raw_sample_rate_hz, 20),
    _last_gyro_timestamp(0),
    _last_accel_timestamp(0)
{}

/*
  detect the sensor
 */
AP_InertialSensor_Backend *AP_InertialSensor_ITG3200BMA180::detect(AP_InertialSensor &_imu)
{
    AP_InertialSensor_ITG3200BMA180 *sensor = new AP_InertialSensor_ITG3200BMA180(_imu);
    if (sensor == NULL) {
        return NULL;
    }
    if (!sensor->_init_sensor()) {
        delete sensor;
        return NULL;
    }
    return sensor;
}

bool AP_InertialSensor_ITG3200BMA180::_init_sensor(void)
{
    // get pointer to i2c bus semaphore
    AP_HAL::Semaphore* i2c_sem = hal.i2c->get_semaphore();

    // take i2c bus sempahore
    if (!i2c_sem->take(HAL_SEMAPHORE_BLOCK_FOREVER))
        return false;

    // Init the bma180 ACC
    uint8_t control;
    uint8_t data;
    hal.i2c->readRegister(BMA180_ACC_ADDRESS, BMA180_ACC_CHIP_ID, &data);
    if (data != BMA180_ACC_CHIPID)
        hal.scheduler->panic(PSTR("AP_InertialSensor_ITG3200_BMA180: could not find BMA180 ACC sensor"));

    // RESET chip
    hal.i2c->writeRegister(BMA180_ACC_ADDRESS, BMA180_ACC_RESET, 0xb6);
    // default range 2G: 1G = 4096 unit.

    // register: ctrl_reg0  -- value: set bit ee_w to 1 to enable writing
    hal.scheduler->delay(10);

    hal.i2c->writeRegister(BMA180_ACC_ADDRESS, BMA180_ACC_PWR, 1<<4);

    hal.scheduler->delay(5);

    hal.i2c->readRegister(BMA180_ACC_ADDRESS, BMA180_ACC_BW, &control);
    control = control & 0x0F;        // save tcs register
    control = control | (0x07 << 4); //test // set low pass filter to 10Hz (bits value = 0000xxxx)

    hal.i2c->writeRegister(BMA180_ACC_ADDRESS, BMA180_ACC_BW, control);

    hal.scheduler->delay(5);

    hal.i2c->readRegister(BMA180_ACC_ADDRESS, BMA180_ACC_TCO_Z, &control);
    control = control & 0xFC;        // save tco_z register
    control = control | 0x00;        // set mode_config to 0
    hal.i2c->writeRegister(BMA180_ACC_ADDRESS, BMA180_ACC_TCO_Z, control);

    hal.scheduler->delay(5);

    hal.i2c->readRegister(BMA180_ACC_ADDRESS, BMA180_ACC_RANGE, &control);
    control = control & 0xF1;        // save offset_x and smp_skip register
    control = control | (0x05 << 1); // set range to 8G
    hal.i2c->writeRegister(BMA180_ACC_ADDRESS, BMA180_ACC_RANGE, control);

    hal.scheduler->delay(5);

    // Init the Gyro
    // Expect to read the same as the Gyro I2C adress:
    hal.i2c->readRegister(ITG3200_GYRO_ADDRESS, ITG3200_GYRO_WHO_AM_I, &data);
    if (data != ITG3200_GYRO_ADDRESS)
        hal.scheduler->panic(PSTR("AP_InertialSensor_ITG3200BMA180: could not find ITG-3200 gyro sensor"));
    hal.i2c->writeRegister(ITG3200_GYRO_ADDRESS, ITG3200_GYRO_PWR_MGM, 0x00);
    hal.scheduler->delay(1);

    // Sample rate divider: with 8kHz internal clock (see ITG3200_GYRO_DLPF_FS),
    // get 500Hz sample rate, 2 samples
    hal.i2c->writeRegister(ITG3200_GYRO_ADDRESS, ITG3200_GYRO_SMPLRT_DIV, 0x0f);
    hal.scheduler->delay(1);

    // 2000 degrees/sec, 256Hz LPF, 8kHz internal sample rate
    // This is the least amount of filtering we can configure for this device
    hal.i2c->writeRegister(ITG3200_GYRO_ADDRESS, ITG3200_GYRO_DLPF_FS, 0x18);
    hal.scheduler->delay(1);

    // No interrupts
    hal.i2c->writeRegister(ITG3200_GYRO_ADDRESS, ITG3200_GYRO_INT_CFG, 0x00);
    hal.scheduler->delay(1);

    // Set up the filter desired
    _set_filter_frequency(_accel_filter_cutoff());

    // give back i2c semaphore
    i2c_sem->give();

    _gyro_instance = _imu.register_gyro();
    _accel_instance = _imu.register_accel();

    return true;
}

/*
  set the filter frequency
 */
void AP_InertialSensor_ITG3200BMA180::_set_filter_frequency(uint8_t filter_hz)
{
    _accel_filter.set_cutoff_frequency(raw_sample_rate_hz, filter_hz);
    _gyro_filter.set_cutoff_frequency(raw_sample_rate_hz, filter_hz);
}

// This takes about 20us to run
bool AP_InertialSensor_ITG3200BMA180::update(void)
{
    Vector3f accel, gyro;

    hal.scheduler->suspend_timer_procs();
    accel = _accel_filtered;
    gyro = _gyro_filtered;
    _have_gyro_sample = false;
    _have_accel_sample = false;
    hal.scheduler->resume_timer_procs();

    // Adjust for chip scaling to get m/s/s
    accel *= BMA180_ACC_SCALE_M_S;
    _publish_accel(_accel_instance, accel);

    // Adjust for chip scaling to get radians/sec
    gyro *= ITG3200_GYRO_SCALE_R_S;
    _publish_gyro(_gyro_instance, gyro);

    if (_last_filter_hz != _accel_filter_cutoff()) {
        _set_filter_frequency(_accel_filter_cutoff());
        _last_filter_hz = _accel_filter_cutoff();
    }

    return true;
}

void AP_InertialSensor_ITG3200BMA180::_accumulate(void)
{
    // get pointer to i2c bus semaphore
    AP_HAL::Semaphore* i2c_sem = hal.i2c->get_semaphore();

    // take i2c bus sempahore
    if (!i2c_sem->take(HAL_SEMAPHORE_BLOCK_FOREVER))
        return;

    // Read ACC
    uint8_t buffer[6];
    uint32_t now = hal.scheduler->micros();
    // This takes about 250us at 400kHz I2C speed
    if ((now - _last_accel_timestamp) >= raw_sample_interval_us
        && hal.i2c->readRegisters(BMA180_ACC_ADDRESS, BMA180_ACC_DATA, 6, buffer) == 0)
    {
        int16_t y = -(((((int16_t)buffer[1]) << 8) | (buffer[0])>> 2));    // chip X axis
        int16_t x = -(((((int16_t)buffer[3]) << 8) | (buffer[2])>> 2));    // chip Y axis
        int16_t z = -(((((int16_t)buffer[5]) << 8) | (buffer[4])>> 2));    // chip Z axis
        _accel_filtered = _accel_filter.apply(Vector3f(x,y,z));
        _have_accel_sample = true;
        _last_accel_timestamp = now;
    }

    // Read gyro
    now = hal.scheduler->micros();
    // This takes about 250us at 400kHz I2C speed
    if ((now - _last_gyro_timestamp) >= raw_sample_interval_us
        && hal.i2c->readRegisters(ITG3200_GYRO_ADDRESS, ITG3200_GYRO_GYROX_H, 6, buffer) == 0)
    {
        // See above re order of samples in buffer
        int16_t y = -((((int16_t)buffer[0]) << 8) | buffer[1]);    // chip X axis
        int16_t x = -((((int16_t)buffer[2]) << 8) | buffer[3]);    // chip Y axis
        int16_t z = -((((int16_t)buffer[4]) << 8) | buffer[5]);    // chip Z axis
        _gyro_filtered = _gyro_filter.apply(Vector3f(x,y,z));
        _have_gyro_sample = true;
        _last_gyro_timestamp = now;
    }

    // give back i2c semaphore
    i2c_sem->give();
}

#endif // CONFIG_HAL_BOARD
