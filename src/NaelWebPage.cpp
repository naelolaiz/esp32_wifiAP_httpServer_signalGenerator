#include "NaelWebPage.h"
#include "SignalGeneratorControlForm.h"
#include <regex>

const char *NaelCppLibrary::getForm() { return nullptr; }

std::string FormForLed::mHtmlEmptyBody = "<html>\n\
<head>\n\
<title>TITLE</title>\n\
</head>\n\
<body>\n\
BODY\n\
</body>\n\
</html>\n\
";

std::string FormForLed::mHtmlForm =
    "<form action=\"\" method=\"post\" id=\"form\">\n\
  <input type=\"checkbox\" onchange=\"document.forms.form.submit();\" id=\"led1\" name=\"led1\" class=\"checkbox\" LED1_CHECKED>\n\
  <label for=\"led1\"> Led status </label><br>\n\
  <input type=\"checkbox\" id=\"led2\" name=\"led2\" LED2_CHECKED>\n\
  <label for=\"led2\"> Led status </label><br>\n\
  <input type=\"submit\" value=\"Submit\">\n\
  <!-- <input type=\"text\" id=\"lname\" name=\"lname\">\n -->\
  <label for=\"toUart\">Send to Uart:</label><br>\n \
  <input type=\"text\" id=\"toUart\" name=\"toUart\"><br>\n \
  <input id=\"dummyId\" name=\"dummyId\" type=\"hidden\" value=\"lalalaa\">\n\
</form>\n <br/>";
// See
// https://stackoverflow.com/questions/18135420/jquery-javascript-submit-form-when-checkbox-is-clicked
// https://forums.phpfreaks.com/topic/162996-auto-submit-form-on-selecting-checkbox/
// https://stackoverflow.com/questions/2435525/best-practice-access-form-elements-by-html-id-or-name-attribute
FormForLed::FormForLed(const std::string &pageTitle) : mTitle(pageTitle) {}
void FormForLed::setLed1Value(bool value) { mLed1Value = value; }

void FormForLed::setLed2Value(bool value) { mLed2Value = value; }

void FormForLed::setExtraText(const std::string &extraText) {
  mExtraText = extraText;
}

void FormForLed::clearExtraText() { mExtraText.clear(); }

bool FormForLed::parseLed1Status(const char *content) {
  return std::regex_search(content, std::regex("led1=on"));
}

bool FormForLed::parseLed2Status(const char *content) {
  return std::regex_search(content, std::regex("led2=on"));
}

std::string FormForLed::parseToUart(const char *content) {
  std::string temp(content); // TODO : avoid copy
  std::smatch m;
  if (std::regex_match(temp, m, std::regex("toUart=(.*)&"))) {
    return m[1];
  }
  return "";
}

const std::string &FormForLed::getHtmlPage() {
  const std::string body =
      std::regex_replace(
          std::regex_replace(mHtmlForm, std::regex("LED1_CHECKED"),
                             (mLed1Value ? "checked" : "")),
          std::regex("LED2_CHECKED"), (mLed2Value ? "checked" : "")) +
      mExtraText;

  mTemporalStringToReturnHtml = std::regex_replace(
      std::regex_replace(mHtmlEmptyBody, std::regex("TITLE"), mTitle),
      std::regex("BODY"), body);
  return mTemporalStringToReturnHtml;
}

const std::string &FormForLed::getOscControlPage() {
  return SignalGeneratorForm;
}