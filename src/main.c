#include "esp_wifi.h"
#include "esp_log.h"
//#include "tcpip_adapter.h"
#include "esp_netif.h"
#include "esp_err.h"
#include "nvs_flash.h"
#include <stdlib.h> 
#include <string.h>
#include "freertos/event_groups.h"

// http
#include <esp_http_server.h>


#define AP_SSID "TEST_ESP32\0"
#define AP_PASSWORD "myPassword1234\0"

static const char* TAG = "TEST";
const int CONNECTED_BIT = BIT0;
EventGroupHandle_t wifi_event_group;


static esp_err_t event_handler(void *ctx, system_event_t *event)
{
  switch(event->event_id) {
    case SYSTEM_EVENT_STA_START:
      esp_wifi_connect();
      break;
    case SYSTEM_EVENT_STA_GOT_IP:
      xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
      break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
      esp_wifi_connect();
      xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
      break;
    default:
      break;
  }
  return ESP_OK;
}



 static void wpa2_enterprise_example_task(void *pvParameters)
{
  tcpip_adapter_ip_info_t ip;
  memset(&ip, 0, sizeof(tcpip_adapter_ip_info_t));
  vTaskDelay(2000 / portTICK_PERIOD_MS);

  while (1) {
    vTaskDelay(5000 / portTICK_PERIOD_MS);

    if (tcpip_adapter_get_ip_info(TCPIP_ADAPTER_IF_AP , &ip) == 0) {
      ESP_LOGI(TAG, "~~~~~~~~~~~");
      ESP_LOGI(TAG, "IP:"IPSTR, IP2STR(&ip.ip));
      ESP_LOGI(TAG, "MASK:"IPSTR, IP2STR(&ip.netmask));
      ESP_LOGI(TAG, "GW:"IPSTR, IP2STR(&ip.gw));
      ESP_LOGI(TAG, "~~~~~~~~~~~");
    }
  }
}

#ifdef OLD_TEST_HTTPD
void httpd_task(void *pvParameters)
{
    struct netconn *client = NULL;
    struct netconn *nc = netconn_new(NETCONN_TCP);
    if (nc == NULL) {
        printf("Failed to allocate socket\n");
        vTaskDelete(NULL);
    }
    netconn_bind(nc, IP_ADDR_ANY, 80);
    netconn_listen(nc);
    char buf[512];
    while (1) {
        err_t err = netconn_accept(nc, &client);
        if (err == ERR_OK) {
            struct netbuf *nb;
            if ((err = netconn_recv(client, &nb)) == ERR_OK) {
                void *data;
                u16_t len;
                netbuf_data(nb, &data, &len);
                printf("Received data:\n%.*s\n", len, (char*) data);
                snprintf(buf, sizeof(buf),
                        "HTTP/1.1 200 OK\r\n"
                        "Content-type: text/html\r\n\r\n"
                        "Test");
                netconn_write(client, buf, strlen(buf), NETCONN_COPY);
            }
            netbuf_delete(nb);
        }
        printf("Closing connection\n");
        netconn_close(client);
        netconn_delete(client);
    }
}
#else
/* Our URI handler function to be called during GET /uri request */
esp_err_t get_handler(httpd_req_t *req)
{
    /* Send a simple response */
    const char resp[] = "URI GET Response";
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

/* Our URI handler function to be called during POST /uri request */
esp_err_t post_handler(httpd_req_t *req)
{
    /* Destination buffer for content of HTTP POST request.
     * httpd_req_recv() accepts char* only, but content could
     * as well be any binary data (needs type casting).
     * In case of string data, null termination will be absent, and
     * content length would give length of string */
    char content[100];

    /* Truncate if content length larger than the buffer */
    size_t recv_size = req->content_len < sizeof(content) ? req->content_len : sizeof(content);

    int ret = httpd_req_recv(req, content, recv_size);
    if (ret <= 0) {  /* 0 return value indicates connection closed */
        /* Check if timeout occurred */
        if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
            /* In case of timeout one can choose to retry calling
             * httpd_req_recv(), but to keep it simple, here we
             * respond with an HTTP 408 (Request Timeout) error */
            httpd_resp_send_408(req);
        }
        /* In case of error, returning ESP_FAIL will
         * ensure that the underlying socket is closed */
        return ESP_FAIL;
    }

    /* Send a simple response */
    const char resp[] = "URI POST Response";
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

/* URI handler structure for GET /uri */
httpd_uri_t uri_get = {
    .uri      = "/uri",
    .method   = HTTP_GET,
    .handler  = get_handler,
    .user_ctx = NULL
};

/* URI handler structure for POST /uri */
httpd_uri_t uri_post = {
    .uri      = "/uri",
    .method   = HTTP_POST,
    .handler  = post_handler,
    .user_ctx = NULL
};

/* Function for starting the webserver */
httpd_handle_t start_webserver(void)
{
    /* Generate default configuration */
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    /* Empty handle to esp_http_server */
    httpd_handle_t server = NULL;

    /* Start the httpd server */
    if (httpd_start(&server, &config) == ESP_OK) {
        /* Register URI handlers */
        httpd_register_uri_handler(server, &uri_get);
        httpd_register_uri_handler(server, &uri_post);
    }
    /* If server failed to start, handle will be NULL */
    return server;
}

/* Function for stopping the webserver */
void stop_webserver(httpd_handle_t server)
{
    if (server) {
        /* Stop the httpd server */
        httpd_stop(server);
    }
}
#endif


void app_main()
{   
  // init nvram - used by the WIFI storage
  ESP_ERROR_CHECK( nvs_flash_init());


  //
  wifi_event_group = xEventGroupCreate();
  ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );

  // https://www.esp32.com/viewtopic.php?t=340
  tcpip_adapter_init(); 

    wifi_init_config_t wifiInitializationConfig = WIFI_INIT_CONFIG_DEFAULT();
 
    ESP_ERROR_CHECK(esp_wifi_init(&wifiInitializationConfig));
 
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
 
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));

    wifi_config_t ap_config = {
          .ap = {
            .ssid = AP_SSID,
            .password = AP_PASSWORD,
            .channel = 0,
            .authmode = WIFI_AUTH_WPA2_PSK,
            .ssid_hidden = 0,
            .max_connection = 1,
            .beacon_interval = 100
          }
        };
 
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &ap_config));
 
    ESP_ERROR_CHECK(esp_wifi_start());

    xTaskCreate(&wpa2_enterprise_example_task, "wpa2_enterprise_example_task", 4096, NULL, 5, NULL);
