#pragma once

#include <webgpu/webgpu_cpp.h>

#include <string>
#include <string_view>

#include "gpu_context.h"

namespace webgrafix {

class TrianglePass {
   public:
    explicit TrianglePass(std::string shaderPath);

    void render(GpuContext& context, FrameContext& frame);
    void resize(GpuContext& context, uint32_t width, uint32_t height, bool formatChanged);

    std::string_view getName() const;

   private:
    void ensurePipeline(GpuContext& context);

    std::string m_shaderPath;
    wgpu::RenderPipeline m_pipeline;
    wgpu::TextureFormat m_pipelineFormat = wgpu::TextureFormat::Undefined;
};

}  // namespace webgrafix
