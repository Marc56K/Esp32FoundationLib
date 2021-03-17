#include "HtmlParameterServer.h"
#include "html/ConfigCompleted.html"
#include "html/ConfigCanceled.html"
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
                sv.sendContent(R"(<!DOCTYPE HTML><head><meta charset="UTF-8"><meta name="viewport" content="width=device-width, initial-scale=1.0,">)");
                sv.sendContent(R"(<head>)");

                sv.sendContent(R"(<style>)");
                sv.sendContent_P(bootstrap_css);
                sv.sendContent(R"(@media screen and (min-width: 500px) { body form { width: 500px; margin: 0 auto; }} )");
                sv.sendContent(R"(button { margin-left: 10px; width: 100px } )");
                sv.sendContent(R"(form { background: rgb(250, 250, 250); padding: 20px 10px; border-radius: 10px; } )");
                sv.sendContent(R"(</style>)");

                sv.sendContent(R"(</head><body>)");
                sv.sendContent("\n");
                sv.sendContent(R"(<form action="/apply" method="post">)");

                for (auto &param : _paramSet.GetParameters())
                {
                    if (!param.second->IsHidden())
                    {
                        const String s = HtmlTemplates::ParameterInputField(*param.second, param.second->GetDisplayName());

                        if (!s.isEmpty())
                        {
                            sv.sendContent("\n");
                            sv.sendContent(s);
                            sv.sendContent("<hr>");
                        }
                    }
                }
                sv.sendContent("\n");
                sv.sendContent(R"(<div style="display: flex; justify-content: flex-end">)");
                sv.sendContent(R"(<button type="button" class="btn btn-danger" onclick="window.location.href = './cancel'">Cancel</button>)");
                sv.sendContent(R"(<button type="submit" class="btn btn-success">Apply</button>)");
                sv.sendContent(R"(</div>)");
                sv.sendContent(R"(</form>)");
                sv.sendContent(R"(</body></html>)");

                // Stop is needed because we sent no content length
                sv.client().stop();
            });

            On("/cancel", [&](WebServer &sv)
            {
                Serial.println("### cancel");

                sv.setContentLength(CONTENT_LENGTH_UNKNOWN);
                sv.send(200, "text/html", "");                
                sv.sendContent(ConfigCanceledHtml);
                sv.client().stop();

                delay(2000);
                Stop();
            });

            On("/apply", [&](WebServer &sv)
            {
                std::map<String, String> postParams;
                for (int i = 0; i < sv.args(); i++)
                {
                    // takes the last version of each post-parameter
                    postParams[sv.argName(i)] = sv.arg(i);
                }

                for (auto &param : _paramSet.GetParameters())
                {
                    auto it = postParams.find(param.first);
                    if (it != postParams.end())
                    {
                        param.second->SetFromString(it->second);
                    }
                }

                _paramSet.SaveToEEPROM();

                sv.setContentLength(CONTENT_LENGTH_UNKNOWN);
                sv.send(200, "text/html", "");                
                sv.sendContent(ConfigCompletedHtml);
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