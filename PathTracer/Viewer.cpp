#include "Viewer.h"
#include <GLFW/glfw3.h>

void EDX::Viewer::Init()
{
    glfwInit(); 
    m_Window.Create(600, 400, "Viewer");
}

void EDX::Viewer::Shutdown()
{
    m_Window.Destroy(); 
    glfwTerminate(); 
}

bool EDX::Viewer::PollEvents()
{
    //If the window or the application wants to close, break. 
    if (glfwWindowShouldClose(m_Window.GetHandle())) {
        return false; 
    }

    //Otherwise, poll any remaining messages. 
    glfwPollEvents(); 
    return true;
}

void EDX::Viewer::Update()
{
}
