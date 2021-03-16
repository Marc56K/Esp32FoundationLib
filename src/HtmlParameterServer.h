#include "CaptivePortal.h"
#include "ParameterSet.h"
#include "bootstrap.min.h"
#include "HtmlTemplates.h"

namespace esp32
{
    namespace foundation
    {
        class HtmlParameterServer : public CaptivePortal
        {
        public:
            HtmlParameterServer(
                const char* ip = "192.168.3.1",
                ParameterSet& paramSet = DefaultParameterSet);
            ~HtmlParameterServer();

        private:
            ParameterSet& _paramSet;
        };
    }
}