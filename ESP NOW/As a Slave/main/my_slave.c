#include <stdio.h>
#include "ds18b20.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#include "esp_system.h"
#include "esp_log.h"
// This example needs rtc driver to read button state
#include "driver/rtc_io.h"

#include "espnow_basic_config.h"
#define TEMP_BUS 26

static const char *TAG = "My_Slave";
DeviceAddress tempSensors[1];
// Your function to populate a my_data_t to send
void getTempAddresses(DeviceAddress *tempSensorAddresses);

void my_data_populate(my_data_t *data)
{
    ds18b20_init(TEMP_BUS);
	getTempAddresses(tempSensors);
    ds18b20_setResolution(tempSensors,2,10);
    ds18b20_requestTemperatures();
    float tempC = ds18b20_getTempF((DeviceAddress *)tempSensors[0]);
	float temp = ds18b20_getTempC((DeviceAddress *)tempSensors[0]);
    ESP_LOGI(TAG, "Populating my_data_t");
    data->sensor_value = ds18b20_get_temp();
    data->alamat_sensor = tempSensors[0][0];
    data->button_pushed = (rtc_gpio_get_level(GPIO_NUM_0) == 0);
}