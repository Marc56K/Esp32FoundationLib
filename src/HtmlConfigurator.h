#include "CaptivePortal.h"
#include "ParameterSet.h"
#include "bootstrap.min.h"
#include "HtmlTemplates.h"
#include <functional>

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

            void OnCancel(std::function<void(void)> callback);
            void OnApply(std::function<void(ParameterSet&)> callback);

        private:
            ParameterSet& _paramSet;
            std::function<void(void)> _onCancel;
            std::function<void(ParameterSet&)> _onApply;
        };
    }
}