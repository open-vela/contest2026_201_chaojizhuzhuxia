#ifndef __YANTU_BLE_PROTOCOL_H
#define __YANTU_BLE_PROTOCOL_H

#include <stdbool.h>
#include <stdint.h>

/* BLE GATT Service UUID for yantu_watch sync */
#define YANTU_SYNC_SERVICE_UUID    "0000YANT-0000-1000-8000-00805F9B34FB"
#define YANTU_SYNC_CMD_CHAR_UUID   "0000YANT-0001-1000-8000-00805F9B34FB"
#define YANTU_SYNC_DATA_CHAR_UUID  "0000YANT-0002-1000-8000-00805F9B34FB"
#define YANTU_SYNC_ACK_CHAR_UUID   "0000YANT-0003-1000-8000-00805F9B34FB"

/* Protocol packet types */
typedef enum {
    SYNC_CMD_START = 0x01,   /* Start file transfer */
    SYNC_CMD_DATA  = 0x02,   /* File data packet */
    SYNC_CMD_END   = 0x03,   /* End file transfer */
    SYNC_CMD_ACK   = 0x04,   /* Acknowledgment */
    SYNC_CMD_NAK   = 0x05,   /* Negative acknowledgment */
    SYNC_CMD_CANCEL = 0x06,  /* Cancel transfer */
} sync_cmd_t;

/* File types */
typedef enum {
    FILE_TYPE_VOCAB = 0x01,  /* Vocabulary JSON */
    FILE_TYPE_AUDIO = 0x02,  /* Audio MP3 */
    FILE_TYPE_PLAN  = 0x03,  /* Study plan JSON */
} file_type_t;

/* Protocol packet header */
typedef struct __attribute__((packed)) {
    uint8_t  cmd;            /* sync_cmd_t */
    uint8_t  file_type;      /* file_type_t */
    uint16_t packet_id;      /* Packet sequence number */
    uint32_t total_size;     /* Total file size */
    uint16_t data_len;       /* Data length in this packet */
    uint16_t crc16;          /* CRC16 checksum */
} sync_packet_header_t;

#define SYNC_MAX_PACKET_DATA 512
#define SYNC_PACKET_HEADER_SIZE sizeof(sync_packet_header_t)
#define SYNC_PACKET_SIZE (SYNC_PACKET_HEADER_SIZE + SYNC_MAX_PACKET_DATA)

int  ble_proto_init(void);
int  ble_proto_send_start(file_type_t type, const char *name, uint32_t size);
int  ble_proto_send_data(const uint8_t *data, uint16_t len);
int  ble_proto_send_end(void);
int  ble_proto_send_ack(uint16_t packet_id);
int  ble_proto_process_packet(const uint8_t *buf, uint16_t len);

#endif /* __YANTU_BLE_PROTOCOL_H */