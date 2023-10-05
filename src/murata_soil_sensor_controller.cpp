#include "murata_soil_sensor_controller.h"
#include "murata_soil_sensor.h"
#include <crc.h>

#define kResponseErrorSize 5
#define kStartMeasurementSize 11
#define kResponseStartMeasurementSize 8

MurataSoilSensorController::MurataSoilSensorController(RS485 *serial, uint8_t enable_pin, byte slave_number)
{
    this->serial = serial;
    this->enable_pin = enable_pin;
    this->slave_number = slave_number;

    pinMode(enable_pin, OUTPUT);
    Enable();

    // TODO Send slave number to initiate
    // SetSlaveNumber(slave_number);
}

void MurataSoilSensorController::Enable()
{
    digitalWrite(enable_pin, HIGH);
}
void MurataSoilSensorController::Disable()
{
    digitalWrite(enable_pin, LOW);
}

bool MurataSoilSensorController::StartMeasurement(word address, word number_of_registers, word register_value)
{
    // Compute every byte of message
    byte address_upper = address >> 8 & 0xFF;
    byte address_lower = address & 0xFF;
    byte number_of_registers_upper = number_of_registers >> 8 & 0xFF;
    byte number_of_registers_lower = number_of_registers & 0xFF;
    byte data_length = 2 * number_of_registers;
    byte upper_register_value = register_value >> 8 & 0xFF;
    byte lower_register_value = register_value & 0xFF;

    // Fill message with all values but the CRC
    byte message[kStartMeasurementSize] = {
        slave_number,
        kFunctionCodeWriteNWords,
        address_upper,
        address_lower,
        number_of_registers_upper,
        number_of_registers_lower,
        data_length,
        upper_register_value,
        lower_register_value};

    AddCrc(message, message, kStartMeasurementSize - 2);

    serial->write(message, kStartMeasurementSize);

    // Receive response
    serial->WaitForInput();
    int response_length = serial->ReadIntoBuffer();
    byte *response = (byte *)serial->GetBuffer();

    // Check if size of reponse is equal to size of error response
    if (response_length == kResponseErrorSize)
    {
        if (ValidateCrc(response, kResponseErrorSize))
        {
            byte error_slave_number = response[0];
            // Check if slave number is correct
            if (slave_number != error_slave_number)
            {
                Serial.print("Slave number is incorrect should be: ");
                Serial.println(error_slave_number, HEX);
            }

            byte error_function_code = response[1];
            // Check if function code is correct
            if (error_function_code != 0x80)
            {
                Serial.print("Error function code is:");
                Serial.println(error_function_code, HEX);
            }

            // Print error corresponding to error code
            byte error_code = response[2];
            PrintError(error_code);
        }
        else
        {
            Serial.println("Crc of error not correct");
        }
        return false;
    }
    else if (response_length == kResponseStartMeasurementSize)
    {
        if (ValidateCrc(response, kResponseStartMeasurementSize))
        {
            byte response_slave_number = response[0];
            if (slave_number != response_slave_number)
            {
                Serial.println("Slave number is different");
                return false;
            }

            byte response_function_code = response[1];
            if (kFunctionCodeWriteNWords != response_function_code)
            {
                Serial.println("Function code is different");
                return false;
            }

            byte response_start_address_upper = response[2];
            if (address_upper != response_start_address_upper)
            {
                Serial.println("Start address upper is different");
                return false;
            }

            byte response_start_address_lower = response[3];
            if (address_lower != response_start_address_lower)
            {
                Serial.println("Start address lower is different");
                return false;
            }

            byte response_number_of_registers_upper = response[4];
            if (number_of_registers_upper != response_number_of_registers_upper)
            {
                Serial.println("Number of registers upper is different");
                return false;
            }

            byte response_number_of_registers_lower = response[5];
            if (number_of_registers_lower != response_number_of_registers_lower)
            {
                Serial.println("Number of registers lower is different");
                return false;
            }

            return true;
        }
    }
    else if (response_length < 1)
    {
        Serial.println("No message was returned");
    }
    else
    {
        Serial.print("Message length was unknown: ");
        Serial.println(response_length);
    }

    return false;
}

// // Cast to ResponseStartMeasurement
// ResponseStartMeasurement *castedResponse = const_cast<ResponseStartMeasurement *>(reinterpret_cast<const ResponseStartMeasurement *>(serial->GetBuffer()));

// // Validate Response
// if (castedResponse->header.slave_number == request.header.slave_number && castedResponse->address == request.address && castedResponse->numberOfRegisters == request.numberOfRegisters)
// {
//     return true;
// }

// return false;
//}

// void MurataSoilSensorController::SendStructAsHex(byte *request, int size)
// {
//     for (int i = 0; i < size; i++)
//     {
//         // Make 15 and below be 0x0F instead of 0xF
//         if (request[i] < 16)
//         {
//             serial->print(0);
//         }

//         serial->print(request[i], HEX);
//     }
//     serial->flush();
// }

// byte *MurataSoilSensorController::ConvertToByteArray(String hexString)
// {
//     // Convert input which is a hex decimal string to char (unsigned short) array
//     byte charArrayResponse[sizeof(ResponseStartMeasurement)];
//     for (size_t i = 0; i < hexString.length(); i += 2)
//     {
//         byte c;
//         sscanf(hexString.substring(i, i + 1).c_str(), "%x", c);
//         charArrayResponse[i / 2] = c;
//     }

//     return charArrayResponse;
// }

// bool MurataSoilSensorController::StartMeasurement(RequestStartMeasurement request)
// {
//     byte *requestCharPointer = reinterpret_cast<byte *>(&request);

//     SendStructAsHex(requestCharPointer, sizeof(RequestStartMeasurement));

//     serial->WaitForInput();
//     ResponseStartMeasurement *response = nullptr;
//     if (serial->available())
//     {
//         String input = serial->readString();
//         // Convert haxadecimal string to char array
//         byte *charArrayResponse = ConvertToByteArray(input);

//         // Check the CRC
//         // TODO check if /2 is correct
//         if (ValidateCrc(charArrayResponse, input.length() / 2) == false)
//         {
//             return false;
//         }

//         // Cast to responseStartMeasurement
//         response = reinterpret_cast<ResponseStartMeasurement *>(charArrayResponse);
//     }

//     // Check the address
//     if (request.address != response->address)
//     {
//         return false;
//     }

//     // Check the register
//     if (request.registerNumber != response->registerNumber)
//     {
//         return false;
//     }

//     return true;
// }

void MurataSoilSensorController::PrintError(byte error_code)
{
    switch (error_code)
    {
    case kErrorCodeIllegalFunctionCode:
        Serial.println("Error illegal function code");
        break;
    case kErrorCodeIllegalStartAddress:
        Serial.println("Error illegal start address");
        break;
    case kErrorCodeIllegalProtocolOrFormat:
        Serial.println("Error illegal protocol or format");
        break;
    case kErrorCodeCRC16Error:
        Serial.println("Error CRC16 not correct");
        break;
    case kErrorCodeSensorIsUnderMeasurement:
        Serial.println("Error sensor is under measurement");
        break;
    case kErrorCodeFailedToWriteToRegister:
        Serial.println("Error failed to write to register");
        break;
    case kErrorCodeI2CCommunicationError:
        Serial.println("Error I2C communication error");
        break;
    default:
        Serial.println("Unknown error code");
        break;
    }
}