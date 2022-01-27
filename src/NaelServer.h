#ifndef __NAEL_SERVER_H__
#define __NAEL_SERVER_H__

// http
extern "C" 
{
    #include <esp_http_server.h>
}
// based on https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/protocols/esp_http_server.html
#include "NaelCppLibrary.h"
#include <driver/gpio.h> // TODO: remove
#include <array>


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
  <label for=\"toUart\">Send to Uart:</label><br>\n \
  <input type=\"text\" id=\"toUart\" name=\"toUart\"><br>\n \
  <input type=\"submit\" value=\"Submit\">\n \
</form>\n <br/>";
static char formPageEnding[] = "</body>\n \
</html>\n \
";

FormForLed formForLed;

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
    //char content[100];
    std::array<char,100> content;
    content.fill(0);

    /* Truncate if content length larger than the buffer */
    size_t recv_size = req->content_len < sizeof(content) ? req->content_len : sizeof(content);

    int ret = httpd_req_recv(req, content.data(), recv_size);
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
//    formForLed.setExtraText(content.data());
    formForLed.setExtraText(formForLed.parseToUart(content.data()));
    const bool led1 = formForLed.parseLed1Status(content.data());

    gpio_set_level(GPIO_NUM_16, led1?0:1); // TODO: remove. task checking and updating. 
    formForLed.setLed1Value(led1);
    formForLed.setLed2Value(formForLed.parseLed2Status(content.data()));
    httpd_resp_send(req, formForLed.getHtmlPage().c_str(), HTTPD_RESP_USE_STRLEN);

    //ESP_LOGI(TAG, "%s", formForLed.parseToUart(content.data()).c_str()); 

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




#endif // __NAEL_SERVER_H__