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
            HtmlParameterServer(ParameterSet& paramSet = DefaultParameterSet);
            ~HtmlParameterServer();

        private:
            ParameterSet& _paramSet;
        };
    }
}