#ifndef __SIGNAL_GENERATOR_CONTROL_HTML_H__
#define __SIGNAL_GENERATOR_CONTROL_HTML_H__
#include <esp_http_server.h>
#include <string>

static const std::string SignalGeneratorForm = "<html>\n \
<head>\n \
<style>\n \
form {\n \
display:\n \
  table;\n \
}\n \
p {\n \
display : table-row;\n \
}\n \
<!--label {display:  table-cell;}-->\n \
label {\n \
width: 110px;\n \
display: inline-block;\n \
}\n \
input {\n \
display: table-cell;\n \
}\n \
</style>\n \
</head>\n \
<body>\n \
<div class=\"rendered-form\">\n \
<form action=\"\"  id=\"form-control-osc1\" method=\"post\">\n \
    <div class=\"formbuilder-select form-group field-select-waveform-osc1\">\n \
    <p>\n \
        <label for=\"select-waveform-osc1\" class=\"formbuilder-select-label\">Wave type</label>\n \
        <select class=\"form-control-osc1\" name=\"select-waveform-osc1\" id=\"select-waveform-osc1\" required=\"required\" aria-required=\"true\">\n \
            <option value=\"off\" selected=\"true\" id=\"select-waveform-osc1-0\">OFF</option>\n \
            <option value=\"sine\" id=\"select-waveform-osc1-1\">SINE</option>\n \
            <option value=\"triangular\" id=\"select-waveform-osc1-2\">TRIANGULAR</option>\n \
            <option value=\"square1\" id=\"select-waveform-osc1-3\">SQUARE1</option>\n \
            <option value=\"square2\" id=\"select-waveform-osc1-4\">SQUARE2</option>\n \
        </select>\n \
    </p>\n \
    </div>\n \
    <div class=\"formbuilder-number form-group field-number-phase-osc1\">\n \
    <p>\n \
        <label for=\"number-phase-osc1\" class=\"formbuilder-number-label\">Starting Phase</label>\n \
        <input type=\"number\" class=\"form-control-osc1\" name=\"number-phase-osc1\" access=\"false\" value=\"0\" min=\"0\" max=\"360\" step=\"0.1\" id=\"number-phase-osc1\" required=\"required\" aria-required=\"true\">\n \
    </p>\n \
    </div>\n \
    <div class=\"formbuilder-number form-group field-number-frequency-osc1\">\n \
    <p>\n \
        <label for=\"number-frequency-osc1\" class=\"formbuilder-number-label\">Frequency</label>\n \
        <input type=\"number\" class=\"form-control-osc1\" name=\"number-frequency-osc1\" access=\"false\" value=\"1000\" min=\"0\" max=\"10000000\" step=\"1\" id=\"number-frequency-osc1\" required=\"required\" aria-required=\"true\">\n \
    </p>\n \
    </div>\n \
    <div class=\"formbuilder-number form-group field-number-gain-osc1\">\n \
    <p>\n \
        <label for=\"number-gain-osc1\" class=\"formbuilder-number-label\">Gain</label>\n \
        <input type=\"number\" class=\"form-control-osc1\" name=\"number-gain-osc1\" access=\"false\" value=\"1.0\" min=\"0\" max=\"1\" step=\"0.1\" id=\"number-gain-osc1\" required=\"required\" aria-required=\"true\">\n \
    </p>\n \
    </div>\n \
    <div class=\"formbuilder-button form-group field-button-submit-osc1\">\n \
        <button type=\"submit\" class=\"btn-default btn\" name=\"button-submit-osc1\" access=\"false\" style=\"default\" id=\"button-submit-osc1\">Send</button>\n \
    </div>\n \
</form>\n \
</div>\n \
</body>\n \
</html>";

#endif // __SIGNAL_GENERATOR_CONTROL_HTML_H__