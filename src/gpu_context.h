#pragma once

#include <webgpu/webgpu_cpp.h>

#include <cstdint>

namespace webgrafix {

struct FrameContext {
    wgpu::CommandEncoder m_encoder;
    wgpu::TextureView m_colorView;
};

struct GpuContext {
    wgpu::Instance m_instance;
    wgpu::Adapter m_adapter;
    wgpu::Device m_device;
    wgpu::Queue m_queue;
    wgpu::Surface m_surface;
    wgpu::TextureFormat m_format = wgpu::TextureFormat::Undefined;
    wgpu::PresentMode m_presentMode = wgpu::PresentMode::Fifo;
    uint32_t m_width = 0;
    uint32_t m_height = 0;
};

}  // namespace webgrafix
