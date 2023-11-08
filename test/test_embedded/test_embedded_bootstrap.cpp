#include <unity.h>
#include <Arduino.h>
#include "test_murata_soil_sensor_controller.hpp"
#include "test_murata_soil_sensor.hpp"

void test_Framework(void)
{
    TEST_ASSERT_TRUE(true);
}

void setup()
{
    // Wait ~2 seconds before the Unity test runner
    // establishes connection with a board Serial interface
    delay(2000);

    UNITY_BEGIN();
    RUN_TEST(test_Framework);
    RUN_TEST(test_StartMeasurement);
    RUN_TEST(test_IsMeasurementFinished);
    RUN_TEST(test_ReadMeasurementData);
    TestMurataSoilSensor();
    UNITY_END();
}

void loop() { delay(1000); }