#ifndef APP_NVS_H
#define APP_NVS_H

#include <stdint.h>

/**
 * @brief Store the device ID in non-volatile storage (NVS).
 *
 * This function stores the given device ID in the NVS, allowing it to be
 * retrieved later even after a reboot or power cycle.
 *
 * @param device_id The device ID to store.
 */
void store_device_id(uint8_t device_id);

/**
 * @brief Retrieve the device ID from non-volatile storage (NVS).
 *
 * This function retrieves the stored device ID from the NVS.
 *
 * @return The stored device ID.
 */
uint8_t get_device_id();

#endif // APP_NVS_H
