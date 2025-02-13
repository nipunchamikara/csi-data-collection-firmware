# ESP32 CSI Data Collection Project

This project involves multiple ESP32 devices that communicate with each other using ESP-NOW to collect and log Channel
State Information (CSI) data. Each device is flashed with the same program, with the default device ID changing the
first time. The device ID is persisted, allowing the same program to be flashed to all devices. Device 0 logs all sent
and received payloads, which contain CSI data. The system uses a timeout mechanism and follows a round-robin protocol
where each device sends a packet once the previous device has sent a packet.

## Project Structure

- `CMakeLists.txt`: CMake build configuration file.
- `app_init.c/h`: Source and header file for application initialization.
- `app_main.c`: Main application source file.
- `app_nvs.c/h`: Source and header file for non-volatile storage (NVS) operations.
- `constants.h`: Defines constants used throughout the project.
- `csi_data.c/h`: Source and header file for handling CSI data.
- `idf_component.yml`: Component configuration file.

## Files

### `CMakeLists.txt`

This file contains the build configuration for the project using CMake.

### `app_init.c` and `app_init.h`

These files handle the initialization of the application, including setting up the ESP-NOW communication and configuring
the device.

### `app_main.c`

This is the main application file where the primary logic of the project is implemented.

### `app_nvs.c` and `app_nvs.h`

These files handle operations related to non-volatile storage (NVS), such as storing and retrieving the device ID.

### `constants.h`

Defines various constants used in the project, such as device IDs, timeout values, and buffer sizes.

### `csi_data.c` and `csi_data.h`

These files define structures and functions for handling CSI data. Key structures include:

- `csi_data_t`: Holds CSI data received from a device.
- `payload_t`: Holds the payload of CSI data received from all devices.

Key functions include:

- `send_csi_data()`: Sends the CSI data to the broadcast address using ESP-NOW.
- `print_csi_data(const csi_data_t *csi_data)`: Prints the CSI data to the console.
- `print_payload(const payload_t *payload)`: Prints the payload to the console.
- `wifi_csi_rx_cb(void *ctx, wifi_csi_info_t *info)`: Callback function to handle received CSI data.
- `turns_away(const uint8_t device_id)`: Returns the number of devices between the current and last device IDs.

## How It Works

1. **Flashing the Devices**: Each ESP32 device is flashed with the same program. The default device ID is set during the
   first run and is persisted for subsequent runs.
2. **Device 0**: Device 0 logs all sent and received payloads, which contain CSI data.
3. **Timeout System**: The system uses a timeout mechanism where each device waits for a certain number of iterations
   before sending a packet. The number of iterations is based on how many devices away the device is from the previous
   device.
4. **Round-Robin Protocol**: The devices follow a round-robin protocol where each device sends a packet once the
   previous device has sent a packet.
5. **Piggybacking CSI Data**: The devices piggyback CSI data on the ESP-NOW packets they send to the broadcast address.

## Building and Flashing

First, add the ports for the ESP32 devices to the `Makefile`:

```makefile
PORTS = /dev/ttyUSB0 /dev/ttyUSB1 /dev/ttyUSB2
```

To build and flash the firmware to the ESP32 devices, use the following commands:

```sh
make install
make build
make flash DEVICE=<device_id>
make monitor DEVICE=<device_id>
```

Replace `<device_id>` with the device ID of the ESP32 device you want to flash. The device ID should be an integer.
If the device ID is not specified, the default device ID is 0.

To flash the firmware and then monitor the serial output, use:

```sh
make flash-monitor DEVICE=<device_id>
```

To clean the build directory, use:

```sh
make clean
```

## Contributing

Contributions are welcome! For feature requests, bug reports, or questions, please open an issue.

## License

This project is licensed under the MIT License. See the `LICENSE` file for more information.

## Appendix

### Obtaining payload data from callback function `wifi_csi_rx_cb`

#### Vendor Specific Action Frame

| MAC Header | Category Code | Organization Identifier | Random Values | Vendor Specific Content | FCS     |
|------------|---------------|-------------------------|---------------|-------------------------|---------|
| 24 bytes   | 1 byte        | 3 bytes                 | 4 bytes       | 7-1532 bytes            | 4 bytes |

#### Vendor Specific Element Frame

| Element ID | Length | Organization Identifier | Type   | Reserved | More data | Version  | Body         |
|------------|--------|-------------------------|--------|----------|-----------|----------|--------------|
| 1 byte     | 1 byte | 3 bytes                 | 1 byte | 7-5 bits | 1 bit     | 3-0 bits | 0-1490 bytes |

The payload data of the Wi-Fi packet is obtained from the `wifi_csi_info_t` structure using the `payload` field.
It starts with the Category Code of the Vendor Specific Action frame. This means, the payload of the ESP-NOW packet
begins at index 15. 
