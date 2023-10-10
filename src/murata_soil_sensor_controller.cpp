#include "murata_soil_sensor_controller.h"
#include "murata_soil_sensor.h"
#include <crc.h>

namespace MurataSoilSensorController
{

    MurataSoilSensorController::MurataSoilSensorController(RS485 *serial, uint8_t enable_pin, byte slave_number)
    {
        this->serial_ = serial;
        this->enable_pin_ = enable_pin;
        this->slave_number_ = slave_number;

        pinMode(enable_pin, OUTPUT);
        Enable();

        SetSlaveNumber(slave_number);
    }

    void MurataSoilSensorController::Enable()
    {
        digitalWrite(enable_pin_, HIGH);
    }
    void MurataSoilSensorController::Disable()
    {
        digitalWrite(enable_pin_, LOW);
    }

    bool MurataSoilSensorController::SetSlaveNumber(byte slave_number)
    {
        if (slave_number > 31)
        {
            Serial.println("Slave number must be between 0 and 31");
            return false;
        }

        if (WriteRegister(MurataSoilSensor::RegisterNumber::kRegisterSensorNumber, 0x0001, slave_number))
        {
            this->slave_number_ = slave_number;
            return true;
        }

        return false;
    }

    bool MurataSoilSensorController::StartMeasurement(void)
    {
        return WriteRegister(MurataSoilSensor::RegisterNumber::kRegisterSensorControl, 0x0001, 0x0001);
    }

    bool MurataSoilSensorController::WriteRegister(word address, word number_of_registers, word register_value)
    {
        // Compute every byte of message
        byte address_upper = address >> kSizeOfByte & 0xFF;
        byte address_lower = address & 0xFF;
        byte number_of_registers_upper = number_of_registers >> kSizeOfByte & 0xFF;
        byte number_of_registers_lower = number_of_registers & 0xFF;
        byte data_length = 2 * number_of_registers;
        byte upper_register_value = register_value >> kSizeOfByte & 0xFF;
        byte lower_register_value = register_value & 0xFF;

        // Fill message with all values but the CRC
        byte message[MessageSize::kSetRegisterSize] = {
            slave_number_,
            MurataSoilSensor::FunctionCode::kFunctionCodeWriteNWords,
            address_upper,
            address_lower,
            number_of_registers_upper,
            number_of_registers_lower,
            data_length,
            upper_register_value,
            lower_register_value};

        Crc::AddCrc(message, message, MessageSize::kSetRegisterSize - 2);

        serial_->write(message, MessageSize::kSetRegisterSize);

        // Receive response
        serial_->WaitForInput();
        int response_length = serial_->ReadIntoBuffer();
        const byte *response = (byte *)serial_->GetBuffer();

        // TODO check for function code instead of length
        //  Check if size of reponse is equal to size of error response
        if (response_length == MessageSize::kResponseErrorSize)
        {
            if (Crc::ValidateCrc(response, MessageSize::kResponseErrorSize))
            {
                byte error_slave_number = response[0];
                // Check if slave number is correct
                if (slave_number_ != error_slave_number)
                {
                    // When the sensor responds with a diffrent slave number use that for following messages
                    Serial.print("Slave number is incorrect should be: ");
                    Serial.println(error_slave_number, HEX);
                    slave_number_ = error_slave_number;
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
        else if (response_length == MessageSize::kResponseSetRegisterSize)
        {
            if (Crc::ValidateCrc(response, MessageSize::kResponseSetRegisterSize))
            {
                byte response_slave_number = response[0];
                // Check if slave numbers are the same except when slave number has been changed by setting address 0x0026
                if (slave_number_ != response_slave_number && address != 0x0026)
                {
                    Serial.println("Slave number is different");
                    return false;
                }

                byte response_function_code = response[1];
                if (MurataSoilSensor::FunctionCode::kFunctionCodeWriteNWords != response_function_code)
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

    bool MurataSoilSensorController::ReadRegister(word address, word number_of_registers, word &register_value)
    {
        // Compute every byte of message
        byte address_upper = address >> kSizeOfByte & 0xFF;
        byte address_lower = address & 0xFF;
        byte number_of_registers_upper = number_of_registers >> kSizeOfByte & 0xFF;
        byte number_of_registers_lower = number_of_registers & 0xFF;

        // Fill message with all values but the CRC
        byte message[MessageSize::kReadRegisterSize] = {
            slave_number_,
            MurataSoilSensor::FunctionCode::kFunctionCodeReadNWords,
            address_upper,
            address_lower,
            number_of_registers_upper,
            number_of_registers_lower,
        };

        Crc::AddCrc(message, message, MessageSize::kSetRegisterSize - 2);

        serial_->write(message, MessageSize::kSetRegisterSize);

        // Receive response
        serial_->WaitForInput();
        int response_length = serial_->ReadIntoBuffer();
        const byte *response = (byte *)serial_->GetBuffer();

        // Check if size of reponse is equal to size of error response
        if (response_length == MessageSize::kResponseErrorSize)
        {
            if (Crc::ValidateCrc(response, MessageSize::kResponseErrorSize))
            {
                byte error_slave_number = response[0];
                // Check if slave number is correct
                if (slave_number_ != error_slave_number)
                {
                    // When the sensor responds with a diffrent slave number use that for following messages
                    Serial.print("Slave number is incorrect should be: ");
                    Serial.println(error_slave_number, HEX);
                    slave_number_ = error_slave_number;
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
        else if (response_length == MessageSize::kResponseReadRegisterSize)
        {
            if (Crc::ValidateCrc(response, MessageSize::kResponseReadRegisterSize))
            {
                byte response_slave_number = response[0];
                // Check if slave numbers are the same except when slave number has been changed by setting address 0x0026
                if (slave_number_ != response_slave_number && address != 0x0026)
                {
                    Serial.println("Slave number is different");
                    return false;
                }

                byte response_function_code = response[1];
                if (MurataSoilSensor::FunctionCode::kFunctionCodeReadNWords != response_function_code)
                {
                    Serial.println("Function code is different");
                    return false;
                }

                byte response_data_length = response[2];
                if (number_of_registers * 2 != response_data_length)
                {
                    Serial.println("Data length is different");
                    return false;
                }

                byte response_register_value_upper = response[3];
                byte response_register_value_lower = response[4];

                // Compute word from two byte register value
                register_value = response_register_value_upper << kSizeOfByte | response_register_value_lower;
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

    void MurataSoilSensorController::PrintError(byte error_code)
    {
        switch (error_code)
        {
        case MurataSoilSensor::ErrorCode::kErrorCodeIllegalFunctionCode:
            Serial.println("Error illegal function code");
            break;
        case MurataSoilSensor::ErrorCode::kErrorCodeIllegalStartAddress:
            Serial.println("Error illegal start address");
            break;
        case MurataSoilSensor::ErrorCode::kErrorCodeIllegalProtocolOrFormat:
            Serial.println("Error illegal protocol or format");
            break;
        case MurataSoilSensor::ErrorCode::kErrorCodeCRC16Error:
            Serial.println("Error CRC16 not correct");
            break;
        case MurataSoilSensor::ErrorCode::kErrorCodeSensorIsUnderMeasurement:
            Serial.println("Error sensor is under measurement");
            break;
        case MurataSoilSensor::ErrorCode::kErrorCodeFailedToWriteToRegister:
            Serial.println("Error failed to write to register");
            break;
        case MurataSoilSensor::ErrorCode::kErrorCodeI2CCommunicationError:
            Serial.println("Error I2C communication error");
            break;
        default:
            Serial.println("Unknown error code");
            break;
        }
    }

} // namespace MurataSoilSensorController