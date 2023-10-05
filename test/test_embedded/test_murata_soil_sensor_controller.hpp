#include "murata_soil_sensor_controller.h"
#include <MAX485TTL.h>
#include "memory_stream.h"
#include <unity.h>

#define kSlaveNumber 1

MemoryStream memory_stream = MemoryStream(64, true);
RS485 rs485 = RS485(2, 3, &memory_stream, true, 64, false);
MurataSoilSensorController controller = MurataSoilSensorController(&rs485, 4, kSlaveNumber);

void test_Constructor(void)
{
    // TODO add get slave number and compare to value that is set in constructor
    // TEST_ASSERT_EQUAL_INT_MESSAGE(kSlaveNumber, slave_number, "Slave number was not set on initialisation");
}

void test_StartMeasurement(void)
{

    { // Prep the presumed response
        const size_t response_size = 8;
        uint8_t response[response_size] = {0x01, 0x10, 0x00, 0x0A, 0x00, 0x01, 0x21, 0xCB};
        uint8_t *read_buffer = memory_stream.GetSecondBuffer();
        memcpy(read_buffer, response, response_size);
        memory_stream.SetInputLength(response_size);
        memory_stream.SetReadCursor(0);
    }

    TEST_ASSERT_TRUE_MESSAGE(controller.StartMeasurement(0x000A, 0x0001, 0x0001), "Start measurement did not succeed");
}

void TestMurataSoilSensorController(void)
{
    RUN_TEST(test_Constructor);
    RUN_TEST(test_StartMeasurement);
}