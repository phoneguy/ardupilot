/// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-
#include <AP_HAL/AP_HAL.h>

#if CONFIG_HAL_BOARD == HAL_BOARD_LINUX
#include "AnalogIn_IIO.h"

extern const AP_HAL::HAL& hal;

const char* AnalogSource_IIO::analog_sources[] = {
    "in_voltage0_raw",
    "in_voltage1_raw",
    "in_voltage2_raw",
    "in_voltage3_raw",
    "in_voltage4_raw",
    "in_voltage5_raw",
    "in_voltage6_raw",
    "in_voltage7_raw",
};

AnalogSource_IIO::AnalogSource_IIO(int16_t pin, float initial_value, float voltage_scaling) :
    _pin(pin),
    _value(initial_value),
    _voltage_scaling(voltage_scaling),
    _sum_value(0),
    _sum_count(0),
    _pin_fd(-1)
{
    init_pins();
    select_pin();
}

void AnalogSource_IIO::init_pins(void)
{
    char buf[100];
    for (unsigned int i = 0; i < ARRAY_SIZE(AnalogSource_IIO::analog_sources); i++) {
        // Construct the path by appending strings
        strncpy(buf, IIO_ANALOG_IN_DIR, sizeof(buf));
        strncat(buf, AnalogSource_IIO::analog_sources[i], sizeof(buf) - strlen(buf) - 1);

        fd_analog_sources[i] = open(buf, O_RDONLY | O_NONBLOCK);
    }
}

/*
  selects a diferent file descriptor among in the fd_analog_sources array
 */
void AnalogSource_IIO::select_pin(void)
{
    _pin_fd = fd_analog_sources[_pin];
}

float AnalogSource_IIO::read_average()
{
    read_latest();
    if (_sum_count == 0) {
        return _value;
    }
    hal.scheduler->suspend_timer_procs();
    _value = _sum_value / _sum_count;
    _sum_value = 0;
    _sum_count = 0;
    hal.scheduler->resume_timer_procs();
    return _value;
}

float AnalogSource_IIO::read_latest()
{
    char sbuf[10];

    if (_pin_fd == -1) {
        _latest = 0;
        return 0;
    }

    memset(sbuf, 0, sizeof(sbuf));
    pread(_pin_fd, sbuf, sizeof(sbuf) - 1, 0);
    _latest = atoi(sbuf) * _voltage_scaling;
    _sum_value += _latest;
    _sum_count++;

    return _latest;
}

// output is in volts
float AnalogSource_IIO::voltage_average()
{
    return read_average();
}

float AnalogSource_IIO::voltage_latest()
{
    read_latest();
    return _latest;
}

void AnalogSource_IIO::set_pin(uint8_t pin)
{
    if (_pin == pin) {
        return;
    }

    hal.scheduler->suspend_timer_procs();
    _pin = pin;
    _sum_value = 0;
    _sum_count = 0;
    _latest = 0;
    _value = 0;
    select_pin();
    hal.scheduler->resume_timer_procs();
}

void AnalogSource_IIO::set_stop_pin(uint8_t p)
{}

void AnalogSource_IIO::set_settle_time(uint16_t settle_time_ms)
{}

AnalogIn_IIO::AnalogIn_IIO()
{}

float AnalogIn_IIO::board_voltage(void)
{
    // set to AIN5 pin
    _vcc_pin_analog_source->set_pin(IIO_VCC_PIN);
//    return _vcc_pin_analog_source->voltage_average() * 5.67;
    return _vcc_pin_analog_source->read_latest() * 5.67; // calibrate with V in / V divider
}

float AnalogIn_IIO::servorail_voltage(void)
{
    // set to AIN3 pin
    _svr_pin_analog_source->set_pin(IIO_SVR_PIN);
//    return _svr_pin_analog_source->voltage_average()* 4.17;
    return _svr_pin_analog_source->read_latest() * 4.17;
}

void AnalogIn_IIO::init()
{
    // use AIN3 for board voltage and AIN5 for servo rail voltage
    _vcc_pin_analog_source = channel(IIO_VCC_PIN);
    _svr_pin_analog_source = channel(IIO_SVR_PIN);
}

AP_HAL::AnalogSource* AnalogIn_IIO::channel(int16_t pin) {
    return new AnalogSource_IIO(pin, 0.0f, IIO_VOLTAGE_SCALING);
}

#endif // CONFIG_HAL_BOARD
