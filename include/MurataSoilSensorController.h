#ifndef MURATA_SOIL_SENSOR_CONTROLLER_H_
#define MURATA_SOIL_SENSOR_CONTROLLER_H_
#include <MurataSoilSensor.h>
#include <MAX485TTL.h>

class MurataSoilSensorController
{
public:
    MurataSoilSensorController(RS485 *serial, int enable_pin);

    void Enable(void);
    void Disable(void);
    bool StartMeasurement(RequestStartMeasurement request);

private:
    RS485 *serial;
    int enable_pin;
    void SendStructAsHex(byte *request, int size);
    byte *ConvertToByteArray(String input);
};

#endif // MURATA_SOIL_SENSOR_CONTROLLER_H_