#ifdef OLD_TEST_HTTPD
    xTaskCreate(&httpd_task, "http_server", 1024, NULL, 2, NULL);
#endif
start_webserver();
 
}

#if 0
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_wpa2.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "tcpip_adapter.h"

#define AP_SSID "ESP32_Test"
#define AP_PASSWORD "nael_test"

EventGroupHandle_t wifi_event_group;

const int CONNECTED_BIT = BIT0;

static const char* TAG = "TEST";

static esp_err_t event_handler(void *ctx, system_event_t *event)
{
  switch(event->event_id) {
    case SYSTEM_EVENT_STA_START:
      esp_wifi_connect();
      break;
    case SYSTEM_EVENT_STA_GOT_IP:
      xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
      break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
      esp_wifi_connect();
      xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
      break;
    default:
      break;
  }
  return ESP_OK;
}

static void initialise_wifi(void)
{
  wifi_event_group = xEventGroupCreate();
  ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );

  // https://www.esp32.com/viewtopic.php?t=340
  tcpip_adapter_init();

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK( esp_wifi_init(&cfg) );

  ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );

  ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_AP) );
  wifi_config_t ap_cfg;
  ap_cfg.ap.authmode = WIFI_AUTH_WPA2_PSK;
  // docs says default 4 max 4 but without explicitly setting it, i keep getting max connections?
  ap_cfg.ap.max_connection = 4;
  // memcpy instead of strcpy because ap.ssid and ap.password are declared as uint8_t for some reason :(
  memcpy(ap_cfg.ap.ssid, AP_SSID, strlen(AP_SSID));
  memcpy(ap_cfg.ap.password, AP_PASSWORD, strlen(AP_PASSWORD));
  
  ap_cfg.ap.channel = 0;
  ap_cfg.ap.ssid_len = 10;
  ESP_LOGI(TAG, "Setting WiFi configuration SSID %s...", ap_cfg.ap.ssid);

  ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_AP, &ap_cfg) );
  ESP_ERROR_CHECK( esp_wifi_start() );
  
  // does not currently work to set the ESP32's address
  /*tcpip_adapter_ip_info_t ipInfo;
  IP4_ADDR(&ipInfo.ip, 192,168,1,1);
  IP4_ADDR(&ipInfo.gw, 192,168,1,1);
  IP4_ADDR(&ipInfo.netmask, 255,255,255,0);
  tcpip_adapter_set_ip_info(TCPIP_ADAPTER_IF_AP, &ipInfo);*/
}

static void wpa2_enterprise_example_task(void *pvParameters)
{
  tcpip_adapter_ip_info_t ip;
  memset(&ip, 0, sizeof(tcpip_adapter_ip_info_t));
  vTaskDelay(2000 / portTICK_PERIOD_MS);

  while (1) {
    vTaskDelay(5000 / portTICK_PERIOD_MS);

    if (tcpip_adapter_get_ip_info(TCPIP_ADAPTER_IF_AP , &ip) == 0) {
      ESP_LOGI(TAG, "~~~~~~~~~~~");
      ESP_LOGI(TAG, "IP:"IPSTR, IP2STR(&ip.ip));
      ESP_LOGI(TAG, "MASK:"IPSTR, IP2STR(&ip.netmask));
      ESP_LOGI(TAG, "GW:"IPSTR, IP2STR(&ip.gw));
      ESP_LOGI(TAG, "~~~~~~~~~~~");
    }
  }
}

void app_main()
{
//  const uart_port_t uart_num = UART_NUM_2;
//  uart_config_t uart_config = {
      //.baud_rate=9600};
      /*
    .baud_rate = 115200,
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_CTS_RTS,
    .rx_flow_ctrl_thresh = 122,
};*/
// Configure UART parameters
//ESP_ERROR_CHECK(uart_param_config(uart_num, &uart_config));
  ESP_ERROR_CHECK( nvs_flash_init() );
  initialise_wifi();
  xTaskCreate(&wpa2_enterprise_example_task, "wpa2_enterprise_example_task", 4096, NULL, 5, NULL);
}
#endif