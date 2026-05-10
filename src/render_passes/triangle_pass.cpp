#include "triangle_pass.h"

#include <cstdlib>
#include <iostream>

#include "shader.h"

namespace webgrafix {

TrianglePass::TrianglePass(std::string shaderPath) : m_shaderPath(std::move(shaderPath)) {}

void TrianglePass::ensurePipeline(GpuContext& context) {
    if (m_pipeline && m_pipelineFormat == context.m_format) {
        return;
    }

    wgpu::ShaderModule shaderModule = Shader::loadWGSL(context.m_device, m_shaderPath);
    if (!shaderModule) {
        std::cerr << "Failed to compile/load shaders: " << m_shaderPath << "\n";
        std::exit(1);
    }

    wgpu::ColorTargetState colorTargetState{.format = context.m_format};
    wgpu::FragmentState fragmentState{
        .module = shaderModule, .entryPoint = "fragMain", .targetCount = 1, .targets = &colorTargetState};

    wgpu::RenderPipelineDescriptor descriptor{.vertex = {.module = shaderModule, .entryPoint = "vertMain"},
                                              .fragment = &fragmentState};
    m_pipeline = context.m_device.CreateRenderPipeline(&descriptor);
    m_pipelineFormat = context.m_format;
}

void TrianglePass::render(GpuContext& context, FrameContext& frame) {
    ensurePipeline(context);

    wgpu::RenderPassColorAttachment attachment{
        .view = frame.m_colorView, .loadOp = wgpu::LoadOp::Clear, .storeOp = wgpu::StoreOp::Store};

    wgpu::RenderPassDescriptor renderpass{.colorAttachmentCount = 1, .colorAttachments = &attachment};

    wgpu::RenderPassEncoder pass = frame.m_encoder.BeginRenderPass(&renderpass);
    pass.SetPipeline(m_pipeline);
    pass.Draw(3);
    pass.End();
}

void TrianglePass::resize(GpuContext& context, uint32_t width, uint32_t height, bool formatChanged) {
    (void)context;
    (void)width;
    (void)height;

    if (formatChanged) {
        m_pipeline = nullptr;
        m_pipelineFormat = wgpu::TextureFormat::Undefined;
    }
}

std::string_view TrianglePass::getName() const { return "TrianglePass"; }

}  // namespace webgrafix
