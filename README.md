# TTGO & SCD41 based CO2 sensor

## hardware

SCD41 connected to the board using I2c bus (SDA & SCL pins wired) - see the resources folder for devices information.

## software

Arduino IDE config:
 * board manager url set to https://dl.espressif.com/dl/package_esp32_index.json
 * board type set to "ESP32 Dev Module" with:
   * PSRAM option set to disabled
   * Upload Speed set to 115200
