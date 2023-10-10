#include <Arduino.h>
#include <MAX485TTL.h>
#include <murata_soil_sensor.h>
#include <murata_soil_sensor_controller.h>

#define ENABLE_PIN 4

byte kSlaveNumber = 1;

RS485 rs = RS485(2, 3, &Serial1);
// TODO make an example project
// MurataSoilSensorController controller = MurataSoilSensorController(&rs, ENABLE_PIN, kSlaveNumber);

void setup()
{
  Serial.begin(9600);

  Serial1.begin(9600);

  // Header header = Header(kSlaveNumber, Header::kWriteNWords);
  // word address = 0x000A;
  // word number_of_registers = 0x0001;
  // word register_value = 0x0001;

  // RequestStartMeasurement request = {header, address, number_of_registers, register_value};
  // Serial.print(controller.StartMeasurement(request));
}

void loop()
{
  delay(1000);
}
