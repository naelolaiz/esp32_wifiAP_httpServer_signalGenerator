#ifndef __NAEL_SERVER_H__
#define __NAEL_SERVER_H__

// http
extern "C" {
#include <esp_http_server.h>
}

// for led setting
#include "OnBoardLedManager.h"
// for controlling the signal generator
#include "AD9833_Manager.h"
#include <memory>
#include <optional>

class FormForLed;
// based on
// https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/protocols/esp_http_server.html
class Server {
private:
  static FormForLed mFormForLed;
  httpd_config_t mConfig = HTTPD_DEFAULT_CONFIG();
  /* Empty handle to esp_http_server */
  httpd_handle_t mServer{nullptr};

  /* URI handler structure for GET /uri */
  httpd_uri_t uri_get = {.uri = "/uri",
                         .method = HTTP_GET,
                         .handler = get_handler,
                         .user_ctx = this};

  /* URI handler structure for POST /uri */
  httpd_uri_t uri_post = {.uri = "/uri",
                          .method = HTTP_POST,
                          .handler = post_handler,
                          .user_ctx = this};

  /* URI handler structure for GET /form */
  httpd_uri_t form_get = {.uri = "/form",
                          .method = HTTP_GET,
                          .handler = get_handler,
                          .user_ctx = this};

  /* URI handler structure for POST /form */
  httpd_uri_t form_post = {.uri = "/form",
                           .method = HTTP_POST,
                           .handler = post_handler,
                           .user_ctx = this};

  /* URI handler structure for GET /form */
  httpd_uri_t oscControlGet = {.uri = "/siggen",
                               .method = HTTP_GET,
                               .handler = get_handler,
                               .user_ctx = this};

  /* URI handler structure for POST /form */
  httpd_uri_t oscControlPost = {.uri = "/siggen",
                                .method = HTTP_POST,
                                .handler = post_handler,
                                .user_ctx = this};

public:
  Server(std::optional<std::shared_ptr<AD9833Manager::SigGenOrchestrator>>
             sigGenOrchestrator = nullptr);
  static esp_err_t get_handler(httpd_req_t *req);
  static esp_err_t post_handler(httpd_req_t *req);
  esp_err_t start_webserver();
  esp_err_t stop_webserver();

private:
  std::optional<std::shared_ptr<AD9833Manager::SigGenOrchestrator>>
      mSigGenOrchestrator;
};

#endif // __NAEL_SERVER_H__