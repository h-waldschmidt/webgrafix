#include "app.h"

#include <dawn/webgpu_cpp_print.h>

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <utility>

#if defined(__EMSCRIPTEN__)
#include <emscripten/emscripten.h>
#endif
#include <webgpu/webgpu_glfw.h>

#include "render_passes/render_pass.h"

namespace webgrafix {

App::App(AppConfig config) : m_config(std::move(config)) {}

void App::addRenderPass(const render_pass::RenderPass& pass) { m_passes.push_back(pass); }

void App::init() {
    wgpu::InstanceDescriptor instanceDesc{.capabilities = {.timedWaitAnyEnable = true}};
    m_context.m_instance = wgpu::CreateInstance(&instanceDesc);

    wgpu::Future requestAdapter = m_context.m_instance.RequestAdapter(
        nullptr, wgpu::CallbackMode::WaitAnyOnly,
        [this](wgpu::RequestAdapterStatus status, wgpu::Adapter adapter, wgpu::StringView message) {
            if (status != wgpu::RequestAdapterStatus::Success) {
                std::cout << "RequestAdapter: " << message << "\n";
                std::exit(0);
            }
            m_context.m_adapter = std::move(adapter);
        });
    m_context.m_instance.WaitAny(requestAdapter, UINT64_MAX);

    wgpu::DeviceDescriptor desc{};
    desc.SetDeviceLostCallback(wgpu::CallbackMode::AllowSpontaneous,
                               [](const wgpu::Device&, wgpu::DeviceLostReason reason, wgpu::StringView message) {
                                   std::cout << "Device lost: " << reason << " - message: " << message << "\n";
                               });
    desc.SetUncapturedErrorCallback([](const wgpu::Device&, wgpu::ErrorType errorType, wgpu::StringView message) {
        std::cout << "Error: " << errorType << " - message: " << message << "\n";
    });

    wgpu::Future requestDevice = m_context.m_adapter.RequestDevice(
        &desc, wgpu::CallbackMode::WaitAnyOnly,
        [this](wgpu::RequestDeviceStatus status, wgpu::Device device, wgpu::StringView message) {
            if (status != wgpu::RequestDeviceStatus::Success) {
                std::cout << "RequestDevice: " << message << "\n";
                std::exit(0);
            }
            m_context.m_device = std::move(device);
        });
    m_context.m_instance.WaitAny(requestDevice, UINT64_MAX);
    m_context.m_queue = m_context.m_device.GetQueue();
}

bool App::configureSurface(bool forceFormatUpdate) {
    const auto previousFormat = m_context.m_format;

    if (forceFormatUpdate || m_context.m_format == wgpu::TextureFormat::Undefined) {
        wgpu::SurfaceCapabilities capabilities;
        m_context.m_surface.GetCapabilities(m_context.m_adapter, &capabilities);
        if (capabilities.formatCount > 0) {
            m_context.m_format = capabilities.formats[0];
        }
    }

    wgpu::SurfaceConfiguration surfaceConfig{.device = m_context.m_device,
                                             .format = m_context.m_format,
                                             .width = m_context.m_width,
                                             .height = m_context.m_height,
                                             .presentMode = m_context.m_presentMode};
    m_context.m_surface.Configure(&surfaceConfig);

    return previousFormat != m_context.m_format;
}

void App::initGraphics() {
    const bool formatChanged = configureSurface(true);
    for (auto& pass : m_passes) {
        render_pass::resize(pass, m_context, m_context.m_width, m_context.m_height, formatChanged);
    }
}

void App::renderFrame() {
    const auto now = std::chrono::steady_clock::now();
    const double dtSeconds = std::chrono::duration<double>(now - m_lastFrameTime).count();
    m_lastFrameTime = now;

    for (auto& pass : m_passes) {
        render_pass::update(pass, m_context, dtSeconds);
    }

    wgpu::SurfaceTexture surfaceTexture;
    m_context.m_surface.GetCurrentTexture(&surfaceTexture);
    wgpu::TextureView view = surfaceTexture.texture.CreateView();

    FrameContext frame{
        .m_encoder = m_context.m_device.CreateCommandEncoder(),
        .m_colorView = view,
    };

    for (auto& pass : m_passes) {
        render_pass::preRender(pass, m_context, frame);
    }

    for (auto& pass : m_passes) {
        render_pass::render(pass, m_context, frame);
    }

    for (auto& pass : m_passes) {
        render_pass::postRender(pass, m_context, frame);
    }

    for (auto& pass : m_passes) {
        render_pass::renderUI(pass, m_context, frame);
    }

    wgpu::CommandBuffer commands = frame.m_encoder.Finish();
    m_context.m_queue.Submit(1, &commands);
    m_context.m_surface.Present();
}

void App::onResize(int width, int height) {
    if (width <= 0 || height <= 0) {
        return;
    }

    m_context.m_width = static_cast<uint32_t>(width);
    m_context.m_height = static_cast<uint32_t>(height);

    const auto previousFormat = m_context.m_format;
    configureSurface(false);
    const bool formatChanged = previousFormat != m_context.m_format;

    for (auto& pass : m_passes) {
        render_pass::resize(pass, m_context, m_context.m_width, m_context.m_height, formatChanged);
    }
}

void App::mainLoopStep(void* arg) {
    App* app = static_cast<App*>(arg);
    app->m_window->pollEvents();
    app->renderFrame();
#if !defined(__EMSCRIPTEN__)
    app->m_context.m_instance.ProcessEvents();
#endif
}

void App::start() {
    WindowConfig windowConfig{.m_width = m_config.m_width, .m_height = m_config.m_height, .m_title = m_config.m_title};
    m_window = std::make_unique<Window>(windowConfig);
    if (!m_window->isValid()) {
        return;
    }

    m_window->setResizeCallback([this](int width, int height) { onResize(width, height); });

    m_context.m_presentMode = m_config.m_vsync ? wgpu::PresentMode::Fifo : wgpu::PresentMode::Immediate;
    m_context.m_surface = wgpu::glfw::CreateSurfaceForWindow(m_context.m_instance, m_window->getHandle());

    int framebufferWidth = 0;
    int framebufferHeight = 0;
    m_window->getFramebufferSize(framebufferWidth, framebufferHeight);
    onResize(framebufferWidth, framebufferHeight);

    initGraphics();
    m_lastFrameTime = std::chrono::steady_clock::now();

#if defined(__EMSCRIPTEN__)
    emscripten_set_main_loop_arg(mainLoopStep, this, 0, false);
#else
    while (!m_window->shouldClose()) {
        mainLoopStep(this);
    }
#endif
}

}  // namespace webgrafix
