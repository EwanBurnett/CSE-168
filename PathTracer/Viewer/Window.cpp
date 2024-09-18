#include "Window.h"
#include "../Utils/Logger.h"

Window::Window()
{
    m_Width = 0;
    m_Height = 0;
    m_GLFWHandle = nullptr;
}

Window::~Window()
{
    if (m_GLFWHandle != nullptr) {
        EDX::Log::Warning("GLFWHandle was not destroyed; Did you call Window::Destroy()?\n");
        glfwDestroyWindow(m_GLFWHandle);
    }

    m_GLFWHandle = nullptr;
}

void Window::Create(const uint16_t width, const uint16_t height, const std::string title)
{
    EDX::Log::Status("Creating Window %s (%d x %d)\n", title.c_str(), width, height);

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); 

    m_Width = width;
    m_Height = height;
    m_GLFWHandle = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
}

void Window::Destroy()
{
    glfwDestroyWindow(m_GLFWHandle);
    m_GLFWHandle = nullptr;
}

const uint32_t Window::Width() const
{
    return m_Width;
}

const uint32_t Window::Height() const
{
    return m_Height;
}

GLFWwindow* Window::GetHandle() const
{
    return m_GLFWHandle;
}
