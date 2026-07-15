#ifndef __YANTU_BLE_SYNC_H
#define __YANTU_BLE_SYNC_H

#include <stdbool.h>
#include <stddef.h>

int  ble_sync_init(void);
int  ble_sync_start_advertising(void);
int  ble_sync_stop_advertising(void);
bool ble_sync_is_connected(void);
int  ble_sync_send_file(const char *local_path, const char *remote_name);
int  ble_sync_get_status(void);

#endif /* __YANTU_BLE_SYNC_H */