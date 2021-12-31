#include <SensirionI2CScd4x.h>
#include "Wire.h"
#include "TFT_eSPI.h"
#include "TaskScheduler.h"
#include "Button2.h"
#include "phyphoxBle.h"
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
boolean BLE = false;
int calibrationInProgress = 0;

// For long time delay, it is recommended to use shallow sleep, which can effectively reduce the current consumption
void espDelay(int ms)
{
    esp_sleep_enable_timer_wakeup(ms * 1000);
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);
    esp_light_sleep_start();
}

uint8_t CO2Monitor[2133] = {80, 75, 3, 4, 20, 0, 8, 0, 8, 0, 78, 116, 126, 81, 0, 0, 0, 0, 0, 0, 0, 0, 207, 43, 0, 0, 17, 0, 32, 0, 99, 111, 50, 115, 101, 110, 115, 111, 114, 46, 112, 104, 121, 112, 104, 111, 120, 85, 84, 13, 0, 7, 228, 244, 196, 95, 16, 245, 196, 95, 228, 244, 196, 95, 117, 120, 11, 0, 1, 4, 232, 3, 0, 0, 4, 232, 3, 0, 0, 213, 90, 235, 110, 219, 54, 20, 254, 157, 62, 5, 39, 160, 88, 130, 205, 177, 101, 59, 77, 81, 216, 6, 178, 110, 93, 7, 20, 8, 176, 116, 88, 135, 162, 40, 104, 137, 138, 8, 200, 162, 38, 82, 78, 92, 244, 87, 95, 104, 123, 134, 62, 202, 158, 100, 135, 20, 117, 191, 152, 74, 157, 116, 75, 127, 84, 18, 207, 33, 191, 115, 63, 36, 189, 136, 252, 93, 228, 179, 91, 180, 37, 49, 167, 44, 92, 90, 246, 169, 61, 177, 86, 143, 16, 252, 45, 4, 21, 1, 89, 61, 191, 252, 231, 211, 167, 209, 21, 9, 57, 139, 23, 227, 244, 91, 58, 238, 96, 65, 174, 89, 188, 171, 145, 228, 159, 53, 21, 11, 88, 188, 154, 206, 46, 46, 230, 231, 48, 168, 222, 210, 17, 151, 112, 39, 166, 145, 128, 133, 87, 175, 125, 202, 17, 185, 141, 72, 76, 55, 36, 20, 104, 75, 121, 130, 3, 250, 129, 112, 36, 124, 130, 184, 154, 27, 185, 88, 96, 196, 60, 245, 41, 131, 254, 252, 114, 170, 135, 79, 23, 227, 242, 148, 233, 34, 212, 97, 33, 242, 88, 188, 193, 98, 105, 113, 17, 211, 240, 218, 74, 1, 47, 198, 114, 44, 195, 2, 51, 143, 224, 85, 96, 26, 130, 46, 210, 175, 26, 191, 254, 136, 56, 192, 89, 90, 160, 30, 135, 189, 167, 161, 148, 69, 143, 244, 83, 11, 178, 137, 134, 208, 251, 201, 102, 208, 244, 160, 176, 58, 125, 55, 131, 173, 208, 135, 236, 198, 108, 126, 91, 195, 31, 194, 32, 241, 215, 233, 247, 169, 83, 46, 50, 76, 161, 131, 56, 36, 164, 97, 75, 72, 165, 54, 56, 246, 72, 49, 72, 128, 33, 216, 7, 193, 54, 71, 44, 169, 93, 234, 121, 131, 166, 119, 88, 28, 19, 71, 12, 178, 174, 75, 121, 20, 224, 221, 48, 3, 15, 101, 146, 54, 30, 188, 144, 52, 115, 27, 83, 159, 131, 35, 26, 82, 145, 179, 127, 32, 177, 161, 225, 33, 52, 162, 168, 97, 161, 253, 228, 141, 21, 204, 192, 133, 151, 129, 91, 97, 59, 58, 234, 165, 7, 171, 18, 186, 37, 238, 143, 144, 9, 239, 176, 92, 76, 32, 183, 146, 45, 249, 33, 17, 130, 25, 102, 175, 51, 107, 181, 222, 9, 114, 17, 199, 45, 54, 131, 100, 222, 150, 147, 23, 52, 140, 18, 81, 154, 116, 29, 36, 68, 48, 38, 124, 68, 221, 165, 5, 213, 192, 66, 33, 222, 16, 253, 136, 54, 204, 133, 231, 144, 9, 234, 81, 168, 76, 80, 24, 44, 20, 67, 133, 82, 232, 121, 178, 150, 245, 98, 77, 46, 195, 43, 129, 99, 144, 196, 195, 1, 39, 86, 49, 189, 90, 130, 37, 2, 214, 68, 142, 143, 227, 165, 229, 184, 174, 103, 79, 38, 211, 209, 108, 226, 157, 143, 230, 79, 206, 237, 209, 211, 245, 124, 54, 58, 35, 243, 201, 26, 159, 205, 206, 236, 57, 182, 16, 192, 206, 107, 170, 23, 48, 44, 102, 211, 87, 84, 64, 237, 252, 41, 116, 41, 14, 243, 26, 146, 78, 125, 255, 235, 65, 213, 244, 56, 1, 1, 231, 165, 130, 244, 224, 139, 63, 45, 170, 91, 182, 246, 17, 184, 229, 125, 46, 105, 79, 75, 37, 178, 46, 240, 98, 156, 123, 79, 230, 116, 37, 247, 90, 52, 200, 155, 190, 86, 83, 158, 226, 174, 10, 50, 51, 23, 36, 15, 134, 74, 92, 212, 29, 190, 137, 185, 140, 115, 129, 67, 28, 236, 56, 116, 83, 60, 32, 36, 130, 216, 60, 133, 48, 96, 225, 111, 156, 196, 191, 200, 153, 10, 31, 47, 166, 252, 102, 52, 122, 111, 242, 55, 26, 173, 42, 76, 31, 11, 209, 175, 136, 64, 82, 207, 72, 166, 171, 178, 78, 62, 214, 153, 62, 255, 117, 168, 127, 114, 230, 202, 212, 136, 122, 40, 75, 153, 8, 84, 48, 249, 30, 221, 16, 228, 227, 45, 65, 144, 1, 208, 14, 48, 186, 68, 144, 120, 3, 9, 197, 77, 145, 66, 119, 40, 187, 201, 91, 132, 111, 41, 63, 69, 175, 136, 248, 150, 163, 132, 19, 245, 85, 206, 197, 5, 222, 68, 178, 233, 196, 225, 14, 133, 228, 38, 109, 66, 35, 70, 161, 75, 197, 161, 139, 160, 177, 116, 192, 31, 64, 122, 166, 120, 180, 51, 235, 46, 85, 206, 16, 203, 36, 148, 4, 228, 20, 85, 1, 171, 177, 214, 232, 203, 11, 69, 211, 97, 53, 83, 241, 5, 68, 14, 8, 231, 75, 75, 196, 9, 177, 16, 249, 19, 250, 102, 253, 210, 238, 156, 1, 145, 222, 169, 157, 160, 40, 48, 53, 55, 43, 113, 136, 93, 4, 233, 114, 139, 3, 57, 229, 164, 143, 178, 58, 119, 38, 68, 27, 125, 22, 242, 45, 12, 41, 156, 166, 228, 212, 251, 127, 136,157, 103, 27, 99, 185, 205, 165, 254, 242, 64, 253, 85, 215, 105, 180, 86, 133, 186, 132, 237, 193, 2, 21, 71, 17, 9, 221, 253, 58, 183, 15, 99, 157, 175, 69, 169, 77, 141, 193, 71, 225, 177, 146, 210, 139, 226, 183, 24, 103, 218, 168, 184, 246, 64, 119, 174, 55, 95, 135, 19, 17, 90, 5, 177, 179, 208, 184, 91, 182, 152, 240, 36, 16, 86, 134, 40, 133, 220, 34, 107, 206, 91, 115, 105, 83, 111, 48, 212, 114, 93, 21, 253, 170, 254, 242, 120, 186, 80, 115, 22, 133, 161, 248, 251, 47, 196, 83, 213, 77, 116, 207, 105, 156, 151, 178, 147, 130, 126, 29, 26, 47, 159, 247, 157, 230, 137, 49, 63, 123, 56, 16, 132, 172, 251, 52, 70, 144, 31, 102, 124, 41, 128, 52, 46, 134, 91, 224, 80, 43, 223, 77, 249, 135, 90, 253, 46, 122, 63, 152, 228, 119, 170, 199, 247, 155, 57, 174, 88, 156, 118, 143, 46, 9, 160, 33, 69, 110, 18, 5, 114, 123, 74, 184, 28, 125, 176, 204, 193, 1, 134, 97, 71, 99, 222, 255, 56, 189, 93, 85, 155, 143, 153, 82, 43, 167, 216, 111, 197, 146, 217, 171, 62, 188, 151, 39, 63, 137, 52, 230, 40, 157, 69, 26, 243, 20, 167, 145, 205, 218, 152, 90, 100, 120, 117, 156, 246, 41, 166, 116, 210, 215, 92, 177, 233, 218, 146, 146, 196, 36, 20, 20, 28, 210, 180, 225, 237, 182, 100, 103, 136, 117, 33, 170, 173, 95, 140, 196, 56, 188, 38, 30, 13, 68, 99, 219, 212, 5, 43, 93, 194, 172, 29, 82, 61, 4, 236, 12, 247, 52, 158, 141, 18, 57, 204, 135, 75, 14, 51, 192, 237, 135, 46, 98, 98, 145, 30, 27, 148, 169, 156, 150, 109, 73, 101, 150, 125, 206, 95, 205, 229, 93, 64, 218, 28, 161, 98, 239, 3, 54, 109, 63, 147, 144, 200, 51, 192, 244, 192, 66, 110, 252, 115, 72, 15, 150, 122, 229, 149, 84, 18, 96, 164, 255, 95, 90, 199, 111, 237, 247, 239, 70, 111, 167, 239, 190, 123, 59, 123, 119, 50, 126, 50, 49, 221, 199, 14, 115, 139, 125, 187, 222, 238, 29, 249, 30, 79, 202, 47, 6, 154, 118, 212, 50, 30, 200, 134, 47, 148, 67, 72, 197, 33, 14, 213, 211, 17, 196, 77, 13, 169, 220, 229, 33, 109, 56, 44, 31, 125, 133, 186, 216, 229, 4, 185, 173, 12, 153, 138, 212, 152, 222, 42, 244, 184, 66, 249, 186, 103, 95, 218, 48, 163, 172, 92, 236, 236, 75, 35, 221, 148, 109, 233, 4, 170, 159, 62, 45, 213, 135, 167, 91, 74, 110, 202, 151, 191, 242, 29, 5, 120, 77, 96, 51, 46, 47, 70, 234, 49, 121, 29, 227, 200, 207, 8, 212, 173, 178, 133, 2, 26, 146, 223, 169, 43, 252, 165, 53, 149, 231, 187, 1, 3, 61, 166, 247, 223, 86, 74, 250, 6, 58, 1, 192, 170, 223, 254, 200, 57, 147, 144, 138, 55, 169, 162, 213, 51, 140, 68, 209, 166, 182, 102, 201, 70, 50, 115, 45, 173, 219, 250, 61, 86, 139, 117, 26, 76, 187, 234, 197, 92, 155, 65, 199, 74, 184, 218, 71, 85, 42, 107, 2, 231, 151, 65, 17, 56, 21, 77, 143, 178, 65, 114, 238, 80, 89, 195, 61, 234, 228, 110, 148, 168, 219, 34, 41, 19, 234, 194, 151, 111, 58, 91, 33, 169, 229, 107, 31, 57, 137, 48, 100, 115, 72, 7, 62, 161, 215, 190, 80, 80, 234, 7, 22, 21, 67, 189, 6, 223, 147, 5, 32, 137, 73, 110, 32, 207, 115, 92, 27, 119, 25, 72, 148, 57, 90, 204, 244, 249, 239, 231, 247, 101, 166, 106, 160, 220, 205, 80, 21, 129, 135, 154, 171, 79, 182, 210, 14, 253, 30, 237, 245, 50, 217, 80, 151, 202, 115, 168, 44, 154, 38, 231, 179, 23, 23, 93, 198, 242, 115, 242, 22, 75, 61, 190, 47, 59, 85, 210, 212, 221, 204, 84, 200, 57, 212, 70, 221, 82, 21, 39, 24, 38, 6, 130, 15, 144, 241, 58, 18, 224, 75, 202, 193, 106, 187, 122, 14, 212, 7, 201, 154, 40, 63, 95, 102, 129, 171, 78, 196, 186, 245, 109, 112, 216, 171, 168, 117, 38, 239, 58, 217, 51, 153, 125, 100, 79, 219, 207, 15, 203, 11, 164, 181, 173, 116, 49, 121, 116, 87, 160, 229, 155, 244, 242, 124, 53, 229, 167, 154, 51, 12, 15, 9, 167, 154, 125, 147, 88, 110, 146, 16, 23, 16, 216, 252, 153, 85, 134, 92, 195, 160, 1, 171, 241, 13, 142, 16, 4, 132, 188, 180, 71, 27, 124, 155, 254, 86, 128, 229, 230, 66, 25, 231, 98, 12, 148, 117, 30, 91, 243, 216, 214, 170, 193, 240, 76, 183, 99, 8, 154, 218, 180, 69, 3, 152, 210, 241, 178, 121, 148, 8, 218, 227, 26, 202, 48, 202, 10, 52, 244, 88, 38, 254, 149, 207, 244, 145, 171, 23, 179, 13, 172, 5, 13, 97, 122, 107, 199, 229, 85, 126, 118, 1, 87, 250, 77, 153, 236, 24, 65, 210, 77, 74, 39, 47, 137, 235, 243, 87, 93, 153, 133, 193, 14, 170, 57, 248,242, 0, 63, 30, 234, 102, 173, 30, 209, 208, 78, 21, 152, 141, 124, 150, 196, 134, 128, 70, 79, 14, 2, 169, 15, 207, 84, 225, 225, 166, 128, 134, 199, 226, 96, 68, 243, 97, 136, 166, 243, 123, 64, 212, 229, 209, 61, 145, 172, 226, 7, 146, 125, 45, 152, 43, 77, 119, 103, 16, 87, 221, 181, 53, 124, 193, 23, 52, 189, 124, 90, 165, 126, 212, 78, 9, 70, 202, 72, 229, 227, 74, 219, 184, 157, 24, 244, 151, 17, 203, 199, 213, 188, 76, 124, 212, 12, 125, 245, 222, 90, 116, 210, 231, 172, 29, 135, 224, 173, 156, 80, 130, 78, 133, 254, 117, 79, 75, 72, 170, 31, 113, 234, 225, 215, 114, 67, 120, 12, 232, 78, 234, 229, 92, 18, 117, 243, 201, 95, 147, 30, 67, 131, 122, 82, 237, 143, 251, 153, 74, 237, 21, 58, 134, 118, 233, 164, 222, 181, 245, 179, 103, 101, 31, 29, 63, 62, 169, 245, 17, 85, 198, 197, 24, 228, 207, 244, 148, 233, 102, 49, 214, 191, 132, 93, 61, 250, 23, 80, 75, 7, 8, 81, 175, 45, 106, 129, 7, 0, 0, 207, 43, 0, 0, 80, 75, 1, 2, 20, 3, 20, 0, 8, 0, 8, 0, 78, 116, 126, 81, 81, 175, 45, 106, 129, 7, 0, 0, 207, 43, 0, 0, 17, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 180, 129, 0, 0, 0, 0, 99, 111, 50, 115, 101, 110, 115, 111, 114, 46, 112, 104, 121, 112, 104, 111, 120, 85, 84, 13, 0, 7, 228, 244, 196, 95, 16, 245, 196, 95, 228, 244, 196, 95, 117, 120, 11, 0, 1, 4, 232, 3, 0, 0, 4, 232, 3, 0, 0, 80, 75, 5, 6, 0, 0, 0, 0, 1, 0, 1, 0, 95, 0, 0, 0, 224, 7, 0, 0, 0, 0};

