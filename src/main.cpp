#include <Arduino.h>
#include <MAX485TTL.hpp>
#include <murata_soil_sensor.h>
#include <murata_soil_sensor_controller.h>

const uint8_t kEnablePin = 4;

const byte kSlaveNumber = 1;

RS485 *rs;
MurataSoilSensorController::MurataSoilSensorController *controller;

int state;

void setup()
{
  Serial.begin(9600);
  // Serial using 8 bits, No parity and 1 stop bit
  Serial1.begin(9600, SERIAL_8N1);

  rs = new RS485(2, 3, &Serial1);
  controller = new MurataSoilSensorController::MurataSoilSensorController(rs, kEnablePin, kSlaveNumber);

  state = 1;
}

void loop()
{
  MurataSoilSensorError response_code;
  switch (state)
  {
  case 0:
  {
    response_code = controller->SetSlaveNumber(kSlaveNumber, true);
    if (response_code == MurataSoilSensorError::kOk)
    { // Create distance between state completes
      Serial.println("Slave number is set via broadcast");
      state++;
    }
    break;
  }
  case 1:
  {
    response_code = controller->StartMeasurement();
    if (response_code == MurataSoilSensorError::kOk)
    {
      Serial.println("Measurement has started");
      state++;
    }
    break;
  }
  case 2:
  {
    bool isFinished = false;
    response_code = controller->IsMeasurementFinished(isFinished);
    // If register is succefully retrieved
    if (response_code == MurataSoilSensorError::kOk)
    {
      if (isFinished)
      {
        Serial.println("Measurement has finished");
        state++;
      }
      else
      {
        Serial.println("Measurement ongoing");
      }
    }
    break;
  }
  case 3:
  {
    MurataSoilSensor::MeasurementData data;
    response_code = controller->ReadMeasurementData(data);
    if (response_code == MurataSoilSensorError::kOk)
    {
      Serial.print("Temperature:");
      Serial.println(MurataSoilSensor::CalculateTemperature(data.temperature));
      Serial.print("EC bulk:");
      Serial.println(MurataSoilSensor::CalculateECBulk(data.ec_bulk));
      Serial.print("VWC:");
      Serial.println(MurataSoilSensor::CalculateVWC(data.vwc));
      Serial.print("EC pore:");
      Serial.println(MurataSoilSensor::CalculateECPore(data.ec_pore));
      state++;
    }
    break;
  }
  case 4:
  {
    Serial.println(".");

    break;
  }
  default:
  {
    Serial.println("Unknown state");
    break;
  }
  } // switch state

  // Print error
  if (static_cast<int>(response_code))
  {
    Serial.print("State: ");
    Serial.println(state);
    Serial.print("Response code: ");
    Serial.println(static_cast<int>(response_code));
  }

  delay(5000);
}
