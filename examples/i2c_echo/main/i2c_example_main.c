/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdio.h>
#include "esp_log.h"
#include "driver/i2c.h"
#include "esp32-hal-i2c-slave.h"
#include "sdkconfig.h"

static const char *TAG = "i2c-example";

#define _I2C_NUMBER(num) I2C_NUM_##num
#define I2C_NUMBER(num) _I2C_NUMBER(num)

#define DATA_LENGTH 512                  /*!< Data buffer length of test buffer */
#define RW_TEST_LENGTH 128               /*!< Data length for r/w test, [0,DATA_LENGTH] */
#define DELAY_TIME_BETWEEN_ITEMS_MS 1000 /*!< delay time between different test items */

#define I2C_SLAVE_SCL_IO CONFIG_I2C_SLAVE_SCL               /*!< gpio number for i2c slave clock */
#define I2C_SLAVE_SDA_IO CONFIG_I2C_SLAVE_SDA               /*!< gpio number for i2c slave data */
#define I2C_SLAVE_NUM I2C_NUMBER(CONFIG_I2C_SLAVE_PORT_NUM) /*!< I2C port number for slave dev */
#define I2C_SLAVE_TX_BUF_LEN (2 * DATA_LENGTH)              /*!< I2C slave tx buffer size */
#define I2C_SLAVE_RX_BUF_LEN (2 * DATA_LENGTH)              /*!< I2C slave rx buffer size */
#define ESP_SLAVE_ADDR CONFIG_I2C_SLAVE_ADDRESS             /*!< ESP32 slave address, you can set any 7bit value */

#define I2C_MASTER_SCL_IO CONFIG_I2C_MASTER_SCL               /*!< gpio number for I2C master clock */
#define I2C_MASTER_SDA_IO CONFIG_I2C_MASTER_SDA               /*!< gpio number for I2C master data  */
#define I2C_MASTER_NUM I2C_NUMBER(CONFIG_I2C_MASTER_PORT_NUM) /*!< I2C port number for master dev */
#define I2C_MASTER_FREQ_HZ CONFIG_I2C_MASTER_FREQUENCY        /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE 0                           /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE 0                           /*!< I2C master doesn't need buffer */

SemaphoreHandle_t print_mux = NULL;

static esp_err_t i2c_master_read_slave(i2c_port_t i2c_num, uint8_t command, uint8_t *data_rd, size_t size)
{
    if (size == 0) {
        return ESP_OK;
    }
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);                                                      //send start bit
    i2c_master_write_byte(cmd, (ESP_SLAVE_ADDR << 1) | I2C_MASTER_WRITE, true); //send the bms address and write bit
    i2c_master_write_byte(cmd, command, true);                                  //send the command to read
    i2c_master_start(cmd);                                                      //send restart
    i2c_master_write_byte(cmd, (ESP_SLAVE_ADDR << 1) | I2C_MASTER_READ, true);  //send the bms address and read bit
    if(size > 1) {
        i2c_master_read(cmd, data_rd, size - 1, I2C_MASTER_ACK);                //if more than one byte is to be read request one less than the number of bytes required
    }
    i2c_master_read(cmd, &data_rd[size - 1], 1, I2C_MASTER_NACK);               //request the last byte and send a NACK
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(i2c_num, cmd, portMAX_DELAY);
    i2c_cmd_link_delete(cmd);
    return ret;
}


/**
 * @brief i2c master initialization
 */
static esp_err_t i2c_master_init(void)
{
    int i2c_master_port = I2C_MASTER_NUM;
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
        // .clk_flags = 0,          /*!< Optional, you can use I2C_SCLK_SRC_FLAG_* flags to choose i2c source clock here. */
    };
    esp_err_t err = i2c_param_config(i2c_master_port, &conf);
    if (err != ESP_OK) {
        return err;
    }
    return i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}

static void i2c_slave_request_cb(uint8_t num, uint8_t *cmd, uint8_t cmd_len, void * arg)
{
    // please not block or print in this callback function
    // because the SCL Stretching will be released if time is too long
    i2cSlaveWrite(I2C_SLAVE_NUM, cmd, cmd_len, 0);
}

static void i2c_slave_receive_cb(uint8_t num, uint8_t * data, size_t len, bool stop, void * arg)
{
    ESP_LOGI(TAG, "rcv_len: %d", len);
    if (len > 0) {
        ESP_LOGI(TAG, "rcv: %02x ...", data[0]);
    }
}

/**
 * @brief i2c slave initialization
 */
static esp_err_t i2c_slave_init(void)
{
    i2cSlaveAttachCallbacks(I2C_SLAVE_NUM, i2c_slave_request_cb, i2c_slave_receive_cb, NULL);
    return i2cSlaveInit(I2C_SLAVE_NUM, I2C_SLAVE_SDA_IO, I2C_SLAVE_SCL_IO, ESP_SLAVE_ADDR, I2C_MASTER_FREQ_HZ, I2C_SLAVE_RX_BUF_LEN, I2C_SLAVE_TX_BUF_LEN);
}

static void i2c_master_task(void *arg)
{
    uint8_t data_rd[1] = {0};
    uint8_t command = 0x00;
    while (1) {
        ++command;
        i2c_master_read_slave(I2C_MASTER_NUM, command, data_rd, 1);
        ESP_LOGI(TAG, "Master Send Command: %02x, Return: %02x", command, data_rd[0]);
        vTaskDelay(200 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

void app_main(void)
{
    ESP_ERROR_CHECK(i2c_slave_init());
    ESP_ERROR_CHECK(i2c_master_init());
    xTaskCreate(i2c_master_task, "i2c_master_task", 1024 * 4, NULL, 1, NULL);
}
