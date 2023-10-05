#include <unity.h>
#include <Arduino.h>
#include "test_crc.hpp"
#include "test_murata_soil_sensor.hpp"
#include "test_murata_soil_sensor_controller.hpp"

void setUp(void)
{
}

void tearDown(void)
{
}

void setup()
{
    // Wait ~2 seconds before the Unity test runner
    // establishes connection with a board Serial interface
    delay(2000);

    UNITY_BEGIN();
    TestCrc();
    TestMurataSoilSensor();
    TestMurataSoilSensorController();
    UNITY_END();
}

void loop() { delay(1000); }