void setup()
{
  Serial.begin(115200);
  Wire.begin();
  float voltage = getVoltage();
  if (voltage > 4.5) {
    // start BLE server when not on battery
    PhyphoxBLE::start("SCD41-based CO2 Monitor", &CO2Monitor[0], sizeof(CO2Monitor));
    BLE = true;
  }
  btn1.setPressedHandler([](Button2 & b) {
    showVoltage();
  });
  btn2.setLongClickTime(2000);
  btn2.setPressedHandler([](Button2 & b) {
    tft.fillScreen(TFT_WHITE);
    tft.setTextColor(TFT_BLACK, TFT_WHITE);
    tft.loadFont(Purisa_24);
    // check hardware has been running for more than 3 minutes
    // as stated in the documentation
    if (millis() > 180000) {
      // stop measurements...
      mainTask.disable();
      tft.drawString("Appui 2s", tft.width()/2, tft.height()/2-50);
      tft.drawString("pour", tft.width()/2, tft.height()/2);
      tft.drawString("calibration", tft.width()/2, tft.height()/2+50);
    } else {
      tft.drawString("Calibration", tft.width()/2, tft.height()/2-50);
      tft.drawString("possible", tft.width()/2, tft.height()/2);
      tft.drawString("dans " + String(int((180000-millis())/1000)) + " s", tft.width()/2, tft.height()/2+50);
    }
    tft.unloadFont();
  });
  btn2.setReleasedHandler([](Button2 & b) {
    if (calibrationInProgress == 0) {
      mainTask.enable();
    }
  });
  btn2.setLongClickHandler([](Button2 & b) {
    if (millis() < 180000) {
      return;
    }
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
  scd.setTemperatureOffset(6.9);
  
  tft.init();
  tft.setRotation(0);
  
  tft.fillScreen(TFT_WHITE);

  // splash screen:
  tft.setSwapBytes(true);
  tft.pushImage(0, 0,  135, 240, c2c);
  espDelay(6000);


  // Start Measurement
  scd.startPeriodicMeasurement();

  runner.init();
  runner.addTask(mainTask);
  mainTask.enable();
}

void performCalibration() {
  scd.stopPeriodicMeasurement();
  uint16_t correction;
  scd.performForcedRecalibration(415, correction);
  scd.startPeriodicMeasurement();
}

float getVoltage() {
  uint16_t v = analogRead(ADC_PIN);
  int vref = 1100;
  float voltage = ((float)v / 4095.0) * 2.0 * 3.3 * (vref / 1000.0);
  return voltage;
}

void showVoltage()
{
    static uint64_t timeStamp = 0;
    if (millis() - timeStamp > 1000) {
        timeStamp = millis();
        //uint16_t v = analogRead(ADC_PIN);
        float voltage = getVoltage(); //((float)v / 4095.0) * 2.0 * 3.3 * (vref / 1000.0);
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
  float c, t;
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

    c=float(co2);
    t = millis()/1000;
    if (BLE) {
      PhyphoxBLE::write(c, temperature, humidity, t);
    }
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
      if (co2 < 600) {
        tft.drawString("Masque non requis", tft.width()- 70, tft.height() - 30);
      } else {
        tft.drawString("Masque requis", tft.width()- 70, tft.height() - 30);
      }
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
