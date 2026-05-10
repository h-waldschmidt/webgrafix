#pragma once

#include <chrono>
#include <cstdint>
#include <memory>
#include <string>
#include <variant>
#include <vector>

#include "gpu_context.h"
#include "render_passes/render_pass_types.h"
#include "window.h"

namespace webgrafix {

struct AppConfig {
    uint32_t m_width = 0;
    uint32_t m_height = 0;
    std::string m_title = "WebGPU window";
    bool m_vsync = true;
};

class App {
   public:
    App(AppConfig config);
    ~App() = default;

    App(const App&) = delete;
    App& operator=(const App&) = delete;
    App(App&&) = delete;
    App& operator=(App&&) = delete;

    void addRenderPass(const render_pass::RenderPass& pass);

    void init();
    void start();

   private:
    bool configureSurface(bool forceFormatUpdate);
    void initGraphics();
    void renderFrame();
    void onResize(int width, int height);

    // Helper for Emscripten's main loop
    static void mainLoopStep(void* arg);

    AppConfig m_config;
    std::unique_ptr<Window> m_window;
    GpuContext m_context;
    std::vector<render_pass::RenderPass> m_passes;
    std::chrono::steady_clock::time_point m_lastFrameTime;
};

}  // namespace webgrafix
