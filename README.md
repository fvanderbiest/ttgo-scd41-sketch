# TTGO & SCD41 based CO2 sensor

## hardware

[SCD41](https://www.sensirion.com/en/environmental-sensors/carbon-dioxide-sensors/carbon-dioxide-sensor-scd4x/) connected to [TTGO board](http://www.lilygo.cn/claprod_view.aspx?TypeId=62&Id=1126&FId=t28:62:28) using I2c bus (SDA & SCL pins wired) - see the resources folder for devices information.

## software

Arduino IDE config:
 * board manager url set to https://dl.espressif.com/dl/package_esp32_index.json
 * board type set to "ESP32 Dev Module" with:
   * Upload Speed set to 460800
   * CPU Freq 240 MHz
   * Flash Freq 80MHz
   * Flash Size 16Mb (128Mb)
   * Partition Scheme 16 Mb Flash (3 Mb App / 9 Mb FATFS) - MANDATORY
   * PSRAM option set to disabled
