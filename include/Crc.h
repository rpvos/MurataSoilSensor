#ifndef CRC_H_
#define CRC_H_
#include <stdint.h>
#include <stdio.h>

namespace Crc
{

    uint16_t Crc16(const uint8_t *data, const size_t size);
    bool ValidateCrc(const uint8_t *message, const size_t messageLength);

    /**
     * @brief Function used to add crc to a message
     *
     * @param message_without_crc source
     * @param message_with_crc destination array that will be used to store message with crc
     * @param message_length_without_crc
     */
    void AddCrc(const uint8_t *message_without_crc, uint8_t *message_with_crc, const size_t message_length_without_crc);
} // namespace Crc

#endif // CRC_H_