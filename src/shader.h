#pragma once

#include <webgpu/webgpu_cpp.h>

#include <string>

namespace webgrafix {

class Shader {
   public:
    static wgpu::ShaderModule loadWGSL(const wgpu::Device& device, const std::string& filepath);
};

}  // namespace webgrafix
