#include "HttpServer.h"
#include "NaelWebPage.h"
#include <array>

namespace {

class ParseRequests {
public:
  // static size_t getSizeT(httpd_req_t *req, const char *id) {
  static size_t getSizeT(const char *content, const char *id) {
    char buffer[30];
    const esp_err_t err =
        //        httpd_req_get_hdr_value_str(req, id, buffer, sizeof(buffer));
        httpd_query_key_value(content, id, buffer, sizeof(buffer));

    if (err != ESP_OK) {
      return 0;
    }
    return std::atoi(buffer);
  }
  static size_t getFrequency(const char *content) {
    static const std::string id = "number-frequency-osc1";
    return getSizeT(content, id.c_str());
  }
  static float getFloat(const char *content, const char *id) {
    char buffer[30];
    const esp_err_t err =
        //        httpd_req_get_hdr_value_str(req, id, buffer, sizeof(buffer));
        httpd_query_key_value(content, id, buffer, sizeof(buffer));

    if (err != ESP_OK) {
      return 0;
    }
    return std::atof(buffer);
  }

  static float getPhase(const char *content) {
    static const std::string id = "number-phase-osc1";
    return getFloat(content, id.c_str());
  }

  // static std::string getStdString(httpd_req_t *req, const char *id) {
  static std::string getStdString(const char *content, const char *id) {
    char buffer[255];
    const esp_err_t err =
        httpd_query_key_value(content, id, buffer, sizeof(buffer));
    // httpd_req_get_hdr_value_str(req, id, buffer, sizeof(buffer));
    //        ESP_ERROR_CHECK()
    if (err != ESP_OK) {
      return "";
    }
    return std::string(buffer);
  }
};
} // namespace

Server::Server(Misc::OnBoardLedManager &onBoardLedMonitor)
    : mOnBoardLedMonitor(onBoardLedMonitor) {}

/* Our URI handler function to be called during GET /uri request */
esp_err_t Server::Server::get_handler(httpd_req_t *req) {
  FormForLed &formForLed = static_cast<Server *>(req->user_ctx)->mFormForLed;
  if (strcmp(req->uri, "/form") == 0) {
    formForLed.clearExtraText();
    httpd_resp_send(req, formForLed.getHtmlPage().c_str(),
                    HTTPD_RESP_USE_STRLEN);
  }
  if (strcmp(req->uri, "/siggen") == 0) {
    httpd_resp_send(req, mFormForLed.getOscControlPage().c_str(),
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

  if (strcmp(req->uri, "/form") == 0) {
    FormForLed &mFormForLed = static_cast<Server *>(req->user_ctx)->mFormForLed;
    //    ESP_LOGI(TAG, content);
    /* Send a simple response */
    // const char resp[] = "URI POST Response";
    ////httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    // httpd_resp_send(req, content, 10);//HTTPD_RESP_USE_STRLEN);
    //    mFormForLed.setExtraText(content.data());
    // mFormForLed.setExtraText(mFormForLed.parseToUart(content.data()));
    const bool led1 = mFormForLed.parseLed1Status(content.data());

    mOnBoardLedMonitor.setRequestedValue(led1);
    mFormForLed.setLed1Value(led1);
    mFormForLed.setLed2Value(mFormForLed.parseLed2Status(content.data()));
    httpd_resp_send(req, mFormForLed.getHtmlPage().c_str(),
                    HTTPD_RESP_USE_STRLEN);

    // ESP_LOGI(TAG, "%s", mFormForLed.parseToUart(content.data()).c_str());
  } else if (strcmp(req->uri, "/siggen") == 0) {
    const size_t frequency = ParseRequests::getFrequency(content.data());
    const std::string waveform =
        ParseRequests::getStdString(content.data(), "select-waveform-osc1") +
        " - " + std::to_string(frequency) + " - " +
        std::to_string(ParseRequests::getPhase(content.data()));
    if (waveform.compare("off0") == 0) {
      mOnBoardLedMonitor.setRequestedValue(1);
    }

    // httpd_resp_send(req, content.data(), HTTPD_RESP_USE_STRLEN);
    httpd_resp_send(req, waveform.c_str(), HTTPD_RESP_USE_STRLEN);
    httpd_resp_send(req, mFormForLed.getOscControlPage().c_str(),
                    HTTPD_RESP_USE_STRLEN);
  } else {
    const char resp[] = "WTF?";
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
  }

  return ESP_OK;
}

/* Function for starting the webserver */
esp_err_t Server::Server::start_webserver() {
  /* Start the httpd server */
  const auto err = httpd_start(&mServer, &mConfig);
  if (err == ESP_OK) {
    /* Register URI handlers */
    httpd_register_uri_handler(mServer, &uri_get);
    httpd_register_uri_handler(mServer, &uri_post);
    httpd_register_uri_handler(mServer, &form_get);
    httpd_register_uri_handler(mServer, &form_post);
    httpd_register_uri_handler(mServer, &oscControlGet);
    httpd_register_uri_handler(mServer, &oscControlPost);
  }
  /* If server failed to start, handle will be NULL */
  return err;
}

/* Function for stopping the webserver */
esp_err_t Server::Server::stop_webserver() {
  if (mServer) {
    /* Stop the httpd server */
    httpd_stop(mServer);
    mServer = nullptr;
    return ESP_OK;
  }
  return ESP_ERR_NOT_FOUND;
}

FormForLed Server::Server::mFormForLed = FormForLed();
