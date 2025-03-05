# ESP32 CSI Data Collection Project

This project involves multiple ESP32 devices that communicate with each other using ESP-NOW to collect and log Channel
State Information (CSI) data. Each device is flashed with the same program, with the default device ID changing the
first time. The device ID is persisted, allowing the same program to be flashed to all devices. Device 0 logs all sent
and received payloads, which contain CSI data. The system uses a timeout mechanism and follows a round-robin protocol
where each device sends a packet once the previous device has sent a packet.

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

To build and flash the firmware to the ESP32 devices, you need to have the ESP-IDF installed. If you haven't already
installed the ESP-IDF, follow the instructions in
the [ESP-IDF Programming Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html).
The Makefile assumes that the ESP-IDF is installed in the `~/esp/esp-idf` directory. If it is installed in a different
directory, update the `IDF_PATH` variable in the Makefile.

Next, add the ports for the ESP32 devices to the `Makefile`:

```makefile
PORTS = /dev/ttyUSB0 /dev/ttyUSB1 /dev/ttyUSB2 /dev/ttyUSB3
```

These ports can be found by running the following in the terminal:

```bash
ls /dev/tty*
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

If the device is being flash for the first time, set the `DEFAULT_DEVICE_ID` variable in the `constants.h` file to the
device ID of the device being flashed.

To flash the firmware and then monitor the serial output, use:

```sh
make flash-monitor DEVICE=<device_id>
```

To clean the build directory, use:

```sh
make clean
```

## Logging

The first device (device 0) logs all sent and received payloads to the console in the form of comma-separated value. The
header of the log is as follows:

```csv
type,time_index,device_id,recv_device_id,mac,rssi,rate,sig_mode,mcs,cwb,smoothing,not_sounding,aggregation,stbc,fec_coding,sgi,noise_floor,ampdu_cnt,channel,secondary_channel,timestamp,ant,sig_len,rx_state,len,csi_data
```

The following columns have been added to the log:

- `time_index`: This is used to determine which iteration the data was sent or received. It is also used to identify the
  packets that were sent and received at the same time.
- `device_id`: The device ID of the device that sent the packet.
- `recv_device_id`: The device ID of the device that received the packet.

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
