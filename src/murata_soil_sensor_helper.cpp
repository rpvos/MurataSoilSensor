#include "murata_soil_sensor_helper.h"
#include <crc16.h>

namespace MurataSoilSensorHelper
{
    /**
     * @brief Helper function to convert unsigned 12 bit value to signed 12 bit value
     *
     * @param u12 unsigned 12 bit value
     * @return int16_t signed 16 bit value
     */
    word Signed12BitToSigned16Bit(word u12)
    {
        u12 &= 0xFFF; // optionally, mask out potential clutter in upper bytes

        if (u12 & (1u << 11)) // if signed, bit 11 is set
        {
            u12 |= 0xFFFFFFu << 12; // "sign extend"
        }

        return u12; // unsigned to signed conversion
    }

    double CalculateTemperature(word value)
    {
        //  Convert to 12 bit 2 complement value
        word dataSigned = Signed12BitToSigned16Bit(value);
        return dataSigned * 0.0625;
    }

    double CalculateECBulk(word value)
    {
        double ecIndSM = value * 0.001;
        return ecIndSM;
    }

    double CalculateVWC(word value)
    {
        double vwcInPercentage = value * 0.1;
        return vwcInPercentage;
    }

    double CalculateECPore(word value)
    {
        double ecIndSM = value * 0.001;
        return ecIndSM;
    }

} // namespace MurataSoilSensor