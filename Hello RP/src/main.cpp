#include <Adafruit_SSD1306.h>
#include <Adafruit_VEML7700.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif
#define PIN       A0
#define NUMPIXELS 16
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
#define DELAYVAL 10
Adafruit_VEML7700 veml = Adafruit_VEML7700();
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);
#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BME280 bme;



void setup() {
  #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
  #endif


  pixels.begin();


  if (!bme.begin(0x76)) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }


  if (veml.begin()) {
    Serial.println("Found a VEML7700 sensor");
  } else {
    Serial.println("No sensor found ... check your wiring?");
    while (1);
  }


  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.display();
  delay(500); // Pause for 2 seconds
  display.setTextSize(1);
  display.setTextColor(WHITE);


  veml.setGain(VEML7700_GAIN_1);
  veml.setIntegrationTime(VEML7700_IT_100MS);
}



void loop() {
  float temp=bme.readTemperature();
  float lumi=veml.readLux();
  if (lumi<100.0){
      pixels.clear();
      for(int i=0; i<NUMPIXELS; i++) {
        pixels.setPixelColor(i, pixels.Color(0, 0, 0));
        pixels.show();
        delay(DELAYVAL);
  }
  }
  else{
    if (temp>30.0){
      pixels.clear();
      for(int i=0; i<NUMPIXELS; i++) {
        pixels.setPixelColor(i, pixels.Color(255, 0, 0));
        pixels.show();
        delay(DELAYVAL);
    }
    pixels.clear();
      for(int i=0; i<NUMPIXELS; i++) {
        pixels.setPixelColor(i, pixels.Color(0, 0, 0));
        pixels.show();
        delay(DELAYVAL);
    }
    }
    else if (temp<=30.0 && temp>=24.0){
      pixels.clear();
      for(int i=0; i<NUMPIXELS; i++) {
        pixels.setPixelColor(i, pixels.Color(255, 128, 0));
        pixels.show();
        delay(DELAYVAL);
    }
    }
    else if (temp<=24.0 && temp>=20.0){
      pixels.clear();
      for(int i=0; i<NUMPIXELS; i++) {
        pixels.setPixelColor(i, pixels.Color(0, 0, 255));
        pixels.show();
        delay(DELAYVAL);
    }
    }
    else{
      pixels.clear();
      for(int i=0; i<NUMPIXELS; i++) {
        pixels.setPixelColor(i, pixels.Color(255, 255, 255));
        pixels.show();
        delay(DELAYVAL);
    }
      pixels.clear();
      for(int i=0; i<NUMPIXELS; i++) {
        pixels.setPixelColor(i, pixels.Color(0, 0, 0));
        pixels.show();
        delay(DELAYVAL);
    }
    }
  }


  display.clearDisplay();
  display.setCursor(0,8);
  display.print("Lumi: "); display.print(veml.readLux()); display.println(" Lux"); 
  display.print("Temp: "); display.print(bme.readTemperature()); display.println(" oC"); 
  display.print("Humi: "); display.print(bme.readHumidity()); display.println(" %");
  display.display();
  delay(50);


}