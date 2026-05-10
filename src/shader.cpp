#include "shader.h"

#include <fstream>
#include <iostream>
#include <sstream>

namespace webgrafix {

wgpu::ShaderModule Shader::loadWGSL(const wgpu::Device& device, const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open shader file: " << filepath << "\n";
        return nullptr;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    wgpu::ShaderSourceWGSL wgsl;
    wgsl.code = source.c_str();

    wgpu::ShaderModuleDescriptor descriptor;
    descriptor.nextInChain = &wgsl;

    return device.CreateShaderModule(&descriptor);
}

}  // namespace webgrafix
