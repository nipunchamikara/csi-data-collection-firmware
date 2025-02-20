#ifndef CSI_DATA_H
#define CSI_DATA_H

#include "esp_now.h"

/**
 * @brief Structure to hold CSI data received from a device.
 *
 * This structure contains the device ID, RX control information, source MAC address,
 * destination MAC address,
 */
typedef struct csi_data {
  uint8_t device_id; /**< Device ID that has sent the CSI data (Tx) */
  uint16_t time_index; /**< Time index when the CSI data was sent/received */
  wifi_pkt_rx_ctrl_t rx_ctrl; /**< RX control information */
  uint8_t mac[6]; /**< Source MAC address of the CSI data */
  uint8_t dmac[6]; /**< Destination MAC address of the CSI data */
  int8_t buf[CSI_DATA_LENGTH]; /**< CSI data buffer */
} csi_data_t;

/**
 * @brief Structure to hold the payload of CSI data received from all devices.
 *
 * This structure contains the device ID that has received the CSI data (Rx) and an array of
 * CSI data.
 */
typedef struct payload {
  uint8_t device_id; /**< Device ID that has received the CSI data (Rx) */
  uint16_t time_index; /**< Time index when the payload was sent/received */
  uint8_t csi_data_arr_len; /**< Number of CSI data entries */
  csi_data_t csi_data_arr[CSI_DATA_ARR_LEN]; /**< Array of CSI data */
} payload_t;

/**
 * Stores the CSI data that is sent to the broadcast address using ESP-NOW.
 */
extern payload_t payload;

/**
 * The last device ID from which CSI data was received.
 */
extern uint8_t last_id;

/**
 * The time index of the last received CSI data.
 */
extern uint16_t last_time_index;

/**
 * The number of loop iterations before sending CSI data.
 * If the timeout count exceeds the timeout threshold, CSI data is sent to the broadcast address.
 */
extern uint8_t timeout_count;

/**
 * The index of the payload.
 */
extern uint8_t payload_index;

/**
 * Sends the CSI data to the broadcast address using ESP-NOW.
 *
 * @param time_index The time index of the payload
 */
void send_csi_data(uint16_t time_index);

/**
 * @brief Callback function to handle received CSI data.
 *
 *  This function is called when CSI data is received from a device. It extracts the CSI data
 *  and stores it in the payload. If the current device is the first device, it prints the
 *  payload of the received packet. If the current device is the next device in the sequence,
 *  it sends the CSI data to the broadcast address using ESP-NOW.
 *
 * @param ctx Context
 * @param info CSI data
 */
void wifi_csi_rx_cb(void *ctx, wifi_csi_info_t *info);

#endif // CSI_DATA_H
