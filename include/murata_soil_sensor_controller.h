#ifndef MURATA_SOIL_SENSOR_CONTROLLER_H_
#define MURATA_SOIL_SENSOR_CONTROLLER_H_
#include "murata_soil_sensor.h"
#include <MAX485TTL.h>

namespace MurataSoilSensorController
{

    // Define used to shift by byte size
    const uint8_t kSizeOfByte = 8;

    namespace MessageSize
    {
        const uint8_t kHeaderSize = 2;
        const uint8_t kCrcSize = 2;
        const uint8_t kAddressSize = 2;
        const uint8_t kNumberOfRegistersSize = 2;
        const uint8_t kNumberOfDataLengthSize = 1;
    } // namespace MessageSize

    class MurataSoilSensorController
    {
    public:
        MurataSoilSensorController(RS485 *serial, uint8_t enable_pin, byte slave_number = 1);

        void Enable(void);
        void Disable(void);
        bool WriteRegister(const word address, const word number_of_registers, const word *register_values);
        bool ReadRegister(const word address, const word number_of_registers, word *register_values);
        bool SetSlaveNumber(byte slave_number);
        bool StartMeasurement(void);
        bool IsMeasurementFinished(bool &isFinished);
        bool ReadMeasurementData(MurataSoilSensor::MeasurementData &measurement_data);

    private:
        RS485 *serial_;
        uint8_t enable_pin_;
        byte slave_number_;

        void HandleError(const byte *response, const int response_length);
        void PrintError(byte error_code);
    };

} // namespace MurataSoilSensorController

#endif // MURATA_SOIL_SENSOR_CONTROLLER_H_