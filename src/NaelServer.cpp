#include "NaelServer.h"

/* Our URI handler function to be called during GET /uri request */
esp_err_t Server::Server::get_handler(httpd_req_t *req) {
  FormForLed &formForLed = static_cast<Server *>(req->user_ctx)->mFormForLed;
  if (strcmp(req->uri, "/form") == 0) {
    formForLed.clearExtraText();
    httpd_resp_send(req, formForLed.getHtmlPage().c_str(),
                    HTTPD_RESP_USE_STRLEN);
  } else {
    /* Send a simple response */
    const char resp[] = "URI GET Response";
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
  }
  return ESP_OK;
}

/* Our URI handler function to be called during POST /uri request */
esp_err_t Server::Server::post_handler(httpd_req_t *req) {
  /* Destination buffer for content of HTTP POST request.
   * httpd_req_recv() accepts char* only, but content could
   * as well be any binary data (needs type casting).
   * In case of string data, null termination will be absent, and
   * content length would give length of string */
  // char content[100];
  std::array<char, 100> content;
  content.fill(0);

  /* Truncate if content length larger than the buffer */
  size_t recv_size =
      req->content_len < sizeof(content) ? req->content_len : sizeof(content);

  int ret = httpd_req_recv(req, content.data(), recv_size);
  if (ret <= 0) { /* 0 return value indicates connection closed */
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

  FormForLed &mFormForLed = static_cast<Server *>(req->user_ctx)->mFormForLed;
  //    ESP_LOGI(TAG, content);
  /* Send a simple response */
  // const char resp[] = "URI POST Response";
  ////httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
  // httpd_resp_send(req, content, 10);//HTTPD_RESP_USE_STRLEN);
  //    mFormForLed.setExtraText(content.data());
  // mFormForLed.setExtraText(mFormForLed.parseToUart(content.data()));
  const bool led1 = mFormForLed.parseLed1Status(content.data());

  gpio_set_level(GPIO_NUM_16,
                 led1 ? 0 : 1); // TODO: remove. task checking and updating.
  mFormForLed.setLed1Value(led1);
  mFormForLed.setLed2Value(mFormForLed.parseLed2Status(content.data()));
  httpd_resp_send(req, mFormForLed.getHtmlPage().c_str(),
                  HTTPD_RESP_USE_STRLEN);

  // ESP_LOGI(TAG, "%s", mFormForLed.parseToUart(content.data()).c_str());

  return ESP_OK;
}

/* Function for stopping the webserver */
/* Function for starting the webserver */
httpd_handle_t Server::Server::start_webserver(void) {
  /* Generate default configuration */
  // httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  /* Empty handle to esp_http_server */
  // httpd_handle_t server = NULL;

  /* Start the httpd server */
  if (httpd_start(&mServer, &mConfig) == ESP_OK) {
    /* Register URI handlers */
    httpd_register_uri_handler(mServer, &uri_get);
    httpd_register_uri_handler(mServer, &uri_post);
    httpd_register_uri_handler(mServer, &form_get);
    httpd_register_uri_handler(mServer, &form_post);
  }
  /* If server failed to start, handle will be NULL */
  return mServer;
}
void Server::Server::stop_webserver(httpd_handle_t server) {
  if (server) {
    /* Stop the httpd server */
    httpd_stop(server);
  }
}

FormForLed Server::Server::mFormForLed = FormForLed();