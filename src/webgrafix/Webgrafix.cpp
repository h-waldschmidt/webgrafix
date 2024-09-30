#include "Webgrafix.h"

#include <spdlog/spdlog.h>
#include <webgpu/webgpu_cpp.h>

#include <stdexcept>

namespace webgrafix {
void Webgrafix::setup() {
    wgpu::InstanceDescriptor instance_descriptor;
    instance_descriptor.features.timedWaitAnyEnable = true;
    wgpu::Instance instance = wgpu::CreateInstance(&instance_descriptor);
    if (instance == nullptr) {
        throw std::runtime_error("Couldn't create instance!");
    }

    // Synchronously request the adapter.
    wgpu::RequestAdapterOptions options = {};
    wgpu::Adapter adapter;
    wgpu::RequestAdapterCallbackInfo callback_info = {};
    callback_info.nextInChain = nullptr;
    callback_info.mode = wgpu::CallbackMode::WaitAnyOnly;
    callback_info.callback = [](WGPURequestAdapterStatus status, WGPUAdapter adapter, const char *message,
                                void *userdata) {
        if (status != WGPURequestAdapterStatus_Success) {
            std::string prefix = "Failed to get an adapter: ";
            throw std::runtime_error(prefix + message);
        }
        *static_cast<wgpu::Adapter *>(userdata) = wgpu::Adapter::Acquire(adapter);
    };

    callback_info.userdata = &adapter;
    instance.WaitAny(instance.RequestAdapter(&options, callback_info), UINT64_MAX);
    if (adapter == nullptr) {
        throw std::runtime_error("RequestAdapter failed");
    }

    wgpu::DawnAdapterPropertiesPowerPreference power_props{};

    wgpu::AdapterInfo info{};
    info.nextInChain = &power_props;

    adapter.GetInfo(&info);
    spdlog::info("VendorID: {}", info.vendorID);
    spdlog::info("Vendor: {}", info.vendor);
    spdlog::info("Architecture: {}", info.architecture);
    spdlog::info("DeviceID: {}", info.deviceID);
    spdlog::info("Name: {}", info.device);
    spdlog::info("Driver description: {}", info.description);
}
}  // namespace webgrafix
