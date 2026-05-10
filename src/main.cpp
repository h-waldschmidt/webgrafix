#include "app.h"

int main() {
    webgrafix::AppConfig config{
        .m_width = 800,
        .m_height = 600,
        .m_title = "WebGPU window",
        .m_vsync = true,
    };

    webgrafix::App app(config);
    app.addRenderPass(webgrafix::TrianglePass("shaders/main.wgsl"));

    app.init();
    app.start();
    return 0;
}