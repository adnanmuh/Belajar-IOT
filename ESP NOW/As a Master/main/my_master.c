#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#include "esp_log.h"
#include "espnow_basic_config.h"
static const char *TAG = "My_Master";
float sensor_value;
int alamat_sensor;

// Your task to handle received my_data_t
void my_data_receive(const uint8_t *sender_mac_addr, const my_data_t *data )
{
    ESP_LOGI(TAG, "Data from "MACSTR": Sensor Value - %0.1fF, Adrres Value - 0x%02x, Button - %s",
                MAC2STR(sender_mac_addr), 
                data->sensor_value, 
                data->alamat_sensor, 
                data->button_pushed ? "Pushed" : "Released");
    sensor_value = data->sensor_value;
    alamat_sensor=data->alamat_sensor;
    printf("Temperatures: %0.1fF\n", sensor_value);
    printf("Address : 0x%02x  \n", alamat_sensor);
}

