#ifndef CRC_H_
#define CRC_H_

#include <Arduino.h>

word CRC16(byte *data, int size);
bool ValidateCRC(byte *message, int messageLength);
void AddCRC(byte *message, int messageLength);

#endif // CRC_H_