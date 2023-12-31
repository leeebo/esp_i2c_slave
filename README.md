## ESP I2C Slave Driver

[![Build examples](https://github.com/leeebo/esp_i2c_slave/actions/workflows/build_examples.yml/badge.svg)](https://github.com/leeebo/esp_i2c_slave/actions/workflows/build_examples.yml)

[esp_i2c_slave](./components/esp_i2c_slave) is a ESP-IDF component, which modified from [arduino-esp32/esp32-hal-i2c-slave.h](https://github.com/espressif/arduino-esp32/blob/master/cores/esp32/esp32-hal-i2c-slave.h), which supports I2C slave SCL stretching.

## Tested Target

* ESP32-S2
* ESP32-S3

## SCL Stretch Test Result

1. Stretch SCL when Master read
2. Read from RX FIFO
3. Process -> Write to Slave TX FIFO
4. Slave Release SCL Stretch
5. Master read back

![](./components/esp_i2c_slave/stretch_test_result.png)

![](./components/esp_i2c_slave/stretch_test_result2.png)

## Special SCL Stretch Case

If the slave doesn't know how many bytes the master will read, the slave can try to write some bytes first, if the master reads more bytes than the slave writes, the SCL Stretch will be triggered again, and the `i2c_slave_request_cb` will be called again, then the slave can write more bytes.

![](./components/esp_i2c_slave/stretch_special_case.png)


## Example Log

```
I (291) sleep: Configure to isolate all GPIO pins in sleep state
I (297) sleep: Enable automatic switching of GPIO sleep configuration
I (304) cpu_start: Starting scheduler on PRO CPU.
I (0) cpu_start: Starting scheduler on APP CPU.
I (325) i2c_slave: Initialising I2C Slave: sda=4 scl=5 freq=100000, addr=0x28
I (325) gpio: GPIO[4]| InputEn: 1| OutputEn: 0| OpenDrain: 1| Pullup: 1| Pulldown: 0| Intr:0 
I (335) gpio: GPIO[5]| InputEn: 1| OutputEn: 0| OpenDrain: 1| Pullup: 1| Pulldown: 0| Intr:0 
I (345) gpio: GPIO[5]| InputEn: 1| OutputEn: 1| OpenDrain: 1| Pullup: 1| Pulldown: 0| Intr:0 
I (355) gpio: GPIO[4]| InputEn: 1| OutputEn: 1| OpenDrain: 1| Pullup: 1| Pulldown: 0| Intr:0 
I (365) i2c-example: Master Send Command: 01, Return: 01
I (575) i2c-example: Master Send Data: 01 02 03 04, Return: 01 02 03 04
I (575) i2c-example: Master Send Command: 02, Return: 02
I (775) i2c-example: Master Send Data: 02 03 04 05, Return: 02 03 04 05
I (775) i2c-example: Master Send Command: 03, Return: 03
I (975) i2c-example: Master Send Data: 03 04 05 06, Return: 03 04 05 06
I (975) i2c-example: Master Send Command: 04, Return: 04
I (1175) i2c-example: Master Send Data: 04 05 06 07, Return: 04 05 06 07
I (1175) i2c-example: Master Send Command: 05, Return: 05
I (1375) i2c-example: Master Send Data: 05 06 07 08, Return: 05 06 07 08
I (1375) i2c-example: Master Send Command: 06, Return: 06
I (1575) i2c-example: Master Send Data: 06 07 08 09, Return: 06 07 08 09
I (1575) i2c-example: Master Send Command: 07, Return: 07
I (1775) i2c-example: Master Send Data: 07 08 09 0a, Return: 07 08 09 0a
I (1775) i2c-example: Master Send Command: 08, Return: 08
I (1975) i2c-example: Master Send Data: 08 09 0a 0b, Return: 08 09 0a 0b
```
