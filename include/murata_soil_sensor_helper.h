#ifndef MURATA_SOIL_SENSOR_HELPER_H_
#define MURATA_SOIL_SENSOR_HELPER_H_
#include <Arduino.h>

namespace MurataSoilSensorHelper
{
    /**
     * @brief Register locations of components of murata soil sensor
     *
     */
    namespace RegisterNumber
    {
        static const word kRegisterSensorControl = 0x000A; // Start measurement
        static const word kRegisterSensorState = 0x000C;   // State of measurement
        static const word kRegisterTemperature = 0x0016;   // Soil temperature
        static const word kRegisterEcBulk = 0x0018;        // Ions in the water
        static const word kRegisterVWC = 0x001C;           // Volumetric Water Content
        static const word kRegisterEcPore = 0x0022;        // Ions in the soil
        static const word kRegisterSensorNumber = 0x0026;  // Number of sensor to set if more then 1 sensor is being used
    }

    /**
     * @brief Function codes used by murata soil sensor
     *
     */
    namespace FunctionCode
    {
        static const byte kFunctionCodeReadNWords = 0x03;
        static const byte kFunctionCodeWriteNWords = 0x10;
        static const byte kFunctionCodeError = 0x80;
    }

    /**
     * @brief Error codes returned by murata soil sensor
     *
     */
    namespace ErrorCode
    {
        static const byte kErrorCodeIllegalFunctionCode = 0x01;
        static const byte kErrorCodeIllegalStartAddress = 0x02;
        static const byte kErrorCodeIllegalProtocolOrFormat = 0x03;
        static const byte kErrorCodeCRC16Error = 0x05;
        static const byte kErrorCodeSensorIsUnderMeasurement = 0x06;
        static const byte kErrorCodeFailedToWriteToRegister = 0x10;
        static const byte kErrorCodeI2CCommunicationError = 0x20;
    }

    /**
     * @brief Struct containing all values the murata soil sensor can measure
     *
     */
    struct MeasurementData
    {
        word temperature;
        word ec_bulk;
        word vwc;
        word ec_pore;
    };

    /**
     * @brief Helper function to convert word containing a signed 12 bit value to a signed 16 bit value
     *
     * @param u12 signed 12 bit value
     * @return int16_t signed 16 bit value
     */
    [[nodiscard]] word Signed12BitToSigned16Bit(word u12);
    [[nodiscard]] double CalculateTemperature(word value);
    [[nodiscard]] double CalculateECBulk(word value);
    [[nodiscard]] double CalculateVWC(word value);
    [[nodiscard]] double CalculateECPore(word value);

} // namespace MurataSoilSensorHelper

#endif // MURATA_SOIL_SENSOR_HELPER_H_