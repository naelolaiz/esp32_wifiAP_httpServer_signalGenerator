#include "NaelCppLibrary.h"

const char * NaelCppLibrary::getForm()
{
    return nullptr;
}


std::string FormForLed::mHtmlEmptyBody = "<html>\n\
<head>\n\
<title>TITLE</title>\n\
</head>\n\
<body>\n\
BODY\n\
</body>\n\
</html>\n\
";

std::string FormForLed::mHtmlForm = "<form action=\"/form\" method=\"post\">\n\
  <input type=\"checkbox\" id=\"led1\" name=\"led1\" LED1_CHECKED>\n\
  <label for=\"led1\"> Led status </label><br>\n\
  <input type=\"checkbox\" id=\"led2\" name=\"led2\" LED2_CHECKED>\n\
  <label for=\"led2\"> Led status </label><br>\n\
  <input type=\"submit\" value=\"Submit\">\n\
  <input type=\"text\" id=\"lname\" name=\"lname\">\n\
</form>\n <br/>";
FormForLed::FormForLed(const std::string & pageTitle)
: mTitle(pageTitle)
{}
void FormForLed::setLed1Value(bool value)
{
    mLed1Value = value;
}

void FormForLed::setLed2Value(bool value)
{
    mLed2Value = value;
}

void FormForLed::setExtraText(const std::string& extraText)
{
    mExtraText = extraText;
}

const std::string & FormForLed::getHtmlPage()
{
    const std::string body = std::regex_replace(std::regex_replace(mHtmlForm, std::regex("LED1_CHECKED"), (mLed1Value ? "checked" : "")),
                                                                        std::regex("LED2_CHECKED"), (mLed2Value ? "checked" : ""))
                             + std::to_string(mExtraText.size());

    mTemporalStringToReturnHtml = std::regex_replace(std::regex_replace(mHtmlEmptyBody, std::regex("TITLE"), mTitle),
                                                                                        std::regex("BODY"), body); 
    return mTemporalStringToReturnHtml;
}
