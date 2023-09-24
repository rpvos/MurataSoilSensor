#ifndef MURATA_SOIL_SENSOR_H_
#define MURATA_SOIL_SENSOR_H_
#include <Arduino.h>

#pragma region helper_structs

enum FunctionCode
{
    kReadNWords = 0x03,
    kWriteNWords = 0x10,
};

struct Header
{
    Header();
    Header(byte slaveNumber, byte functionCode);
    byte slaveNumber;
    byte functionCode;
};

struct Crc
{
    word crc;
    byte lower;
    byte upper;
};

class Address
{
public:
    Address(word value);
    byte lower;
    byte upper;
};

struct RegisterNumber
{
    byte lower;
    byte upper;
};

struct RegisterValue
{
    byte lower;
    byte upper;
};

struct TemperatureValue
{
    // 12 bits value with 2-compliment system
    short value : 12;
};

#pragma endregion

class RequestStartMeasurement
{
public:
    RequestStartMeasurement(Header header, word address, word registerNumber, word dataLength, word registerValue);
    Header header;
    word address;
    word registerNumber;
    byte dataLength;
    word registerValue;
};

struct ResponseStartMeasurement
{
    Header header;
    word address;
    word registerNumber;
    Crc crc;
};

struct RequestDataMeasurement
{
    Header header;
    Address address;
    RegisterNumber registerNumber;
};

struct ResponseDataMeasurement
{
    Header header;
    byte dataLength;
    RegisterValue registerValue;
    Crc crc;
};

enum ErrorCode
{
    kIllegalFunctionCode = 0x01,
    kIllegalStartAddress = 0x02,
    kIllegalProtocolOrFormat = 0x03,
    kCRC16Error = 0x05,
    kSensorIsUnderMeasurement = 0x06,
    kFailedToWriteToRegister = 0x10,
    kI2CCommunicationError = 0x20,
};

struct ErrorMessage
{
    Header header;
    byte ErrorCode;
    Crc crc;
};
#endif // MURATA_SOIL_SENSOR_H_
