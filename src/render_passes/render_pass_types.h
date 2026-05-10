#pragma once

#include <variant>

#include "render_passes/triangle_pass.h"

namespace webgrafix {
namespace render_pass {
using RenderPass = std::variant<TrianglePass>;
}  // namespace render_pass
}  // namespace webgrafix
