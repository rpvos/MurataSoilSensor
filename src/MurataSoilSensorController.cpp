#include <MurataSoilSensorController.h>
#include <MurataSoilSensor.h>
#include <Crc.h>

MurataSoilSensorController::MurataSoilSensorController(RS485 *serial, int enable_pin)
{
    this->serial = serial;
    this->enable_pin = enable_pin;

    pinMode(enable_pin, OUTPUT);
    Enable();
}

void MurataSoilSensorController::Enable()
{
    digitalWrite(enable_pin, HIGH);
}
void MurataSoilSensorController::Disable()
{
    digitalWrite(enable_pin, LOW);
}

void MurataSoilSensorController::SendStructAsHex(byte *request, int size)
{
    for (int i = 0; i < size; i++)
    {
        // Make 15 and below be 0x0F instead of 0xF
        if (request[i] < 16)
        {
            serial->print(0);
        }

        serial->print(request[i], HEX);
    }
    serial->flush();
}

byte *MurataSoilSensorController::ConvertToByteArray(String hexString)
{
    // Convert input which is a hex decimal string to char (unsigned short) array
    byte charArrayResponse[sizeof(ResponseStartMeasurement)];
    for (size_t i = 0; i < hexString.length(); i += 2)
    {
        byte c;
        sscanf(hexString.substring(i, i + 1).c_str(), "%x", c);
        charArrayResponse[i / 2] = c;
    }

    return charArrayResponse;
}

bool MurataSoilSensorController::StartMeasurement(RequestStartMeasurement request)
{
    byte *requestCharPointer = reinterpret_cast<byte *>(&request);

    SendStructAsHex(requestCharPointer, sizeof(RequestStartMeasurement));

    serial->WaitForInput();
    ResponseStartMeasurement *response = nullptr;
    if (serial->available())
    {
        String input = serial->readString();
        // Convert haxadecimal string to char array
        byte *charArrayResponse = ConvertToByteArray(input);

        // Check the CRC
        // TODO check if /2 is correct
        if (ValidateCRC(charArrayResponse, input.length() / 2) == false)
        {
            return false;
        }

        // Cast to responseStartMeasurement
        response = reinterpret_cast<ResponseStartMeasurement *>(charArrayResponse);
    }

    // Check the address
    if (request.address != response->address)
    {
        return false;
    }

    // Check the register
    if (request.registerNumber != response->registerNumber)
    {
        return false;
    }

    return true;
}