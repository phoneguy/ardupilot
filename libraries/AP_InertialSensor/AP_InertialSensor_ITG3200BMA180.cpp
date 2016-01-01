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

// Interface to the gyro and accel sensors:
// ITG3200 Gyroscope  http://www.sparkfun.com/datasheets/Sensors/Gyro/PS-ITG-3200-00-01.4.pdf
// BMA180 Accelerometer http://irtfweb.ifa.hawaii.edu/~tcs3/jumpman/jumppc/1107-BMA180/BMA180-DataSheet-v2.5.pdf

#include <AP_HAL/AP_HAL.h>
#if CONFIG_HAL_BOARD == HAL_BOARD_LINUX

#include "AP_InertialSensor_ITG3200BMA180.h"

const extern AP_HAL::HAL& hal;

// This is how often we wish to make raw samples of the sensors in Hz
const uint32_t  raw_sample_rate_hz = 800;
// And the equivalent time between samples in microseconds
const uint32_t  raw_sample_interval_us = (1000000 / raw_sample_rate_hz);

/// ACC BMA180 register definitions
#define BMA180_ADDRESS    0x40
#define BMA180_CHIPID     0x03
#define BMA180_CHIP_ID    0x00
#define BMA180_PWR        0x0d
#define BMA180_RESET      0x10
#define BMA180_BW_REG     0x20
#define BMA180_RANGE_REG  0x35
#define BMA180_TCO_Z      0x30
#define BMA180_DATA       0x02
#define BMA180_GRAVITY    255
#define BMA180_CTRL_REG0  0x0d
#define BMA180_CTRL_REG1  0x0e
#define BMA180_CTRL_REG2  0x0f

#define BMA180_BW_10      0x00
#define BMA180_BW_20      0x01
#define BMA180_BW_40      0x02
#define BMA180_BW_75      0x03
#define BMA180_BW_150     0x04
#define BMA180_BW_300     0x05
#define BMA180_BW_600     0x06
#define BMA180_BW_1200    0x07

#define BMA180_RANGE_1G   0x00
#define BMA180_RANGE_1_5G 0x01
#define BMA180_RANGE_2G   0x02
#define BMA180_RANGE_3G   0x03
#define BMA180_RANGE_4G   0x04
#define BMA180_RANGE_8G   0x05
#define BMA180_RANGE_16G  0x06

// BMA180 ACC scaling for 16g 1.98 mg/LSB 14 bit mode
// Result will be scaled to 1m/s/s
#define BMA180_ACC_SCALE_M_S    (GRAVITY_MSS / 2303); // 4096 is 8 g

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

#if CONFIG_HAL_BOARD_SUBTYPE == HAL_BOARD_SUBTYPE_LINUX_BBB
    _default_rotation(ROTATION_NONE)
#else
    _default_rotation(ROTATION_NONE)
