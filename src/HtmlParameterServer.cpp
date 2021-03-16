#include "HtmlParameterServer.h"

namespace esp32
{
    namespace foundation
    {
        HtmlParameterServer::HtmlParameterServer(const char* ip, ParameterSet &paramSet)
            : CaptivePortal(ip), _paramSet(paramSet)
        {
            On("/", [&](WebServer &sv)
            {
                sv.setContentLength(CONTENT_LENGTH_UNKNOWN);
                sv.send(200, "text/html", "");
                sv.sendContent("<!DOCTYPE HTML><html lang=\"de\"><head><meta charset=\"UTF-8\"><meta name= viewport content=\"width=device-width, initial-scale=1.0,\">");
                sv.sendContent("<head>");

                sv.sendContent("<style>");
                sv.sendContent_P(bootstrap_css);
                sv.sendContent("@media screen and (min-width: 600px) { body form { width: 600px; margin: 0 auto; }}");
                sv.sendContent("</style>");

                sv.sendContent("</head><body style=\"padding: 5px;\">");
                sv.sendContent("<form action=\"/save\" method=\"post\">");

                for (auto &param : _paramSet.GetParameters())
                {
                    String s = HtmlTemplates::ParameterInputField(*param.second, param.second->Name);

                    if (!s.isEmpty())
                    {
                        sv.sendContent(s);
                    }
                }

                sv.sendContent("<button type=\"submit\" class=\"btn btn-primary\">Save</button>");
                sv.sendContent("</form>");
                sv.sendContent("</body></html>");

                // Stop is needed because we sent no content length
                sv.client().stop();
            });

            On("/save", [&](WebServer &sv)
            {
                for (auto &param : _paramSet.GetParameters())
                {
                    if (sv.hasArg(param.first))
                    {
                        auto value = sv.arg(param.first);
                        param.second->SetFromString(value);
                    }
                }

                _paramSet.SaveToEEPROM();

                sv.setContentLength(CONTENT_LENGTH_UNKNOWN);
                sv.send(200, "text/html", "configuration completed");
                sv.client().stop();

                delay(2000);

                Stop();
            });
        }

        HtmlParameterServer::~HtmlParameterServer()
        {
        }
    }
}