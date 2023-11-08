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
            const uint8_t size = 19;
            const byte querry[size] = {0x01, 0x03, 0x0E, 0x01, 0xC9, 0x00, 0x06, 0x01, 0x66, 0x01, 0x98, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE7, 0xF6};
            memory_stream->AddOutput(reinterpret_cast<const char *>(querry), size);
        }

        MurataSoilSensor::MeasurementData data;
        bool has_succeeded = controller->ReadMeasurementData(data);
        TEST_ASSERT_TRUE(has_succeeded);

        double temperature = MurataSoilSensor::CalculateTemperature(data.temperature);
        double ec_bulk = MurataSoilSensor::CalculateECBulk(data.ec_bulk);
        double vwc = MurataSoilSensor::CalculateVWC(data.vwc);
        double ec_pore = MurataSoilSensor::CalculateECPore(data.ec_pore);
        TEST_ASSERT_DOUBLE_WITHIN(0.285, 28.5, temperature);
        TEST_ASSERT_DOUBLE_WITHIN(0.00006, 0.006, ec_bulk);
        TEST_ASSERT_DOUBLE_WITHIN(0.408, 40.8, vwc);
        TEST_ASSERT_DOUBLE_WITHIN(0.00001, 0.000, ec_pore);

        { // Check sent message
            const uint8_t size = 8;
            const byte response[size] = {0x01, 0x03, 0x00, 0x16, 0x00, 0x07, 0xE5, 0xCC};
            memory_stream->ReadInput(buffer, buffer_size);
            TEST_ASSERT_EQUAL_HEX_ARRAY(response, buffer, size);
            memory_stream->ReadInput(buffer, buffer_size);
            TEST_ASSERT_EQUAL_STRING("", buffer);
        }
    }
}