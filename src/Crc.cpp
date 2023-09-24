#include <Arduino.h>

word CRC16(byte *data, int size)
{
    word cr = 0xFFFF;

    for (int i = 0; i < size; i++)
    {
        cr = cr ^ data[i];
        for (int j = 0; j < 8; j++)
        {
            if ((cr & 0x0001) == 0x0001)
            {
                cr >>= 1;
                cr ^= 0xA001;
            }
            else
            {
                cr >>= 1;
            }
        }
    }
    return cr;
}

// Helper function to validate CRC in message
bool ValidateCRC(byte *message, int messageLength)
{
    // -2 because CRC is 2 bytes
    byte withoutCRC[messageLength - 2];
    memcpy(message, withoutCRC, messageLength - 2);

    unsigned short crc = CRC16(withoutCRC, messageLength - 2);
    byte lowerWordCRC = crc & 0xFF;
    byte upperWordCRC = (crc >> 8) & 0xFF;

    if (message[messageLength - 1] == lowerWordCRC && message[messageLength] == upperWordCRC)
    {
        return true;
    }
    return false;
}

// Helper function to add CRC to message
void AddCRC(byte *message, int messageLength)
{
    unsigned int crc = CRC16(message, messageLength);
    word lowerWordCRC = crc & 0xFF;
    word upperWordCRC = (crc >> 8) & 0xFF;

    byte newMessage[messageLength + 2];
    memcpy(message, newMessage, messageLength);
    newMessage[messageLength] = lowerWordCRC;
    newMessage[messageLength + 1] = upperWordCRC;

    // TODO might not work like this
    message = newMessage;
}