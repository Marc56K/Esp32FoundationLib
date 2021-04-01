#include "StringUtils.h"
#include <sstream>

namespace esp32
{
    namespace foundation
    {
        bool StringUtils::SplitKeyValue(const String &input, const String &delimiter, String &key, String &value)
        {
            int idx = input.indexOf(delimiter);
            if (idx > -1)
            {
                key = input.substring(0, idx);
                value = (idx + 1 < input.length()) ? input.substring(idx + 1) : "";
                return true;
            }

            key = input;
            value = "";
            return false;
        }

        String StringUtils::PrependZeros(const String& input, const int totalLength)
        {
            String result = input;
            while (result.length() < totalLength)
            {
                result = "0" + result;
            }
            return result;
        }
    }
}