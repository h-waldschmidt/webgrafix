#pragma once

#include <GLFW/glfw3.h>

#include <cstdint>
#include <functional>
#include <string>

namespace webgrafix {

struct WindowConfig {
    uint32_t m_width = 0;
    uint32_t m_height = 0;
    std::string m_title;
};

class Window {
   public:
    explicit Window(const WindowConfig& config);
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    Window(Window&&) = delete;
    Window& operator=(Window&&) = delete;

    bool isValid() const;
    GLFWwindow* getHandle() const;

    void pollEvents() const;
    bool shouldClose() const;
    void getFramebufferSize(int& width, int& height) const;

    void setResizeCallback(std::function<void(int, int)> callback);

   private:
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);

    GLFWwindow* m_window = nullptr;
    std::function<void(int, int)> m_onResize;
};

}  // namespace webgrafix
