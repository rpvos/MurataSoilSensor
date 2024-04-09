#include "murata_soil_sensor.h"
#include <crc16.hpp>

#ifndef kDelayTimeAfterSwitch
#define kDelayTimeAfterSwitch 10
#endif

namespace MurataSoilSensor
{

    MurataSoilSensor::MurataSoilSensor(RS485 *serial, uint8_t enable_pin, byte slave_number)
    {
        this->serial_ = serial;
        this->enable_pin_ = enable_pin;

        modbus_.SetSlaveId(slave_number);
        pinMode(enable_pin, OUTPUT);
        Enable();
    }

    MurataSoilSensor::~MurataSoilSensor()
    {
    }

    void MurataSoilSensor::Enable()
    {
        digitalWrite(enable_pin_, HIGH);
    }
    void MurataSoilSensor::Disable()
    {
        digitalWrite(enable_pin_, LOW);
    }

    MurataSoilSensorError MurataSoilSensor::SetSlaveNumber(byte slave_number, bool broadcast)
    {
        if (slave_number > 31)
        {
            Serial.println("Slave number must be between 0 and 31");
            return MurataSoilSensorError::kInvalidParameter;
        }

        word slave_number_word = slave_number;
        MurataSoilSensorError response_code = WriteRegister(MurataSoilSensorHelper::RegisterNumber::kRegisterSensorNumber, 0x0001, &slave_number_word, broadcast);
        if (response_code == MurataSoilSensorError::kOk)
        {
            this->modbus_.SetSlaveId(slave_number);
        }

        return response_code;
    }

    MurataSoilSensorError MurataSoilSensor::StartMeasurement(void)
    {
        word start_measurement = 0x0001;
        return WriteRegister(MurataSoilSensorHelper::RegisterNumber::kRegisterSensorControl, 0x0001, &start_measurement);
    }

    MurataSoilSensorError MurataSoilSensor::IsMeasurementFinished(bool &isFinished)
    {
        word value;
        MurataSoilSensorError response_code = ReadRegister(MurataSoilSensorHelper::RegisterNumber::kRegisterSensorState, 0x0001, &value);
        isFinished = value;
        return response_code;
    }

    MurataSoilSensorError MurataSoilSensor::ReadMeasurementData(MurataSoilSensorHelper::MeasurementData &measurement_data)
    {
        // 7 registers are needed to extract all sensor data starting from temperature register
        word number_of_registers = 0x0007;
        word value[number_of_registers];

        MurataSoilSensorError response_code = ReadRegister(MurataSoilSensorHelper::RegisterNumber::kRegisterTemperature, number_of_registers, value);

        if (response_code == MurataSoilSensorError::kOk)
        {
            measurement_data.temperature = value[0];
            measurement_data.ec_bulk = value[1];
            measurement_data.vwc = value[3];
            measurement_data.ec_pore = value[6];
        }

        return response_code;
    }

    MurataSoilSensorError MurataSoilSensor::WriteRegister(const word address, const word number_of_registers, const word *register_values, bool broadcast)
    {
        const byte function_code = MurataSoilSensorHelper::FunctionCode::kFunctionCodeWriteNWords;

        // Fill const elements of message with values
        size_t message_length = modbus_.GetWriteMessageLength(number_of_registers);
        byte message[message_length];
        modbus_.ConstructWriteMessage(message, function_code, address, number_of_registers, register_values, broadcast);

        serial_->SetMode(OUTPUT);
        delay(kDelayTimeAfterSwitch);
        serial_->write(message, message_length);
        serial_->flush();
        serial_->SetMode(INPUT);

        // If message is broadcasted no message will be returned
        if (broadcast)
        {
            return MurataSoilSensorError::kOk;
        }

        // Receive response
        serial_->WaitForInput();
        int response_length = serial_->available();
        if (response_length < 1)
        {
            Serial.println("No message was returned");
            return MurataSoilSensorError::kNoMessageReturned;
        }

        byte buffer[response_length];
        for (int i = 0; i < response_length; i++)
        {
            buffer[i] = serial_->read();
        }

        if (modbus_.ValidateWriteResponse(buffer, function_code, address, number_of_registers, broadcast))
        {
            return MurataSoilSensorError::kOk;
        }

        HandleError(buffer, response_length);

        // TODO: remove
        //  // Print debug info of message
        //  Serial.print("Message length was: ");
        //  Serial.println(response_length);
        //  // If length is smaller then 2 no function code was given
        //  if (response_length > 1)
        //  {
        //      Serial.print("Function code was: 0x");
        //      Serial.println(function_code, HEX);
        //  }
        //  // Print message in hex
        //  Serial.print("Message was: 0x");
        //  for (int i = 0; i < response_length; i++)
        //  {
        //      Serial.print(buffer[i], HEX);
        //      Serial.print(' ');
        //  }
        //  Serial.println();

        return MurataSoilSensorError::kIncorrectReturnMessage;
    }

