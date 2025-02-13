#ifndef APP_INIT_H
#define APP_INIT_H

/**
 * @brief Initialize Wi-Fi in station mode with specific configurations.
 *
 * This function sets up the Wi-Fi in station mode, initializes the default event loop,
 * network interface, and configures various Wi-Fi settings such as bandwidth, power save mode,
 * channel, and promiscuous mode.
 */
void wifi_init();

/**
 * @brief Initialize ESP-NOW with specific configurations.
 *
 * This function initializes ESP-NOW, sets the primary master key (PMK), adds a peer with
 * specific configurations, and sets the peer rate configuration.
 */
void now_init();

/**
 * @brief Initialize Wi-Fi Channel State Information (CSI) with specific configurations.
 *
 * This function sets up the CSI configuration, registers the CSI receive callback, and
 * enables CSI collection.
 */
void csi_init();

#endif // APP_INIT_H
