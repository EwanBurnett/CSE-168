#include "Viewer.h"
#include <GLFW/glfw3.h>

void EDX::Viewer::Init()
{
    glfwInit(); 
    m_Window.Create(600, 400, "Viewer");

    //Initialize the Vulkan backend
    VkEngineInitInfo initInfo = {};
    {
        initInfo.enableDebugUtils = true;
        initInfo.enableValidationLayers = true;
        //initInfo.requestedDeviceExtensions.push_back(VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME);
        initInfo.requestedDeviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

        uint32_t glfwExtCount = 0;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtCount);

        for (uint32_t i = 0; i < glfwExtCount; i++) {
            initInfo.requestedInstanceExtensions.push_back(glfwExtensions[i]);
        }
    }
    m_Engine.Init(initInfo, &m_Window); 
}

void EDX::Viewer::Shutdown()
{
    m_Engine.Shutdown(); 
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
