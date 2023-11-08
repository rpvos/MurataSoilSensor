#include "murata_soil_sensor_controller.h"
#include "murata_soil_sensor.h"
#include "crc16.h"

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
        if (slave_number <= 31)
        {
            Serial.println("Slave number must be between 0 and 31");
            return false;
        }

        word slave_number_word = slave_number;
        if (WriteRegister(MurataSoilSensor::RegisterNumber::kRegisterSensorNumber, 0x0001, &slave_number_word))
        {
            this->slave_number_ = slave_number;
            return true;
        }

        return false;
    }

    bool MurataSoilSensorController::StartMeasurement(void)
    {
        word start_measurement = 0x0001;
        return WriteRegister(MurataSoilSensor::RegisterNumber::kRegisterSensorControl, 0x0001, &start_measurement);
    }

    bool MurataSoilSensorController::IsMeasurementFinished(bool &isFinished)
    {
        word value;
        bool has_succeeded = ReadRegister(MurataSoilSensor::RegisterNumber::kRegisterSensorState, 0x0001, &value);
        isFinished = value;
        return has_succeeded;
    }

    bool MurataSoilSensorController::ReadMeasurementData(MurataSoilSensor::MeasurementData &measurement_data)
    {
        // TODO ReadMeasurementData
        //  word value;
        //  bool has_succeeded = ReadRegister(MurataSoilSensor::RegisterNumber::kRegisterTemperature, 0x0001, value);
        //  isFinished = value;
        //  return has_succeeded;
    }

    bool MurataSoilSensorController::WriteRegister(const word address, const word number_of_registers, const word *register_values)
    {
        const byte function_code = MurataSoilSensor::FunctionCode::kFunctionCodeWriteNWords;

        // Compute every byte of message
        byte address_upper = address >> kSizeOfByte & 0xFF;
        byte address_lower = address & 0xFF;
        byte number_of_registers_upper = number_of_registers >> kSizeOfByte & 0xFF;
        byte number_of_registers_lower = number_of_registers & 0xFF;
        const byte data_length = 2 * number_of_registers;

        const uint8_t const_elements_of_message_size = MessageSize::kHeaderSize + MessageSize::kAddressSize + MessageSize::kNumberOfRegistersSize + MessageSize::kNumberOfDataLengthSize;
        const byte message_size = const_elements_of_message_size + data_length + MessageSize::kCrcSize;

        // Fill const elements of message with values
        byte *message = new byte[message_size];
        message[0] = slave_number_;
        message[1] = function_code;
        message[2] = address_upper;
        message[3] = address_lower;
        message[4] = number_of_registers_upper;
        message[5] = number_of_registers_lower;
        message[6] = data_length;

        for (size_t register_value = 0; register_value < number_of_registers; register_value++)
        {
            byte upper_register_value = register_values[register_value] >> kSizeOfByte & 0xFF;
            byte lower_register_value = register_values[register_value] & 0xFF;

            message[const_elements_of_message_size + (register_value * 2)] = upper_register_value;
            message[const_elements_of_message_size + (register_value * 2) + 1] = lower_register_value;
        }

        Crc::AddCrcModbus(message, message, message_size - 2, false);

        serial_->write(message, message_size);
        serial_->flush();

        delete[] message;

        // Receive response
        serial_->WaitForInput();
        int response_length = serial_->ReadIntoBuffer();
        Serial.print("Response length: ");
        Serial.println(response_length);

        const byte *response = reinterpret_cast<const byte *>(serial_->GetBuffer());

        //  Check if functioncode of reponse is equal to the set function code
        if (response[1] == function_code)
        {
            if (!Crc::ValidateCrcModbus(response, response_length, false))
            {
                Serial.println("Crc of response is not correct");
                return false;
            }

            byte response_slave_number = response[0];
            // Check if slave numbers are the same except when slave number has been changed by setting address 0x0026
            if (slave_number_ != response_slave_number || address == 0x0026)
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
        else if (response[1] == MurataSoilSensor::FunctionCode::kFunctionCodeError)
        {
            if (!Crc::ValidateCrcModbus(response, response_length, false))
            {
                Serial.println("Crc of error is not correct");
            }
            else
            {
                // Print error corresponding to error code
                byte error_code = response[2];
                PrintError(error_code);
            }
        }
        else if (response_length < 1)
        {
            Serial.println("No message was returned");
        }
        else
        {
            Serial.print("Function code was unknown: ");
            Serial.println(function_code);
            Serial.print("Message length was: ");
            Serial.println(response_length);
            Serial.print("Message was: ");
            Serial.write(response, response_length);
        }

        return false;
    }

    bool MurataSoilSensorController::ReadRegister(const word address, const word number_of_registers, word *register_values)
    {
        const byte function_code = MurataSoilSensor::FunctionCode::kFunctionCodeReadNWords;
        const uint8_t message_size = MessageSize::kHeaderSize + MessageSize::kAddressSize + MessageSize::kNumberOfRegistersSize + MessageSize::kCrcSize;

        // Compute every byte of message
        byte address_upper = address >> kSizeOfByte & 0xFF;
        byte address_lower = address & 0xFF;
        byte number_of_registers_upper = number_of_registers >> kSizeOfByte & 0xFF;
        byte number_of_registers_lower = number_of_registers & 0xFF;

        // Fill message with all values but the CRC
        byte message[message_size] = {
            slave_number_,
            function_code,
            address_upper,
            address_lower,
            number_of_registers_upper,
            number_of_registers_lower,
        };

        Crc::AddCrcModbus(message, message, message_size - 2, false);

        serial_->write(message, message_size);
        serial_->flush();

        // Receive response
        serial_->WaitForInput();
        int response_length = serial_->ReadIntoBuffer();
        if (response_length <= 0)
        {
            return false;
        }

        const byte *response = reinterpret_cast<const byte *>(serial_->GetBuffer());

        // Check if size of reponse and function code are correct
        if (response[1] == function_code)
        {
            if (!Crc::ValidateCrcModbus(response, response_length, false))
            {
                Serial.println("Crc of error not correct");
                return false;
            }

            byte response_slave_number = response[0];
            // Check if slave numbers are the same except when slave number has been changed by setting address 0x0026
            if (slave_number_ != response_slave_number && address != 0x0026)
            {
                Serial.println("Slave number is different");
                return false;
            }

            byte response_data_length = response[2];
            if (number_of_registers * 2 != response_data_length)
            {
                Serial.println("Data length is different");
                return false;
            }

            for (size_t register_index = 0; register_index < number_of_registers; register_index++)
            {
                byte response_register_value_upper = response[3];
                byte response_register_value_lower = response[4];
                // Compute word from two byte register value
                word register_value = response_register_value_upper << kSizeOfByte | response_register_value_lower;
                register_values[register_index] = register_value;
            }

            return true;
        }

        HandleError(response, response_length);
        return false;
    }

    void MurataSoilSensorController::HandleError(const byte *response, const int response_length)
    {
        if (response[1] == MurataSoilSensor::FunctionCode::kFunctionCodeError)
        {
            if (Crc::ValidateCrcModbus(response, response_length, false))
            {
                Serial.println("Crc of error not correct");
                return;
            }

            byte error_slave_number = response[0];
            // Check if slave number is correct
            if (slave_number_ != error_slave_number)
            {
                // When the sensor responds with a diffrent slave number use that for following messages
                Serial.print("Slave number is incorrect should be: ");
                Serial.println(error_slave_number, HEX);
                return;
            }

            byte error_function_code = response[1];
            // Check if function code is correct
            if (error_function_code != MurataSoilSensor::FunctionCode::kFunctionCodeError)
            {
                Serial.print("Error function code is:");
                Serial.println(error_function_code, HEX);
                return;
            }

            // Print error corresponding to error code
            byte error_code = response[2];
            PrintError(error_code);
        }
        else if (response_length < 1)
        {
            Serial.println("No message was returned");
        }
        else
        {
            Serial.print("Message length unidentified\nlength was: ");
            Serial.println(response_length);
            Serial.print("Function code was: ");
            Serial.println(response[1]);
        }
    }

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