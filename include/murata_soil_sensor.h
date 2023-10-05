#ifndef MURATA_SOIL_SENSOR_H_
#define MURATA_SOIL_SENSOR_H_
#include <Arduino.h>

enum FunctionCode
{
    kFunctionCodeReadNWords = 0x03,
    kFunctionCodeWriteNWords = 0x10,
};

enum ErrorCode
{
    kErrorCodeIllegalFunctionCode = 0x01,
    kErrorCodeIllegalStartAddress = 0x02,
    kErrorCodeIllegalProtocolOrFormat = 0x03,
    kErrorCodeCRC16Error = 0x05,
    kErrorCodeSensorIsUnderMeasurement = 0x06,
    kErrorCodeFailedToWriteToRegister = 0x10,
    kErrorCodeI2CCommunicationError = 0x20,
};

int16_t Unsigned12BitToSigned16Bit(word u12);

#endif // MURATA_SOIL_SENSOR_H_
