#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_now.h"
#include "esp_wifi.h"

#include "app_init.h"
#include "constants.h"
#include "csi_data.h"

void wifi_init() {
  ESP_ERROR_CHECK(esp_event_loop_create_default());
  ESP_ERROR_CHECK(esp_netif_init());
  const wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
  ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
  ESP_ERROR_CHECK(esp_wifi_set_bandwidth(ESP_IF_WIFI_STA, WIFI_BW_HT40));
  ESP_ERROR_CHECK(esp_wifi_start());
  ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_NONE));
  ESP_ERROR_CHECK(esp_wifi_set_channel(CONFIG_LESS_INTERFERENCE_CHANNEL, WIFI_SECOND_CHAN_BELOW));
  ESP_ERROR_CHECK(esp_wifi_set_promiscuous(true));
  ESP_LOGI(TAG, "Wi-Fi initialized");
}

void now_init() {
  ESP_ERROR_CHECK(esp_now_init());
  ESP_ERROR_CHECK(esp_now_set_pmk((uint8_t *) "pmk1234567890123"));

  // Sends data to all peers
  const esp_now_peer_info_t peer = {
      .channel = CONFIG_LESS_INTERFERENCE_CHANNEL,
      .ifidx = WIFI_IF_STA,
      .encrypt = false,
      .peer_addr = BROADCAST_ADDR,
  };
  ESP_ERROR_CHECK(esp_now_add_peer(&peer));

  esp_now_rate_config_t rate_config = {
      .phymode = WIFI_PHY_MODE_HT40,
      .rate = WIFI_PHY_RATE_MCS0_SGI,
  };
  const uint8_t peer_addr[] = BROADCAST_ADDR;
  ESP_ERROR_CHECK(esp_now_set_peer_rate_config(peer_addr, &rate_config));
  ESP_LOGI(TAG, "ESP-NOW initialized");
}

void csi_init() {
  const wifi_csi_config_t csi_config = {
      .lltf_en = true,
      .htltf_en = true,
      .stbc_htltf2_en = true,
      .ltf_merge_en = true,
      .channel_filter_en = true,
      .manu_scale = false,
      .shift = false,
  };
  ESP_ERROR_CHECK(esp_wifi_set_csi_config(&csi_config));
  ESP_ERROR_CHECK(esp_wifi_set_csi_rx_cb(wifi_csi_rx_cb, NULL));
  ESP_ERROR_CHECK(esp_wifi_set_csi(true));
  ESP_LOGI(TAG, "Wi-Fi CSI initialized");
}
