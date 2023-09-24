
#include <MurataSoilSensor.h>
#include <Crc.h>

/**
 * @brief Helper function to convert unsigned 12 bit value to signed 12 bit value
 *
 * @param u12 unsigned 12 bit value
 * @return short signed 16 bit value
 */
short U12_to_S16(word u12)
{
    u12 &= 0xFFF; // optionally, mask out potential clutter in upper bytes

    if (u12 & (1u << 11)) // if signed, bit 11 is set
    {
        u12 |= 0xFFFFFFu << 12; // "sign extend"
    }

    return u12; // unsigned to signed conversion
}

double calculateTemp(RegisterValue value)
{
    // TODO test
    word dataUnsigned = (value.lower << 8) | value.lower;
    //  Convert to 12 bit 2 complement value
    short dataSigned = U12_to_S16(dataUnsigned);
    // tempValue_t tempData = {value}; // Other method to try
    // double tempInC = tempData.value * 0.0625;
    double tempInC = dataSigned * 0.0625;
    return tempInC;
}

double calculateECBulk(RegisterValue value)
{
    word data = (value.lower << 8) | value.upper;
    double ecIndSM = data * 0.001;
    return ecIndSM;
}

double calculateVWC(RegisterValue value)
{
    word data = (value.lower << 8) | value.upper;
    double vwcInPercentage = data * 0.1;
    return vwcInPercentage;
}

double calculateECPore(RegisterValue value)
{
    word data = (value.lower << 8) | value.upper;
    double ecIndSM = data * 0.001;
    return ecIndSM;
}

Address::Address(word value)
{
    this->lower = (value >> 16);
    this->upper = (value & 0xFF);
}

Header::Header(byte slaveNumber, byte functionCode)
{
    this->slaveNumber = slaveNumber;
    this->functionCode = functionCode;
};

Header::Header()
{
    this->slaveNumber = 1;
    this->functionCode = kReadNWords;
}

RequestStartMeasurement::RequestStartMeasurement(Header header, word address, word registerNumber, word dataLength, word registerValue)
{
    this->header = header;
    this->address = address;
    this->registerNumber = registerNumber;
    this->dataLength = dataLength;
    this->registerValue = registerValue;
};
