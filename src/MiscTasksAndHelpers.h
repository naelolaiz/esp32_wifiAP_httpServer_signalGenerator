#ifndef __NAEL_TASKS_H__
#define __NAEL_TASKS_H__
#include "esp_log.h"
#include <esp_netif.h>
#include <string.h>
namespace Misc {

namespace Tasks {
static const char *TAG = "TEST";
static void monitor_tcpip_task(void *pvParameters);
} // namespace Tasks

} // namespace Misc

#endif // __NAEL_TASKS_H__