#ifndef MURATA_SOIL_SENSOR_CONTROLLER_H_
#define MURATA_SOIL_SENSOR_CONTROLLER_H_
#include "murata_soil_sensor.h"
#include "murata_soil_sensor_errors.h"
#include <MAX485TTL.hpp>
#include "modbus.h"

namespace MurataSoilSensorController
{
    class MurataSoilSensorController
    {
    public:
        MurataSoilSensorController(RS485 *serial, uint8_t enable_pin, byte slave_number = 1);
        ~MurataSoilSensorController();

        void Enable(void);
        void Disable(void);
        [[nodiscard]] MurataSoilSensorError WriteRegister(const word address, const word number_of_registers, const word *register_values, bool broadcast = false);
        [[nodiscard]] MurataSoilSensorError ReadRegister(const word address, const word number_of_registers, word *register_values);
        [[nodiscard]] MurataSoilSensorError SetSlaveNumber(byte slave_number, bool broadcast = false);
        [[nodiscard]] MurataSoilSensorError StartMeasurement(void);
        [[nodiscard]] MurataSoilSensorError IsMeasurementFinished(bool &isFinished);
        [[nodiscard]] MurataSoilSensorError ReadMeasurementData(MurataSoilSensor::MeasurementData &measurement_data);

    private:
        // Define used to shift by byte size
        const uint8_t kSizeOfByte = 8;
        // Message sizes used to calculate size of buffer for message
        const uint8_t kHeaderSize = 2;
        const uint8_t kCrcSize = 2;
        const uint8_t kAddressSize = 2;
        const uint8_t kNumberOfRegistersSize = 2;
        const uint8_t kNumberOfDataLengthSize = 1;

        RS485 *serial_;
        ModbusSlave *modbus_;
        uint8_t enable_pin_;

        void HandleError(const byte *response, const int response_length);
        void PrintError(byte error_code);
    };

} // namespace MurataSoilSensorController

#endif // MURATA_SOIL_SENSOR_CONTROLLER_H_