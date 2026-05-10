#pragma once

#include <concepts>
#include <string_view>
#include <utility>
#include <variant>

namespace {
template <typename Pass, typename... Args>
concept HasRender = requires(Pass& pass, Args&&... args) { pass.render(std::forward<Args>(args)...); };

template <typename Pass, typename... Args>
concept HasPreRender = requires(Pass& pass, Args&&... args) { pass.preRender(std::forward<Args>(args)...); };

template <typename Pass, typename... Args>
concept HasPostRender = requires(Pass& pass, Args&&... args) { pass.postRender(std::forward<Args>(args)...); };

template <typename Pass, typename... Args>
concept HasRenderUI = requires(Pass& pass, Args&&... args) { pass.renderUI(std::forward<Args>(args)...); };

template <typename Pass, typename... Args>
concept HasUpdate = requires(Pass& pass, Args&&... args) { pass.update(std::forward<Args>(args)...); };

template <typename Pass, typename... Args>
concept HasResize = requires(Pass& pass, Args&&... args) { pass.resize(std::forward<Args>(args)...); };

template <typename Pass>
concept HasGetName = requires(const Pass& pass) {
    { pass.getName() } -> std::convertible_to<std::string_view>;
};

template <typename Pass, typename... Args>
void callRender(Pass& pass, Args&&... args) {
    static_assert(HasRender<Pass, Args...>, "Render pass must implement render(...)");
    pass.render(std::forward<Args>(args)...);
}

template <typename Pass, typename... Args>
void callPreRender(Pass& pass, Args&&... args) {
    if constexpr (HasPreRender<Pass, Args...>) {
        pass.preRender(std::forward<Args>(args)...);
    }
}

template <typename Pass, typename... Args>
void callPostRender(Pass& pass, Args&&... args) {
    if constexpr (HasPostRender<Pass, Args...>) {
        pass.postRender(std::forward<Args>(args)...);
    }
}

template <typename Pass, typename... Args>
void callRenderUi(Pass& pass, Args&&... args) {
    if constexpr (HasRenderUI<Pass, Args...>) {
        pass.renderUI(std::forward<Args>(args)...);
    }
}

template <typename Pass, typename... Args>
void callUpdate(Pass& pass, Args&&... args) {
    if constexpr (HasUpdate<Pass, Args...>) {
        pass.update(std::forward<Args>(args)...);
    }
}

template <typename Pass, typename... Args>
void callResize(Pass& pass, Args&&... args) {
    if constexpr (HasResize<Pass, Args...>) {
        pass.resize(std::forward<Args>(args)...);
    }
}

template <typename Pass>
std::string_view callGetName(const Pass& pass) {
    if constexpr (HasGetName<Pass>) {
        return pass.getName();
    }
    return "UnnamedPass";
}

template <typename Variant, typename Func>
void visit(Variant& variant, Func&& func) {
    std::visit([&](auto& pass) { std::forward<Func>(func)(pass); }, variant);
}

}  // anonymous namespace

namespace webgrafix {
namespace render_pass {

template <typename Variant, typename... Args>
void render(Variant& variant, Args&&... args) {
    visit(variant, [&](auto& pass) { callRender(pass, std::forward<Args>(args)...); });
}

template <typename Variant, typename... Args>
void preRender(Variant& variant, Args&&... args) {
    visit(variant, [&](auto& pass) { callPreRender(pass, std::forward<Args>(args)...); });
}

template <typename Variant, typename... Args>
void postRender(Variant& variant, Args&&... args) {
    visit(variant, [&](auto& pass) { callPostRender(pass, std::forward<Args>(args)...); });
}

template <typename Variant, typename... Args>
void renderUI(Variant& variant, Args&&... args) {
    visit(variant, [&](auto& pass) { callRenderUi(pass, std::forward<Args>(args)...); });
}

template <typename Variant, typename... Args>
void update(Variant& variant, Args&&... args) {
    visit(variant, [&](auto& pass) { callUpdate(pass, std::forward<Args>(args)...); });
}

template <typename Variant, typename... Args>
void resize(Variant& variant, Args&&... args) {
    visit(variant, [&](auto& pass) { callResize(pass, std::forward<Args>(args)...); });
}

}  // namespace render_pass
}  // namespace webgrafix
