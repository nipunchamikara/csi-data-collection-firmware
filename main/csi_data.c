#include <rom/ets_sys.h>
#include <string.h>
#include <unistd.h>

#include "esp_log.h"
#include "esp_now.h"

#include "constants.h"
#include "csi_data.h"

payload_t payload;

uint8_t last_id;

uint16_t last_time_index;

uint8_t timeout_count;

uint8_t payload_index;

uint8_t org_id[3] = ORG_ID;

uint8_t payload_magic_number[] = PAYLOAD_MAGIC_NUMBER;

/**
 * Returns the minimum of two values.
 * @param a The first value.
 * @param b The second value.
 * @return The minimum of the two values.
 */
uint8_t min(const uint8_t a, const uint8_t b) { return a < b ? a : b; }

void send_csi_data(const uint16_t time_index) {
  const uint8_t peer_addr[] = BROADCAST_ADDR;
  payload.time_index = time_index;
  memcpy(&payload.magic, payload_magic_number, sizeof(payload_magic_number));
  const esp_err_t ret = esp_now_send(peer_addr, (uint8_t *) &payload, sizeof(payload_t));
  if (ret != ESP_OK) {
    ESP_LOGW(TAG, "<%s> ESP-NOW send error", esp_err_to_name(ret));
  } else if (payload.device_id != 0) {
    ESP_LOGI(TAG, "ESP-NOW packet sent");
  }
}

/**
 * Prints the CSI data to the console.
 * @param csi_data The CSI data to print.
 * @param recv_device_id The device ID that received the CSI data.
 */
void print_csi_data(const csi_data_t *csi_data, const uint8_t recv_device_id) {
  const wifi_pkt_rx_ctrl_t *rx_ctrl = &csi_data->rx_ctrl;
  ets_printf("CSI_DATA,%d,%d,%d," MACSTR
             ",%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
             csi_data->time_index, csi_data->device_id, recv_device_id, MAC2STR(csi_data->mac),
             rx_ctrl->rssi, rx_ctrl->rate, rx_ctrl->sig_mode, rx_ctrl->mcs, rx_ctrl->cwb,
             rx_ctrl->smoothing, rx_ctrl->not_sounding, rx_ctrl->aggregation, rx_ctrl->stbc,
             rx_ctrl->fec_coding, rx_ctrl->sgi, rx_ctrl->noise_floor, rx_ctrl->ampdu_cnt,
             rx_ctrl->channel, rx_ctrl->secondary_channel, rx_ctrl->timestamp, rx_ctrl->ant,
             rx_ctrl->sig_len, rx_ctrl->rx_state);

  ets_printf(",%d,\"[%d", CSI_DATA_LENGTH, csi_data->buf[0]);

  for (int i = 1; i < CSI_DATA_LENGTH; i++) {
    ets_printf(",%d", csi_data->buf[i]);
  }

  ets_printf("]\"\n");
}

/**
 * Prints the payload to the console.
 * @param payload The payload to print.
 */
void print_payload(const payload_t *payload) {
  for (int i = 0; i < payload->csi_data_arr_len; i++) {
    // TODO: Investigate why the next elements are offset by 7 bytes each
    const int offset = payload->device_id ? 7 * i : 0;
    print_csi_data((void *) &payload->csi_data_arr[i] + offset, payload->device_id);
  }
}

void wifi_csi_rx_cb(void *ctx, wifi_csi_info_t *info) {
  if (!info || !info->buf) {
    ESP_LOGW(TAG, "<%s> wifi_csi_cb", esp_err_to_name(ESP_ERR_INVALID_ARG));
    return;
  }

  // Check if the received packet is from an ESP32 device, if not, return
  const uint8_t *org = info->payload + 1;
  if (memcmp(org, org_id, 3) != 0)
    return;

  // ESP-NOW header takes up 15 bytes (refer to README.md)
  const payload_t *p = (struct payload *) (info->payload + 15);

  // Check if the magic number is valid
  if (memcmp(p->magic, payload_magic_number, sizeof(payload_magic_number)) != 0) {
    return;
  }

  // Check if the device ID is valid
  if (p->device_id >= TOTAL_DEVICES)
    return;

  if (payload.device_id != 0) {
    ESP_LOGI(TAG, "Receiving CSI data from device %d at time index %d", p->device_id,
             p->time_index);
  }

  // Overwrite the oldest CSI data if the array is full or the time index is 0
  if (payload_index >= CSI_DATA_ARR_LEN)
    payload_index = 0;

  if (p->time_index == 0) {
    payload_index = 0;
    payload.csi_data_arr_len = 0;
  }

  csi_data_t *csi_data = &payload.csi_data_arr[payload_index];

  const wifi_pkt_rx_ctrl_t *rx_ctrl = &info->rx_ctrl;

  // Copy the CSI data to the payload
  csi_data->device_id = p->device_id;
  memcpy(&csi_data->rx_ctrl, rx_ctrl, sizeof(wifi_pkt_rx_ctrl_t));
  memcpy(csi_data->mac, info->mac, sizeof(info->mac));
  memcpy(csi_data->dmac, info->dmac, sizeof(info->dmac));
  memcpy(csi_data->buf, info->buf, info->len * sizeof(uint8_t));
  csi_data->time_index = p->time_index;

  // Print the payload of received packet if current device is the first device
  if (payload.device_id == 0)
    print_payload(p);

  last_id = p->device_id;
  last_time_index = p->time_index;
  timeout_count = 0;
  payload.csi_data_arr_len = min(payload.csi_data_arr_len + 1, CSI_DATA_ARR_LEN - 1);
  payload_index++;

  if (payload.device_id == (p->device_id + 1) % TOTAL_DEVICES) {
    // Print payload of sent packet if current device is the first device
    if (payload.device_id == 0)
      print_payload(&payload);
    send_csi_data(++last_time_index);
    payload.csi_data_arr_len = 0;
    payload_index = 0;
  }
}
