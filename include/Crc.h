#ifndef CRC_H_
#define CRC_H_

#include <Arduino.h>

word Crc16(const byte *data, int size);
bool ValidateCrc(const byte *message, int messageLength);

/**
 * @brief Function used to add crc to a message
 *
 * @param message_without_crc source
 * @param message_with_crc destination array that will be used to store message with crc
 * @param message_length_without_crc
 */
void AddCrc(byte *message_without_crc, byte *message_with_crc, int message_length_without_crc);

#endif // CRC_H_