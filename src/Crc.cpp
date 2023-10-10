#include "crc.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

namespace Crc
{

    uint16_t Crc16(const uint8_t *data, const size_t size)
    {
        uint16_t cr = 0xFFFF;

        for (size_t i = 0; i < size; i++)
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
    bool ValidateCrc(const uint8_t *message, const size_t message_length)
    {
        // -2 because CRC is 2 bytes
        uint8_t withoutCRC[message_length - 2];
        memcpy(withoutCRC, message, message_length - 2);

        uint16_t crc = Crc16(withoutCRC, message_length - 2);
        uint8_t lower_byte_crc = crc & 0xFF;
        uint8_t upper_byte_crc = (crc >> 8) & 0xFF;

        // Crc lower byte is second last byte and upper byte is last byte
        if (message[message_length - 2] == lower_byte_crc && message[message_length - 1] == upper_byte_crc)
        {
            return true;
        }
        return false;
    }

    // Helper function to add CRC to message
    void AddCrc(const uint8_t *message_without_crc, uint8_t *message_with_crc, const size_t message_length_without_crc)
    {
        uint16_t crc = Crc16(message_without_crc, message_length_without_crc);
        uint8_t lower_byte_Crc = crc & 0xFF;
        uint8_t upper_byte_Crc = (crc >> 8) & 0xFF;

        memcpy(message_with_crc, message_without_crc, message_length_without_crc);
        message_with_crc[message_length_without_crc] = lower_byte_Crc;
        message_with_crc[message_length_without_crc + 1] = upper_byte_Crc;
    }
} // namespace Crc