/* 06/16/2017 Copyright Tlera Corporation
 *  
 *  Created by Kris Winer
 *  
 This sketch uses SDA/SCL on pins 42/43 (back pads), respectively, and it uses the Dragonfly STM32L476RE Breakout Board.
 The BME280 is a simple but high resolution pressure/humidity/temperature sensor, which can be used in its high resolution
 mode but with power consumption of 20 microAmp, or in a lower resolution mode with power consumption of
 only 1 microAmp. The choice will depend on the application.
 
 Library may be used freely and without limit with attribution.
 
*/
#include "BME280.h"

#define myLed   13  // board led

/* Specify BME280 configuration
 *  Choices are:
 P_OSR_01, P_OSR_02, P_OSR_04, P_OSR_08, P_OSR_16 // pressure oversampling
 H_OSR_01, H_OSR_02, H_OSR_04, H_OSR_08, H_OSR_16 // humidity oversampling
 T_OSR_01, T_OSR_02, T_OSR_04, T_OSR_08, T_OSR_16 // temperature oversampling
 full, BW0_223ODR,BW0_092ODR, BW0_042ODR, BW0_021ODR // bandwidth at 0.021 x sample rate
 BME280Sleep, forced,, forced2, normal //operation modes
 t_00_5ms = 0, t_62_5ms, t_125ms, t_250ms, t_500ms, t_1000ms, t_10ms, t_20ms // determines sample rate
 */
uint8_t Posr = P_OSR_16, Hosr = H_OSR_16, Tosr = T_OSR_02, Mode = normal, IIRFilter = BW0_042ODR, SBy = t_62_5ms;     // set pressure amd temperature output data rate
// t_fine carries fine temperature as global value for BME280

float Temperature, Pressure, Humidity;   // stores BME280 pressures sensor pressure and temperature
int32_t rawPress, rawTemp, rawHumidity;  // pressure, humidity, and temperature raw count output for BME280


float   temperature_C, temperature_F, pressure, humidity, altitude; // Scaled output of the BME280

uint32_t delt_t = 0, count = 0, sumCount = 0, slpcnt = 0;  // used to control display output rate

   BME280 BME280; // instantiate BME280 class

void setup()
{
  pinMode(myLed, OUTPUT);
  digitalWrite(myLed, LOW); // start with the led on since active LOW

  Serial.begin(115200);
  delay(4000);
  Wire.begin(); // set master mode, on Ladybug uses pins 20/21 by default
  Wire.setClock(400000); // I2C frequency at 400 kHz  
  delay(1000);

  BME280.I2Cscan();  // check for I2C devices on the bus

  // Read the WHO_AM_I register of the BME280 this is a good test of communication
  byte f = BME280.getChipID();  // Read WHO_AM_I register for BME280
  Serial.print("BME280 "); Serial.print("I AM "); Serial.print(f, HEX); Serial.print(" I should be "); Serial.println(0x60, HEX);
  Serial.println(" ");
  
  delay(1000); 

  if(f == 0x60) {
   
  BME280.resetBME280(); // reset BME280 before initilization
  delay(100);

  BME280.BME280Init(Posr, Hosr, Tosr, Mode, IIRFilter, SBy); // Initialize BME280 altimeter

 }
 
  else Serial.println(" BME280 not functioning!");
  
}
 

void loop()
{  
   // Serial print and/or display at 1 s rate independent of data rates
    delt_t = millis() - count;
    if (delt_t > 1000) { // update LCD once per second independent of read rate

    rawPress =  BME280.readBME280Pressure();
    pressure = (float) BME280.BME280_compensate_P(rawPress)/25600.f; // Pressure in mbar
    altitude = 145366.45f*(1.0f - powf((pressure/1013.25f), 0.190284f));
    
    rawTemp =   BME280.readBME280Temperature();
    temperature_C = (float) BME280.BME280_compensate_T(rawTemp)/100.;
    
    rawHumidity =   BME280.readBME280Humidity();
    humidity = (float) BME280.BME280_compensate_H(rawHumidity)/1024.;
 
      Serial.println("BME280:");
      Serial.print("Altimeter temperature = "); 
      Serial.print( temperature_C, 2); 
      Serial.println(" C"); // temperature in degrees Celsius
      Serial.print("Altimeter temperature = "); 
      Serial.print(9.*temperature_C/5. + 32., 2); 
      Serial.println(" F"); // temperature in degrees Fahrenheit
      Serial.print("Altimeter pressure = "); 
      Serial.print(pressure, 2);  
      Serial.println(" mbar");// pressure in millibar
      Serial.print("Altitude = "); 
      Serial.print(altitude, 2); 
      Serial.println(" feet");
      Serial.print("Altimeter humidity = "); 
      Serial.print(humidity, 1);  
      Serial.println(" %RH");// pressure in millibar
      Serial.println(" ");
      
      
      digitalWrite(myLed, !digitalRead(myLed)); // toggle red led
      count = millis(); 
    }
    
}