    MurataSoilSensorError MurataSoilSensor::ReadRegister(const word address, const word number_of_registers, word *register_values)
    {
        const byte function_code = MurataSoilSensorHelper::FunctionCode::kFunctionCodeReadNWords;

        // Fill const elements of message with values
        size_t message_length = modbus_.GetReadMessageLength();
        byte message[message_length];
        modbus_.ConstructReadMessage(message, function_code, address, number_of_registers);

        serial_->SetMode(OUTPUT);
        delay(kDelayTimeAfterSwitch);
        serial_->write(message, message_length);
        serial_->flush();
        serial_->SetMode(INPUT);

        // Receive response
        serial_->WaitForInput();
        // Wait 10 ms so full message can be received, otherwise only first bit will be avaiable without wait
        delay(10);
        int response_length = serial_->available();
        byte buffer[response_length];
        for (int i = 0; i < response_length; i++)
        {
            buffer[i] = serial_->read();
        }

        if (modbus_.ValidateReadResponse(buffer, function_code, number_of_registers))
        {

            modbus_.GetReadRegister(buffer, number_of_registers, register_values);
            return MurataSoilSensorError::kOk;
        }

        HandleError(buffer, response_length);
        return MurataSoilSensorError::kIncorrectReturnMessage;
    }

    void MurataSoilSensor::HandleError(const byte *response, const int response_length)
    {
        if (response[1] == MurataSoilSensorHelper::FunctionCode::kFunctionCodeError)
        {

            if (Crc::ValidateCrc16Modbus(response, response_length, false))
            {
                Serial.println("Crc of error not correct");
                return;
            }

            byte error_slave_number = response[0];
            // Check if slave number is correct
            if (modbus_.GetSlaveId() != error_slave_number)
            {
                // When the sensor responds with a diffrent slave number use that for following messages
                Serial.print("Slave number is incorrect should be: ");
                Serial.println(error_slave_number, HEX);
            }

            byte error_function_code = response[1];
            // Check if function code is correct
            if (error_function_code != MurataSoilSensorHelper::FunctionCode::kFunctionCodeError)
            {
                Serial.print("Error function code is:");
                Serial.println(error_function_code, HEX);
            }

            // Print error corresponding to error code
            byte error_code = response[2];
            PrintError(error_code);
        }
    }

    void MurataSoilSensor::PrintError(byte error_code)
    {
        switch (error_code)
        {
        case MurataSoilSensorHelper::ErrorCode::kErrorCodeIllegalFunctionCode:
            Serial.println("Error illegal function code");
            break;
        case MurataSoilSensorHelper::ErrorCode::kErrorCodeIllegalStartAddress:
            Serial.println("Error illegal start address");
            break;
        case MurataSoilSensorHelper::ErrorCode::kErrorCodeIllegalProtocolOrFormat:
            Serial.println("Error illegal protocol or format");
            break;
        case MurataSoilSensorHelper::ErrorCode::kErrorCodeCRC16Error:
            Serial.println("Error CRC16 not correct");
            break;
        case MurataSoilSensorHelper::ErrorCode::kErrorCodeSensorIsUnderMeasurement:
            Serial.println("Error sensor is under measurement");
            break;
        case MurataSoilSensorHelper::ErrorCode::kErrorCodeFailedToWriteToRegister:
            Serial.println("Error failed to write to register");
            break;
        case MurataSoilSensorHelper::ErrorCode::kErrorCodeI2CCommunicationError:
            Serial.println("Error I2C communication error");
            break;
        default:
            Serial.println("Unknown error code");
            break;
        }
    }

} // namespace MurataSoilSensor