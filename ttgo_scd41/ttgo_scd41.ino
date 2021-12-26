#include <Arduino.h>
#include <SensirionI2CScd4x.h>

#include "Wire.h"
#include "TFT_eSPI.h"
#include "SPI.h"
#include "TaskScheduler.h"
#include "Button2.h"
// splash screen:
#include "bmp.h"
// fonts:
#include "Purisa_48.h"
#include "Purisa_24.h"
#include "Purisa_12.h"
/*
Font files in the .vlw format can be created using Processing out of any font which is installed in your system.

The process is simple: In the processing IDE,
- go to Tools / Create Font
- pick a font and a size and accept.

If you click then Sketch / Show sketch folder you'll be able to see the font in the data folder. 
Now you can convert it to an array using https://tomeko.net/online_tools/file_to_hex.php?lang=en
Fonts as arrays are loaded as tabs.
*/

#define BUTTON_1 35
#define BUTTON_2 0
#define ADC_PIN 34
Button2 btn1(BUTTON_1);
Button2 btn2(BUTTON_2);

void measure();
Task mainTask(5000, TASK_FOREVER, &measure);

SensirionI2CScd4x scd;

TFT_eSPI tft = TFT_eSPI(135, 240); // pins defined in User_Setup.h
Scheduler runner;
int co2;
int calibrationInProgress = 0;

// For long time delay, it is recommended to use shallow sleep, which can effectively reduce the current consumption
void espDelay(int ms)
{
    esp_sleep_enable_timer_wakeup(ms * 1000);
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);
    esp_light_sleep_start();
}

void setup()
{
  Serial.begin(115200);
  Wire.begin();
  btn1.setPressedHandler([](Button2 & b) {
    showVoltage();
  });
  btn2.setLongClickTime(2000);
  btn2.setPressedHandler([](Button2 & b) {
    // stop measurements...
    mainTask.disable();
    tft.fillScreen(TFT_WHITE);
    tft.setTextColor(TFT_BLACK, TFT_WHITE);
    tft.loadFont(Purisa_24);
    tft.drawString("Appui 2s", tft.width()/2, tft.height()/2-50);
    tft.drawString("pour", tft.width()/2, tft.height()/2);
    tft.drawString("calibration", tft.width()/2, tft.height()/2+50);
    tft.unloadFont();
  });
  btn2.setReleasedHandler([](Button2 & b) {
    if (calibrationInProgress == 0) {
      mainTask.enable();
    }
  });
  btn2.setLongClickHandler([](Button2 & b) {
    calibrationInProgress = 1;
    tft.fillScreen(TFT_WHITE);
    tft.setTextColor(TFT_BLACK, TFT_WHITE);
    tft.loadFont(Purisa_24);
    tft.drawString("Calibration", tft.width()/2, tft.height()/2-50);
    tft.drawString("en cours", tft.width()/2, tft.height()/2);
    tft.drawString("(plein air)", tft.width()/2, tft.height()/2+50);
    tft.unloadFont();
    performCalibration();
    mainTask.enable();
    calibrationInProgress = 0;
  });
  scd.begin(Wire);
  // stop potentially previously started measurement
  scd.stopPeriodicMeasurement();
  scd.setAutomaticSelfCalibration(false);
  
  tft.init();
  tft.setRotation(0);
  
  tft.fillScreen(TFT_WHITE);

  // splash screen:
  tft.setSwapBytes(true);
  tft.pushImage(0, 0,  135, 240, c2c);
  espDelay(6000);


  // Start Measurement
  scd.startPeriodicMeasurement();
  Serial.println("Waiting for first measurement... (5 sec)");

  runner.init();
  runner.addTask(mainTask);
  mainTask.enable();
}

void performCalibration() {
  // should check 3 minutes runtime before
  scd.stopPeriodicMeasurement();
  uint16_t correction;
  scd.performForcedRecalibration(415, correction);
  scd.startPeriodicMeasurement();
}

void showVoltage()
{
    static uint64_t timeStamp = 0;
    int vref = 1100;
    if (millis() - timeStamp > 1000) {
        timeStamp = millis();
        uint16_t v = analogRead(ADC_PIN);
        float voltage = ((float)v / 4095.0) * 2.0 * 3.3 * (vref / 1000.0);
        tft.loadFont(Purisa_24);
        tft.fillScreen(TFT_WHITE);
        tft.setTextColor(TFT_BLACK, TFT_WHITE);
        tft.setTextDatum(MC_DATUM);
        if (voltage > 4.5) {
          tft.drawString("USB",  tft.width() / 2, tft.height() / 2 - 25);
          tft.drawString(String(voltage) + " V",  tft.width() / 2, tft.height() / 2 + 25);
        } else {
          tft.drawString("Battery",  tft.width() / 2, tft.height() / 2 - 25);
          tft.drawString(String((int)((voltage-3)/1.26*100)) + " %",  tft.width() / 2, tft.height() / 2);
          tft.drawString(String(voltage) + " V",  tft.width() / 2, tft.height() / 2 + 25);
        }
        tft.unloadFont();
    }
}


void measure() {

  uint16_t error;
  char errorMessage[256];
  // Read Measurement
  uint16_t co2;
  float temperature;
  float humidity;
  error = scd.readMeasurement(co2, temperature, humidity);
  if (error) {
      Serial.print("Error trying to execute readMeasurement(): ");
      errorToString(error, errorMessage, 256);
      Serial.println(errorMessage);
  } else if (co2 == 0) {
      Serial.println("Invalid sample detected, skipping.");
  } else {
    Serial.print("co2(ppm):");
    Serial.print(co2);
    Serial.print(" temp(C):");
    Serial.print(temperature, 1);
    Serial.print(" humidity(%):");
    Serial.print(humidity, 1);
    Serial.println();
  
    if (co2 > 1000) {
      tft.setTextColor(TFT_BLACK, TFT_RED);
      tft.fillScreen(TFT_RED);
    } else if (co2 < 800) { 
      tft.setTextColor(TFT_BLACK, TFT_GREEN);
      tft.fillScreen(TFT_GREEN);
    } else {
      tft.setTextColor(TFT_BLACK, TFT_ORANGE);
      tft.fillScreen(TFT_ORANGE);
    }
    tft.setTextDatum(MC_DATUM);
  
    tft.loadFont(Purisa_48);
    tft.drawString("CO2", tft.width() / 2, 40);
    tft.drawString(String(co2), tft.width()/2, tft.height()/2);
    tft.unloadFont(); // Unload the font to recover used RAM
    
    tft.loadFont(Purisa_24);
    tft.drawString("ppm", tft.width() - 55, tft.height()/2 + 40);
    tft.unloadFont();
  
  
    tft.loadFont(Purisa_12);
    if (co2 > 1000) {
      tft.drawString("Quittez la pièce", tft.width()- 70, tft.height() - 30);
    } else if (co2 < 800) { 
      tft.drawString("Tout va bien", tft.width()- 70, tft.height() - 30);
    } else {
      tft.drawString("Ouvrez les fenêtres", tft.width()- 70, tft.height() - 30);
    }
    tft.unloadFont();
  }
}

void loop()
{
  runner.execute();
  btn1.loop();
  btn2.loop();
}
