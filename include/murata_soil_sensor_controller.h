#ifndef MURATA_SOIL_SENSOR_CONTROLLER_H_
#define MURATA_SOIL_SENSOR_CONTROLLER_H_
#include "murata_soil_sensor.h"
#include <MAX485TTL.h>

class MurataSoilSensorController
{
public:
    MurataSoilSensorController(RS485 *serial, uint8_t enable_pin, byte slave_number);

    void Enable(void);
    void Disable(void);
    bool StartMeasurement(word address, word number_of_registers, word register_value);

private:
    RS485 *serial;
    uint8_t enable_pin;
    byte slave_number;

    void PrintError(byte error_code);
};

#endif // MURATA_SOIL_SENSOR_CONTROLLER_H_