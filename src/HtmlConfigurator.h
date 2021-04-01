#include "CaptivePortal.h"
#include "ParameterSet.h"
#include "bootstrap.min.h"
#include "HtmlTemplates.h"

namespace esp32
{
    namespace foundation
    {
        class HtmlConfigurator : public CaptivePortal
        {
        public:
            HtmlConfigurator(
                const char* ip = "192.168.3.1",
                ParameterSet& paramSet = DefaultParameterSet);
            ~HtmlConfigurator();

        private:
            ParameterSet& _paramSet;
        };
    }
}