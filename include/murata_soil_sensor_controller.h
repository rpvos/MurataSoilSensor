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
        const uint8_t kResponseErrorSize = 5;
        const uint8_t kSetRegisterSize = 11;
        const uint8_t kReadRegisterSize = 8;
        const uint8_t kResponseSetRegisterSize = 8;
        const uint8_t kResponseReadRegisterSize = 7;
    } // namespace MessageSize

    class MurataSoilSensorController
    {
    public:
        MurataSoilSensorController(RS485 *serial, uint8_t enable_pin, byte slave_number = 1);

        void Enable(void);
        void Disable(void);
        bool WriteRegister(word address, word number_of_registers, word register_value);
        bool ReadRegister(word address, word number_of_registers, word &register_value);
        bool SetSlaveNumber(byte slave_number);
        bool StartMeasurement(void);

    private:
        RS485 *serial_;
        uint8_t enable_pin_;
        byte slave_number_;

        void PrintError(byte error_code);
    };

} // namespace MurataSoilSensorController

#endif // MURATA_SOIL_SENSOR_CONTROLLER_H_