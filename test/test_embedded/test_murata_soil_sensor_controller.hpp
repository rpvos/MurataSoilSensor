#include <unity.h>
#include <murata_soil_sensor_controller.h>
#include <memory_stream.h>
#include <MAX485TTL.h>

MemoryStream *memory_stream;
RS485 *rs485;
MurataSoilSensorController::MurataSoilSensorController *controller;
const uint8_t kSlaveNumber = 1;

void setUp(void)
{
    memory_stream = new MemoryStream(64, true);
    rs485 = new RS485(2, 3, memory_stream, true, 64, false);
    controller = new MurataSoilSensorController::MurataSoilSensorController(rs485, 4, kSlaveNumber);
}

void tearDown(void)
{
    delete controller;
    delete rs485;
    delete memory_stream;
}

void test_StartMeasurement(void)
{

    { // Prep the presumed response
        const size_t response_size = 8;
        uint8_t response[response_size] = {0x01, 0x10, 0x00, 0x0A, 0x00, 0x01, 0x21, 0xCB};
        uint8_t *read_buffer = memory_stream->GetSecondBuffer();
        memcpy(read_buffer, response, response_size);
        memory_stream->SetInputLength(response_size);
        memory_stream->SetReadCursor(0);
    }

    TEST_ASSERT_TRUE_MESSAGE(controller->WriteRegister(0x000A, 0x0001, 0x0001), "Start measurement did not succeed");
}