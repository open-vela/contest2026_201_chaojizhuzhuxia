/****************************************************************************
 * apps/examples/yantu_watch/ble/ble_protocol.c
 ****************************************************************************/

#include <nuttx/config.h>
#include <stdio.h>
#include <string.h>
#include "ble_protocol.h"

int ble_proto_init(void)
{
  return 0;
}

int ble_proto_send_start(file_type_t type, const char *name, uint32_t size)
{
  /* TODO: Send start command */
  return 0;
}

int ble_proto_send_data(const uint8_t *data, uint16_t len)
{
  /* TODO: Send data packet */
  return 0;
}

int ble_proto_send_end(void)
{
  /* TODO: Send end command */
  return 0;
}

int ble_proto_send_ack(uint16_t packet_id)
{
  /* TODO: Send acknowledgment */
  return 0;
}

int ble_proto_process_packet(const uint8_t *buf, uint16_t len)
{
  /* TODO: Process incoming packet */
  return 0;
}