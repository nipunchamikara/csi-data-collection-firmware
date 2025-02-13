#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "esp_log.h"
#include "esp_netif.h"
#include "esp_now.h"
#include "esp_wifi.h"
#include "nvs.h"
#include "rom/ets_sys.h"

#include "app_nvs.h"
#include "constants.h"

void store_device_id(const uint8_t device_id) {
  nvs_handle_t nvs_handle;
  ESP_ERROR_CHECK(nvs_open("storage", NVS_READWRITE, &nvs_handle));
  ESP_ERROR_CHECK(nvs_set_u8(nvs_handle, "device_id", device_id));
  ESP_ERROR_CHECK(nvs_commit(nvs_handle));
  nvs_close(nvs_handle);
  ESP_LOGI(TAG, "Device ID stored: %d", device_id);
}

uint8_t get_device_id() {
  nvs_handle_t nvs_handle;
  ESP_ERROR_CHECK(nvs_open("storage", NVS_READWRITE, &nvs_handle));
  uint8_t device_id;
  const esp_err_t ret = nvs_get_u8(nvs_handle, "device_id", &device_id);
  if (ret == ESP_ERR_NVS_NOT_FOUND) {
    ESP_LOGI(TAG, "Device ID not found, setting default ID: %d", DEFAULT_DEVICE_ID);
    store_device_id(DEFAULT_DEVICE_ID);
  } else {
    ESP_LOGI(TAG, "Device ID found: %d", device_id);
  }
  nvs_close(nvs_handle);
  return device_id;
}
