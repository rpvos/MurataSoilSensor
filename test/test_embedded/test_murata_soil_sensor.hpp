#include "murata_soil_sensor.h"
#include "unity.h"

void test_Unsigned12BitToSigned16Bit(void)
{
    word test_set1 = 0x0002; // 2
    word test_set2 = 0x0001; // 1
    word test_set3 = 0x0000; // 0
    word test_set4 = 0x0FFF; // -1
    word test_set5 = 0x0FFE; // -2
    word test_set6 = 0x0800; // -2048
    word test_set7 = 0x07FF; // 2047

    int output = MurataSoilSensor::Unsigned12BitToSigned16Bit(test_set1);
    TEST_ASSERT_EQUAL_INT_MESSAGE(2, output, "Positive value was not converted correctly");

    output = MurataSoilSensor::Unsigned12BitToSigned16Bit(test_set2);
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, output, "Positive value was not converted correctly");

    output = MurataSoilSensor::Unsigned12BitToSigned16Bit(test_set3);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, output, "0 was not converted correctly");

    output = MurataSoilSensor::Unsigned12BitToSigned16Bit(test_set4);
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1, output, "Negative value was not converted correctly");

    output = MurataSoilSensor::Unsigned12BitToSigned16Bit(test_set5);
    TEST_ASSERT_EQUAL_INT_MESSAGE(-2, output, "Negative value was not converted correctly");

    output = MurataSoilSensor::Unsigned12BitToSigned16Bit(test_set6);
    TEST_ASSERT_EQUAL_INT_MESSAGE(-2048, output, "Negative max value was not converted correctly");

    output = MurataSoilSensor::Unsigned12BitToSigned16Bit(test_set7);
    TEST_ASSERT_EQUAL_INT_MESSAGE(2047, output, "Positive max value was not converted correctly");
}

void TestMurataSoilSensor(void)
{
    RUN_TEST(test_Unsigned12BitToSigned16Bit);
}
