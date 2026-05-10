#include "window.h"

namespace webgrafix {

Window::Window(const WindowConfig& config) {
    if (!glfwInit()) {
        return;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    m_window = glfwCreateWindow(static_cast<int>(config.m_width), static_cast<int>(config.m_height),
                                config.m_title.c_str(), nullptr, nullptr);

    if (m_window) {
        glfwSetWindowUserPointer(m_window, this);
        glfwSetFramebufferSizeCallback(m_window, framebufferSizeCallback);
    }
}

Window::~Window() {
    if (m_window) {
        glfwDestroyWindow(m_window);
    }
    glfwTerminate();
}

bool Window::isValid() const { return m_window != nullptr; }

GLFWwindow* Window::getHandle() const { return m_window; }

void Window::pollEvents() const { glfwPollEvents(); }

bool Window::shouldClose() const { return m_window ? glfwWindowShouldClose(m_window) != 0 : true; }

void Window::getFramebufferSize(int& width, int& height) const {
    if (!m_window) {
        width = 0;
        height = 0;
        return;
    }

    glfwGetFramebufferSize(m_window, &width, &height);
}

void Window::setResizeCallback(std::function<void(int, int)> callback) { m_onResize = std::move(callback); }

void Window::framebufferSizeCallback(GLFWwindow* windowHandle, int width, int height) {
    auto* self = static_cast<Window*>(glfwGetWindowUserPointer(windowHandle));
    if (self && self->m_onResize) {
        self->m_onResize(width, height);
    }
}

}  // namespace webgrafix
