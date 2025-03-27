#ifndef CONSTANTS_H
#define CONSTANTS_H

#define MICROSECONDS_IN_SECOND 1000000
#define BROADCAST_ADDR {0xff, 0xff, 0xff, 0xff, 0xff, 0xff}
#define TAG "csi_send_recv"
#define CSI_DATA_LENGTH 256
#define CSI_DATA_ARR_LEN CONFIG_TOTAL_DEVICES
#define ORG_ID {0x18, 0xfe, 0x34}
#define PAYLOAD_MAGIC_NUMBER {0x0F, 0xA7, 0x5A, 0x09}

#endif // CONSTANTS_H
