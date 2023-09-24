#include <Arduino.h>
#include <MAX485TTL.h>
#include <MurataSoilSensorController.h>

#define ENABLE_PIN 4

RS485 rs = RS485(2, 3, &Serial1);
MurataSoilSensorController controller = MurataSoilSensorController(&rs, ENABLE_PIN);

void setup()
{
  Serial.begin(9600);

  Serial1.begin(9600);
  controller.Enable();
  RequestStartMeasurement request(Header(1, 1), 1, 1, 1, 1);
  controller.StartMeasurement(request);
}

void loop()
{
}
