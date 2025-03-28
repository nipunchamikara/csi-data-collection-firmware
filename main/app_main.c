#include <rom/ets_sys.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "esp_log.h"
#include "esp_netif.h"
#include "nvs.h"
#include "nvs_flash.h"

#include "app_init.h"
#include "app_nvs.h"
#include "constants.h"
#include "csi_data.h"

/**
 * Returns the number of devices between the current and last device IDs.
 * @param device_id The current device ID.
 * @param previous_packet_device_id The previous packet sender device ID.
 * @return The number of devices between the current and last device IDs.
 */
uint8_t turns_away(const uint8_t device_id, const uint8_t previous_packet_device_id) {
  // device 2, last device 0, 2 - 0 = 2
  if (device_id > previous_packet_device_id) {
    return device_id - previous_packet_device_id;
  }
  // device 0, last device 2, 3 - 2 + 0 = 1
  return CONFIG_TOTAL_DEVICES - previous_packet_device_id + device_id;
}

void app_main() {
  // Initialize NVS
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

#if CONFIG_IS_SET_DEVICE_ID
  store_device_id(CONFIG_DEVICE_ID);
#endif

  payload.device_id = get_device_id();
  prev_packet_device_id = (payload.device_id + CONFIG_TOTAL_DEVICES - 1) % CONFIG_TOTAL_DEVICES;

  // Initialize the payload length and index
  payload.csi_data_arr_len = 0;
  payload_index = 0;
  last_time_index = UINT16_MAX;

  // Initialize Wi-Fi, ESP-NOW, and CSI
  wifi_init();
  now_init();
  csi_init();

  // If the device ID is 0, start sending CSI data
  if (payload.device_id == 0) {
    ets_printf("type,time_index,device_id,recv_device_id,mac,rssi,rate,sig_mode,mcs,cwb,"
               "smoothing,not_sounding,aggregation,stbc,fec_coding,sgi,noise_floor,ampdu_cnt,"
               "channel,secondary_channel,timestamp,ant,sig_len,rx_state,len,csi_data\n");
    last_time_index = 0;
    send_csi_data(0);
  }

  // Infinite loop to send CSI data
  // ReSharper disable once CppDFAEndlessLoop
  while (1) {
    usleep(MICROSECONDS_IN_SECOND / CONFIG_SEND_FREQUENCY);

    // Only start timeout if CSI data has been received from device 0
    if (last_time_index == UINT16_MAX) {
      continue;
    }

    timeout_count++;

    // Determine the multiplier based on the distance between the current and last device IDs
    // This multiplier is used to increase the timeout count for devices further away
    const int multiplier = turns_away(payload.device_id, prev_packet_device_id);

    // If the timeout count exceeds the timeout threshold, send CSI data
    if (timeout_count >= CONFIG_CYCLE_TIMEOUT * multiplier * multiplier) {
      ESP_LOGW(TAG, "ESP-NOW Timeout %d", timeout_count);
      send_csi_data(last_time_index++);
      timeout_count = 0;
    }
  }
}
