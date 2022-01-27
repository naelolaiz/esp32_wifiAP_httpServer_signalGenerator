#ifndef __NAEL_MY_CPP_LIBRARY_H__
#define __NAEL_MY_CPP_LIBRARY_H__

#include <string>

class NaelCppLibrary
{
public:
    static const char * getForm();
};

class FormForLed
{
    const std::string mTitle = "my form page";
    bool mLed1Value = false;
    bool mLed2Value = false;
    static std::string mHtmlEmptyBody;
    static std::string mHtmlForm;
    std::string mTemporalStringToReturnHtml;
    std::string mExtraText;
public:
    FormForLed(const std::string & pageTitle);
    FormForLed() = default;
    void setLed1Value(bool value);
    void setLed2Value(bool value);
    void setExtraText(const std::string& extraText);
    void clearExtraText();
    bool parseLed1Status(const char * content);
    bool parseLed2Status(const char * content);
    std::string parseToUart(const char * content);

    const std::string & getHtmlPage();

};

#endif // __NAEL_MY_CPP_LIBRARY_H__