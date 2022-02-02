#include "HttpServer.h"
#include "NaelWebPage.h"
#include <array>

extern const uint8_t src_http_server_siggen_form_html_start[] asm(
    "_binary_siggen_form_html_start");
extern const uint8_t
    src_http_server_siggen_form_html_end[] asm("_binary_siggen_form_html_end");

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

  static float getGain(const char *content) {
    static const std::string id = "number-gain-osc1";
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

  static ESP_AD9833::mode_t getWaveForm(const char *content) {
    static const std::string id = "select-waveform-osc1";
    const auto modeStr = getStdString(content, id.c_str());
    if (modeStr.compare("off") == 0) {
      return ESP_AD9833::mode_t::MODE_OFF;
    } else if (modeStr.compare("sine") == 0) {
      return ESP_AD9833::mode_t::MODE_SINE;
    } else if (modeStr.compare("triangular") == 0) {
      return ESP_AD9833::mode_t::MODE_TRIANGLE;
    } else if (modeStr.compare("square1") == 0) {
      return ESP_AD9833::mode_t::MODE_SQUARE1;
    } else if (modeStr.compare("square2") == 0) {
      return ESP_AD9833::mode_t::MODE_SQUARE2;
    } else {
      throw std::runtime_error("WTF?");
    }
  }
};
} // namespace

Server::Server(std::optional<std::shared_ptr<AD9833Manager::SigGenOrchestrator>>
                   sigGenOrchestrator)
    : mSigGenOrchestrator(sigGenOrchestrator) {
  ESP_LOGI("loloele", "this: %p", this);
}

/* Our URI handler function to be called during GET /uri request */
esp_err_t Server::Server::get_handler(httpd_req_t *req) {
  FormForLed &formForLed = static_cast<Server *>(req->user_ctx)->mFormForLed;
  if (strcmp(req->uri, "/form") == 0) {
    formForLed.clearExtraText();
    httpd_resp_send(req, formForLed.getHtmlPage().c_str(),
                    HTTPD_RESP_USE_STRLEN);
  }
  if (strcmp(req->uri, "/siggen") == 0) {
    httpd_resp_send(
        req,
        reinterpret_cast<const char *>(src_http_server_siggen_form_html_start),
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
  std::array<char, 200> content;
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

    Misc::OnBoardLedManager::setRequestedValue(led1);
    mFormForLed.setLed1Value(led1);
    mFormForLed.setLed2Value(mFormForLed.parseLed2Status(content.data()));
    httpd_resp_send(req, mFormForLed.getHtmlPage().c_str(),
                    HTTPD_RESP_USE_STRLEN);

    // ESP_LOGI(TAG, "%s", mFormForLed.parseToUart(content.data()).c_str());
  } else if (strcmp(req->uri, "/siggen") == 0) {

    ESP_LOGI("lololo", "before cast , %p", req->user_ctx);
    auto serverInstance = static_cast<Server *>(req->user_ctx);

    if (serverInstance->mSigGenOrchestrator.has_value()) {
      serverInstance->mSigGenOrchestrator.value()->pushRequest(
          ESP_AD9833::ESP_AD9833::channel_t::CHAN_0 /*TODO*/,
          ParseRequests::getFrequency(content.data()),
          ParseRequests::getPhase(content.data()),
          ParseRequests::getWaveForm(content.data()),
          ParseRequests::getGain(content.data()) * 100);
    }

    httpd_resp_send(
        req,
        reinterpret_cast<const char *>(src_http_server_siggen_form_html_start),
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
