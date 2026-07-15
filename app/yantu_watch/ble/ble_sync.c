/****************************************************************************
 * apps/examples/yantu_watch/ble/ble_sync.c
 ****************************************************************************/

#include <nuttx/config.h>
#include <stdio.h>
#include <string.h>
#include "ble_sync.h"

static bool g_connected = false;

int ble_sync_init(void)
{
  g_connected = false;
  /* TODO: Initialize BLE GATT service */
  return 0;
}

int ble_sync_start_advertising(void)
{
  /* TODO: Start BLE advertising */
  return 0;
}

int ble_sync_stop_advertising(void)
{
  /* TODO: Stop BLE advertising */
  return 0;
}

bool ble_sync_is_connected(void)
{
  return g_connected;
}

int ble_sync_send_file(const char *local_path, const char *remote_name)
{
  /* TODO: Send file via BLE */
  return 0;
}

int ble_sync_get_status(void)
{
  return g_connected ? 1 : 0;
}