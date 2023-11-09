## I2C Echo Example

This example demonstrates how to use the I2C slave driver to communicate with the master, and echo back the received data.

## How to use the example

Bind the I2C master and slave IO pins together, the IO can be configured in `menuconfig` or in `sdkconfig.defaults`.

For `esp32s3`, the default configuration is:

| Pin | I2C master | I2C slave |
| --- | ---------- | --------- |
| SCL | 2          | 5         |
| SDA | 1          | 4         |

For `esp32s2`, the default configuration is:

| Pin | I2C master | I2C slave |
| --- | ---------- | --------- |
| SCL | 19         | 5         |
| SDA | 18         | 4         |

## Log output

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
I (575) i2c-example: Master Send Command: 02, Return: 02
I (775) i2c-example: Master Send Command: 03, Return: 03
I (975) i2c-example: Master Send Command: 04, Return: 04
I (1175) i2c-example: Master Send Command: 05, Return: 05
I (1375) i2c-example: Master Send Command: 06, Return: 06
I (1575) i2c-example: Master Send Command: 07, Return: 07
I (1775) i2c-example: Master Send Command: 08, Return: 08
I (1975) i2c-example: Master Send Command: 09, Return: 09
```