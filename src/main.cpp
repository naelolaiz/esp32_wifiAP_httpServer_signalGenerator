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
// based on https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/protocols/esp_http_server.html


#include "NaelCppLibrary.h"
#include <driver/gpio.h>

extern "C"
{
  void app_main();
}



#define AP_SSID "TEST_ESP32\0"
#define AP_PASSWORD "myPassword1234\0"

static const char* TAG = "TEST";
const int CONNECTED_BIT = BIT0;
EventGroupHandle_t wifi_event_group;


static char tempPage[4096];
static size_t tempPageSize = 0;

static char formPage[] = "<html>\n \
<head>\n \
<title>Control Led</title>\n \
</head>\n \
<body>\n \
  <form action=\"/form\" method=\"post\">\n \
  <input type=\"checkbox\" id=\"led1\" name=\"led1\">\n \
  <label for=\"led1\"> Led status </label><br>\n \
  <input type=\"checkbox\" id=\"led2\" name=\"led2\">\n \
  <label for=\"led2\"> Led status </label><br>\n \
  <input type=\"submit\" value=\"Submit\">\n \
</form>\n <br/>";
static char formPageEnding[] = "</body>\n \
</html>\n \
";



FormForLed formForLed;

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

void addLineToHtmlBuffer(const char * line,  size_t stringSize, size_t * startPos)
{ 
  static const char lineEnding[]="<br/>\n";

  size_t i=(*startPos==0?0:(*startPos)-1);
  memcpy(tempPage+(*startPos==0?0:(*startPos)-1), line, stringSize-1);
  (*startPos) = i+stringSize-1;
  memcpy(tempPage+(*startPos), lineEnding, sizeof(lineEnding));
  (*startPos) += sizeof(lineEnding);

}

/* Our URI handler function to be called during GET /uri request */
esp_err_t get_handler(httpd_req_t *req)
{
  if(strcmp(req->uri, "/form") ==0)
  {
//    tempPageSize=0;
//    addLineToHtmlBuffer(formPage, sizeof(formPage),&tempPageSize);
//    addLineToHtmlBuffer(formPageEnding, sizeof(formPageEnding),&tempPageSize);
//    httpd_resp_send(req, tempPage, HTTPD_RESP_USE_STRLEN);
    formForLed.clearExtraText();
    httpd_resp_send(req, formForLed.getHtmlPage().c_str(), HTTPD_RESP_USE_STRLEN);
  }
  else
  {
    tempPageSize=0;
    /* Send a simple response */
//    const char resp[] = "URI GET Response";
//    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    addLineToHtmlBuffer(" URI GET Response", sizeof(" URI GET Response"), &tempPageSize);
    addLineToHtmlBuffer("Hola mundo!", sizeof("Hola mundo!"), &tempPageSize);
    addLineToHtmlBuffer(req->uri, HTTPD_MAX_URI_LEN+1, &tempPageSize);
    httpd_resp_send(req, tempPage, HTTPD_RESP_USE_STRLEN);
  }
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

//    ESP_LOGI(TAG, content);
    /* Send a simple response */
    //const char resp[] = "URI POST Response";
    ////httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    //httpd_resp_send(req, content, 10);//HTTPD_RESP_USE_STRLEN);
//    formForLed.setExtraText(content);
    const bool led1 = formForLed.parseLed1Status(content);

    gpio_set_level(GPIO_NUM_2, led1);
    formForLed.setLed1Value(led1);
    formForLed.setLed2Value(formForLed.parseLed2Status(content));
    httpd_resp_send(req, formForLed.getHtmlPage().c_str(), HTTPD_RESP_USE_STRLEN);
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

/* URI handler structure for GET /form */
httpd_uri_t form_get = {
    .uri      = "/form",
    .method   = HTTP_GET,
    .handler  = get_handler,
    .user_ctx = NULL
};

/* URI handler structure for POST /form */
httpd_uri_t form_post = {
    .uri      = "/form",
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
        httpd_register_uri_handler(server, &form_get);
        httpd_register_uri_handler(server, &form_post);
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

void start_wifi_AP()
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
            {.ssid = AP_SSID},
            {.password = AP_PASSWORD},
            .channel = 0,
            .authmode = WIFI_AUTH_WPA2_PSK,
            .ssid_hidden = 0,
            .max_connection = 1,
            .beacon_interval = 100
          }
        };
 
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &ap_config));
 
    ESP_ERROR_CHECK(esp_wifi_start());


}

void setupLed()
{
  gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT);
}

void app_main()
{   
  start_wifi_AP();
  xTaskCreate(&wpa2_enterprise_example_task, "wpa2_enterprise_example_task", 4096, NULL, 5, NULL);
  start_webserver();
}