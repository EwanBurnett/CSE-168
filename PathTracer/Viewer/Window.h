#pragma once
#include <GLFW/glfw3.h>
#include <string>

class Window {
public:
    Window();
    ~Window();

    void Create(const uint16_t width, const uint16_t height, const std::string title);
    void Destroy();

    const uint32_t Width() const;
    const uint32_t Height() const;

    GLFWwindow* GetHandle() const;
private:
    GLFWwindow* m_GLFWHandle;
    uint32_t m_Width;
    uint32_t m_Height;
};


