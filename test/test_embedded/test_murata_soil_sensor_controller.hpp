#include <unity.h>
#include <murata_soil_sensor_controller.h>
#include <memory_stream.h>
#include <MAX485TTL.h>

MemoryStream *memory_stream;
RS485 *rs485;
MurataSoilSensorController::MurataSoilSensorController *controller;
const uint8_t kSlaveNumber = 1;
const int buffer_size = 64;
char buffer[buffer_size];

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

    {     // Test start measurement
        { // Setup
            const uint8_t size = 8;
            const byte querry[size] = {0x01, 0x10, 0x00, 0x0A, 0x00, 0x01, 0x21, 0xCB};

            memory_stream->AddOutput(reinterpret_cast<const char *>(querry), size);
        }

        bool has_succeeded = controller->StartMeasurement();
        TEST_ASSERT_TRUE(has_succeeded);

        { // Check sent message
            const uint8_t size = 11;
            const byte response[size] = {0x01, 0x10, 0x00, 0x0A, 0x00, 0x01, 0x02, 0x00, 0x01, 0x67, 0x3A};
            memory_stream->ReadInput(buffer, buffer_size);
            TEST_ASSERT_EQUAL_HEX_ARRAY(response, buffer, size);
            memory_stream->ReadInput(buffer, buffer_size);
            TEST_ASSERT_EQUAL_STRING("", buffer);
        }
    }
}

void test_IsMeasurementFinished(void)
{

    {     // Test start measurement
        { // Setup
            const uint8_t size = 7;
            const byte querry[size] = {0x01, 0x03, 0x02, 0x00, 0x00, 0xB8, 0x44};

            memory_stream->AddOutput(reinterpret_cast<const char *>(querry), size);
        }

        bool is_finished = false;
        bool has_succeeded = controller->IsMeasurementFinished(is_finished);
        TEST_ASSERT_TRUE(has_succeeded);
        TEST_ASSERT_TRUE(is_finished);

        { // Check sent message
            const uint8_t size = 8;
            const byte response[size] = {0x01, 0x03, 0x00, 0x0C, 0x00, 0x01, 0x44, 0x09};
            memory_stream->ReadInput(buffer, buffer_size);
            TEST_ASSERT_EQUAL_HEX_ARRAY(response, buffer, size);
            memory_stream->ReadInput(buffer, buffer_size);
            TEST_ASSERT_EQUAL_STRING("", buffer);
        }
    }

    {     // Test start measurement
        { // Setup
            const uint8_t size = 7;
            const byte querry[size] = {0x01, 0x03, 0x02, 0x00, 0x01, 0x79, 0x84};

            memory_stream->AddOutput(reinterpret_cast<const char *>(querry), size);
        }

        bool is_finished = false;
        bool has_succeeded = controller->IsMeasurementFinished(is_finished);
        TEST_ASSERT_TRUE(has_succeeded);
        TEST_ASSERT_TRUE(is_finished);

        { // Check sent message
            const uint8_t size = 8;
            const byte response[size] = {0x01, 0x03, 0x00, 0x0C, 0x00, 0x01, 0x44, 0x09};
            memory_stream->ReadInput(buffer, buffer_size);
            TEST_ASSERT_EQUAL_HEX_ARRAY(response, buffer, size);
            memory_stream->ReadInput(buffer, buffer_size);
            TEST_ASSERT_EQUAL_STRING("", buffer);
        }
    }
}

void test_ReadMeasurementData(void)
{

    {     // Test start measurement
        { // Setup
            const uint8_t size = 7;
            const byte querry[size] = {0x01, 0x03, 0x02, 0x00, 0x00, 0xB8, 0x44};

            memory_stream->AddOutput(reinterpret_cast<const char *>(querry), size);
        }

        MurataSoilSensor::MeasurementData data;
        bool has_succeeded = controller->ReadMeasurementData(data);
        TEST_ASSERT_TRUE(has_succeeded);
        // TODO check values of data

        { // Check sent message
            const uint8_t size = 8;
            const byte response[size] = {0x01, 0x03, 0x00, 0x0C, 0x00, 0x01, 0x44, 0x09};
            memory_stream->ReadInput(buffer, buffer_size);
            TEST_ASSERT_EQUAL_HEX_ARRAY(response, buffer, size);
            memory_stream->ReadInput(buffer, buffer_size);
            TEST_ASSERT_EQUAL_STRING("", buffer);
        }
    }
}