#endif

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

    // Init the bma180 accelerometer
    uint8_t control;
    uint8_t data;
    hal.i2c->readRegister(BMA180_ADDRESS, BMA180_CHIP_ID, &data);
    if (data != BMA180_CHIPID)
        AP_HAL::panic("AP_InertialSensor_ITG3200_BMA180: could not find BMA180 ACC sensor");

    // RESET chip
    hal.i2c->writeRegister(BMA180_ADDRESS, BMA180_RESET, 0xb6);

    hal.scheduler->delay(10);

    hal.i2c->writeRegister(BMA180_ADDRESS, BMA180_PWR, 1<<4);    // enable writing

    hal.scheduler->delay(5);

    hal.i2c->readRegister(BMA180_ADDRESS, BMA180_BW_REG, &control);
    control = control & 0x0F;        // save tcs register
    control = control | (BMA180_BW_300 << 4); // set low pass filter to
    hal.i2c->writeRegister(BMA180_ADDRESS, BMA180_BW_REG, control);
    hal.scheduler->delay(5);

    hal.i2c->readRegister(BMA180_ADDRESS, BMA180_TCO_Z, &control);
    control = control & 0xFC;        // save tco_z register
    control = control | 0x00;        // set mode_config to 0
    hal.i2c->writeRegister(BMA180_ADDRESS, BMA180_TCO_Z, control);
    hal.scheduler->delay(5);

    hal.i2c->readRegister(BMA180_ADDRESS, BMA180_RANGE_REG, &control);
    control = control & 0xF1;                    // save offset_x and smp_skip register
    control = control | (BMA180_RANGE_16G << 1); // set range to
    hal.i2c->writeRegister(BMA180_ADDRESS, BMA180_RANGE_REG, control);
    hal.scheduler->delay(5);

    // Init the Gyro
    // Expect to read the same as the Gyro I2C adress:
    hal.i2c->readRegister(ITG3200_GYRO_ADDRESS, ITG3200_GYRO_WHO_AM_I, &data);
    if (data != ITG3200_GYRO_ADDRESS)
        AP_HAL::panic("AP_InertialSensor_ITG3200BMA180: could not find ITG-3200 gyro sensor");
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

    // give back i2c semaphore
    i2c_sem->give();

    _gyro_instance = _imu.register_gyro(raw_sample_rate_hz);
    _accel_instance = _imu.register_accel(raw_sample_rate_hz);

    _product_id = AP_PRODUCT_ID_DROTEK10DOF;

    return true;
}

// This takes about 20us to run
bool AP_InertialSensor_ITG3200BMA180::update(void)
{
    update_accel(_accel_instance);
    update_gyro(_gyro_instance);

    return true;
}

void AP_InertialSensor_ITG3200BMA180::accumulate(void)
{
    // get pointer to i2c bus semaphore
    AP_HAL::Semaphore* i2c_sem = hal.i2c->get_semaphore();

    // take i2c bus sempahore
    if (!i2c_sem->take(HAL_SEMAPHORE_BLOCK_FOREVER))
        return;

    // Read ACC
    uint8_t buffer[6];
    uint32_t now = AP_HAL::micros();
    // This takes about 250us at 400kHz I2C speed
    if ((now - _last_accel_timestamp) >= raw_sample_interval_us
        && hal.i2c->readRegisters(BMA180_ADDRESS, BMA180_DATA, 6, buffer) == 0)
    {
        int16_t y =  (((((int16_t)buffer[1]) << 8) | (buffer[0]) >> 2));    // chip X axis
        int16_t x = -(((((int16_t)buffer[3]) << 8) | (buffer[2]) >> 2));    // chip Y axis
        int16_t z =  (((((int16_t)buffer[5]) << 8) | (buffer[4]) >> 2));    // chip Z axis
        Vector3f accel = Vector3f(x,y,z);
        // Adjust for chip scaling to get m/s/s
        accel *= BMA180_ACC_SCALE_M_S;
        _rotate_and_correct_accel(_accel_instance, accel);
        _notify_new_accel_raw_sample(_accel_instance, accel);
        _last_accel_timestamp = now;
    }

    // Read gyro
    now = AP_HAL::micros();
    // This takes about 250us at 400kHz I2C speed
    if ((now - _last_gyro_timestamp) >= raw_sample_interval_us
        && hal.i2c->readRegisters(ITG3200_GYRO_ADDRESS, ITG3200_GYRO_GYROX_H, 6, buffer) == 0)
    {
        // See above re order of samples in buffer
        int16_t y =  ((((int16_t)buffer[0]) << 8) | buffer[1]);    // chip X axis
        int16_t x = -((((int16_t)buffer[2]) << 8) | buffer[3]);    // chip Y axis
        int16_t z =  ((((int16_t)buffer[4]) << 8) | buffer[5]);    // chip Z axis
        Vector3f gyro = Vector3f(x,y,z);
        // Adjust for chip scaling to get radians/sec
        gyro *= ITG3200_GYRO_SCALE_R_S;
        _rotate_and_correct_gyro(_gyro_instance, gyro);
        _notify_new_gyro_raw_sample(_gyro_instance, gyro);
    }

    // give back i2c semaphore
    i2c_sem->give();
}

#endif // CONFIG_HAL_BOARD
