#include "HtmlTemplates.h"
#include <sstream>

namespace esp32
{
    namespace foundation
    {
        String HtmlTemplates::HtmlSpecialChars(const String &str)
        {
            String result;
            for (int i = 0; i < str.length(); i++)
            {
                char c = str[i];
                if (c == '<')
                    result += "&lt;";
                else if (c == '>')
                    result += "&gt;";
                else if (c == '&')
                    result += "&amp;";
                else if (c == '"')
                    result += "&quot;";
                else
                    result += c;
            }
            return result;
        }

        String HtmlTemplates::TextInputField(
            const String &id, 
            const String &label, 
            const String &value, 
            const bool password)
        {
            const String _id = HtmlSpecialChars(id);
            String result;
            result += "<div class=\"form-group\">";
            result += "<label for=\"" + _id + "\">" + HtmlSpecialChars(label) + "</label>";
            result += "<input class=\"form-control\" type=\"" + String(password ? "password" : "text") + "\" id=\"" + _id + "\" name=\"" + _id + "\" ";
            result += "value=\"" + HtmlSpecialChars(value) + "\">";
            result += "</div>";
            return result;
        }

        String HtmlTemplates::CheckboxInputField(
                const String &id, 
                const String &label, 
                const bool &checked)
        {
            const String _id = HtmlSpecialChars(id);
            String result;
            result += R"(<div class="form-check" style="margin-bottom: 10px">)";
            result += R"(<input type="hidden" value="false" name=")" + _id + R"(">)";
            result += R"(<input class="form-check-input" type="checkbox" value="true" id=")" + _id + R"(" name=")" + _id + R"(" )";
            if (checked)
            {
                result += "checked";
            }
            result += R"(>)";
            result += R"(<label class="form-check-label" for=")" + _id + R"(">)" + HtmlSpecialChars(label) + R"(</label>)";
            result += R"(</div>)";
            return result;
        }

        String HtmlTemplates::NumberInputField(
            const String &id, 
            const String &label, 
            const String &value, 
            const String &minValue, 
            const String &maxValue)
        {
            const String _id = HtmlSpecialChars(id);
            String result;
            result += "<div class=\"form-group\">";
            result += "<label for=\"" + _id + "\">" + HtmlSpecialChars(label) + "</label>";
            result += "<input class=\"form-control\" type=\"number\" step=\"any\" id=\"" + _id + "\" name=\"" + _id + "\" ";
            result += "value=\"" + HtmlSpecialChars(value) + "\"";

            if (!minValue.isEmpty())
                result += " min=\"" + HtmlSpecialChars(minValue) + "\"";
            
            if (!maxValue.isEmpty())
                result += " max=\"" + HtmlSpecialChars(maxValue) + "\"";

            result += "></div>";
            return result;
        }

        String HtmlTemplates::ParameterInputField(
            Parameter& p,
            const String &label)
        {
            String s;
            switch (p.Type)
            {
            case ParameterType::PT_STRING:
            {
                s = TextInputField(p.Name, label, p.ToString(), false);
            }
            break;
            case ParameterType::PT_FLOAT:
            {
                auto fp = (FloatParameter *)&p;
                const String minVal = fp->MinValue < -9999999.0f ? String("") : String(fp->MinValue, fp->DecimalPlaces);
                const String maxVal = fp->MaxValue >  9999999.0f ? String("") : String(fp->MaxValue, fp->DecimalPlaces);
                s = NumberInputField(p.Name, label, p.ToString(), minVal, maxVal);
            }
            break;
            case ParameterType::PT_INTEGER:
            {
                auto ip = (IntegerParameter *)&p;
                s = NumberInputField(p.Name, label, p.ToString(), String(ip->MinValue), String(ip->MaxValue));
            }
            break;
            case ParameterType::PT_BOOLEAN:
            {
                auto bp = (BooleanParameter *)&p;
                s = CheckboxInputField(p.Name, label, *bp);
            }
            break;
            }
            return s;
        }
    }
}