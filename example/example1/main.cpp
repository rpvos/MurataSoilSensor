#include <Arduino.h>
#include <max485ttl.hpp>
#include <murata_soil_sensor_helper.h>
#include <murata_soil_sensor.h>

const uint8_t kRs485EnablePin = 7;
const uint8_t kSoilSensorEnablePin = 6;

const byte kCurrentSlaveNumber = 1;
const byte kSlaveNumber = 1;

RS485 *rs;
MurataSoilSensor::MurataSoilSensor *sensor;

int state;

void setup()
{
  Serial.begin(9600);
  // Serial using 8 bits, No parity and 1 stop bit
  Serial2.begin(9600, SERIAL_8N1);

  rs = new RS485(kRs485EnablePin, kRs485EnablePin, &Serial2);
  sensor = new MurataSoilSensor::MurataSoilSensor(rs, kSoilSensorEnablePin, kCurrentSlaveNumber);

  state = 1;
}

void loop()
{
  MurataSoilSensorError response_code;
  switch (state)
  {
  case 0:
  {
    response_code = sensor->SetSlaveNumber(kSlaveNumber);
    if (response_code == MurataSoilSensorError::kOk)
    { // Create distance between state completes
      Serial.println("Slave number is set");
      state++;
    }
    break;
  }
  case 1:
  {
    response_code = sensor->StartMeasurement();
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
    response_code = sensor->IsMeasurementFinished(isFinished);
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
    MurataSoilSensorHelper::MeasurementData data;
    response_code = sensor->ReadMeasurementData(data);
    if (response_code == MurataSoilSensorError::kOk)
    {
      Serial.print("Temperature:");
      Serial.println(MurataSoilSensorHelper::CalculateTemperature(data.temperature));
      Serial.print("EC bulk:");
      Serial.println(MurataSoilSensorHelper::CalculateECBulk(data.ec_bulk));
      Serial.print("VWC:");
      Serial.println(MurataSoilSensorHelper::CalculateVWC(data.vwc));
      Serial.print("EC pore:");
      Serial.println(MurataSoilSensorHelper::CalculateECPore(data.ec_pore));
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
