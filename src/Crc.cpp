#include "crc.h"
#include <Arduino.h>

word Crc16(const byte *data, int size)
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
bool ValidateCrc(const byte *message, int message_length)
{
    // -2 because CRC is 2 bytes
    byte withoutCRC[message_length - 2];
    memcpy(withoutCRC, message, message_length - 2);

    word crc = Crc16(withoutCRC, message_length - 2);
    byte lower_byte_crc = crc & 0xFF;
    byte upper_byte_crc = (crc >> 8) & 0xFF;

    // Crc lower byte is second last byte and upper byte is last byte
    if (message[message_length - 2] == lower_byte_crc && message[message_length - 1] == upper_byte_crc)
    {
        return true;
    }
    return false;
}

// Helper function to add CRC to message
void AddCrc(byte *message_without_crc, byte *message_with_crc, int message_length_without_crc)
{
    word crc = Crc16(message_without_crc, message_length_without_crc);
    byte lower_byte_Crc = crc & 0xFF;
    byte upper_byte_Crc = (crc >> 8) & 0xFF;

    memcpy(message_with_crc, message_without_crc, message_length_without_crc);
    message_with_crc[message_length_without_crc] = lower_byte_Crc;
    message_with_crc[message_length_without_crc + 1] = upper_byte_Crc;
}