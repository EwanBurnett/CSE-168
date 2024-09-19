#include "Viewer.h"
#include <GLFW/glfw3.h>
#include "Utils/Logger.h"

constexpr uint32_t FRAMES_IN_FLIGHT = 3u;

void EDX::Viewer::Init()
{
    glfwInit();
    m_Window.Create(640, 480, "Viewer");

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
    m_pImage = nullptr;

    //Initialize Vulkan objects
    {
        m_ImageIdx.resize(FRAMES_IN_FLIGHT);
        m_ImageBuffer = VK_NULL_HANDLE;
        m_ImageBufferSize = 0u;
    }
    CreateCommandObjects();
    CreateSyncPrimitives();

    m_Timer.Start();
}

void EDX::Viewer::Shutdown()
{
    vkDeviceWaitIdle(m_Engine.Device());    //Wait for any GPU work to complete. 
    DestroySyncPrimitives();
    DestroyCommandObjects();

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
    //Compute delta time
    m_Timer.Tick();
    const double dtms = m_Timer.DeltaTime();


    static double acc = 0.0f;
    acc += dtms;
    static uint64_t frameCounter = 0;
    frameCounter++;

    //Fixed update 
    const double fixedUpdateRate = (1.0 / 30.0);
    if (acc >= fixedUpdateRate) {
        //Log::Print("average dtms: %f\n", acc / (double)frameCounter); 
        //If a valid image is present, copy it to the swapchain. 
        if (m_pImage)
        {

            //Copy Image data to a buffer
            if (m_ImageBuffer) {
                uint64_t bufferSize = m_pImage->Size() * sizeof(EDX::Colour);
                m_pImage->Mutex().lock();
                void* pMem = nullptr;
                vmaMapMemory(m_Engine.VMAAllocator(), m_ImageBufferAlloc, &pMem);
                if (pMem) {
                    memcpy(pMem, m_pImage->GetPixels().data(), bufferSize);
                }
                vmaUnmapMemory(m_Engine.VMAAllocator(), m_ImageBufferAlloc);
                m_pImage->Mutex().unlock();
            }
        }

        acc = 0.0f;
        frameCounter = 0;
    }

    BeginFrame();
    //Render 
    {
        const uint64_t idx = m_FrameIdx % FRAMES_IN_FLIGHT;
        VkCommandBuffer& cmd = m_CommandBuffers[idx];

        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.pNext = nullptr;
        beginInfo.pInheritanceInfo = nullptr;
        beginInfo.flags = 0;
        vkBeginCommandBuffer(cmd, &beginInfo);


        if (m_ImageBuffer) {
            VkBufferImageCopy region = {};
            region.bufferOffset = 0;
            region.bufferRowLength =  0;
            region.bufferImageHeight =  0;
            region.imageExtent.width = m_Engine.SwapchainExtents().width;
            region.imageExtent.height = m_Engine.SwapchainExtents().height;
            region.imageExtent.depth = 1;
            region.imageSubresource = {
                VK_IMAGE_ASPECT_COLOR_BIT,
                0,
                0,
                1
            };
            region.imageOffset = {
                0, 
                0, 
                0 
            }; 

            CmdTransitionImageLayout(cmd, m_Engine.SwapchainImages()[m_ImageIdx[idx]], m_Engine.SwapchainImageFormat(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, 0);
            vkCmdCopyBufferToImage(cmd, m_ImageBuffer, m_Engine.SwapchainImages()[m_ImageIdx[idx]], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
            CmdTransitionImageLayout(cmd, m_Engine.SwapchainImages()[m_ImageIdx[idx]], m_Engine.SwapchainImageFormat(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, 1, 0);
        }
        vkEndCommandBuffer(cmd);
    }

    EndFrame();
}

void EDX::Viewer::SetImageHandle(EDX::Image* pImage)
{
    m_pImage = pImage;

    if (pImage == nullptr) {
        return; 
    }

    //If our current buffer is too small, realloc it.
    const uint64_t imageSizeBytes = (m_pImage->Size() * sizeof(EDX::Colour));
    if (m_ImageBufferSize < imageSizeBytes) {
        if (m_ImageBuffer != VK_NULL_HANDLE) {
            m_Engine.DestroyBuffer(m_ImageBuffer, m_ImageBufferAlloc);
        }

        m_ImageBufferSize = imageSizeBytes;

        m_Engine.CreateBuffer(&m_ImageBuffer, &m_ImageBufferAlloc, m_ImageBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_AUTO, VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT);
    }
}

void EDX::Viewer::BeginFrame()
{
    const uint32_t idx = m_FrameIdx % FRAMES_IN_FLIGHT;

    vkAcquireNextImageKHR(m_Engine.Device(), m_Engine.Swapchain(), UINT64_MAX, m_sImageAvailable[idx], VK_NULL_HANDLE, &m_ImageIdx[idx]);
    m_Engine.AllocateCommandBuffers(&m_CommandBuffers[idx], 1, m_CommandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY);

}

void EDX::Viewer::EndFrame()
{
    //Submit pending work to the GPU.
    {
        VkPipelineStageFlags stageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;

        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.pNext = nullptr;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &m_CommandBuffers[m_FrameIdx % FRAMES_IN_FLIGHT];
        submitInfo.pWaitDstStageMask = &stageMask;
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = &m_sImageAvailable[m_FrameIdx % FRAMES_IN_FLIGHT];
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &m_sRenderFinished[m_FrameIdx % FRAMES_IN_FLIGHT];

        vkQueueSubmit(m_Engine.Queue(), 1, &submitInfo, m_fFences[m_FrameIdx % FRAMES_IN_FLIGHT]);
    }
    //Present the Swapchain
    {
        VkSwapchainKHR swapchains[] = { m_Engine.Swapchain() };
        VkPresentInfoKHR presentInfo = {};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.pNext = nullptr;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &m_sRenderFinished[m_FrameIdx % FRAMES_IN_FLIGHT];
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapchains;
        presentInfo.pImageIndices = &m_ImageIdx[m_FrameIdx % FRAMES_IN_FLIGHT];
        presentInfo.pResults = nullptr;

        vkQueuePresentKHR(m_Engine.Queue(), &presentInfo);

        //Wait for the work to complete. 
        vkWaitForFences(m_Engine.Device(), 1, &m_fFences[m_FrameIdx % FRAMES_IN_FLIGHT], VK_TRUE, UINT64_MAX);
        vkResetFences(m_Engine.Device(), 1, &m_fFences[m_FrameIdx % FRAMES_IN_FLIGHT]);
    }

    //Free the command buffer, now that we've finished with it. 
    m_Engine.FreeCommandBuffers(&m_CommandBuffers[m_FrameIdx % FRAMES_IN_FLIGHT], 1, m_CommandPool);
    m_FrameIdx++;
}

void EDX::Viewer::CreateSyncPrimitives()
{
    for (uint32_t i = 0; i < FRAMES_IN_FLIGHT; i++) {
        VkFence fence = VK_NULL_HANDLE;
        m_Engine.CreateFence(&fence, false);
        m_fFences.push_back(fence);

        VkSemaphore s_ImageAvailable = VK_NULL_HANDLE;
        m_Engine.CreateSemaphore(&s_ImageAvailable);
        m_sImageAvailable.push_back(s_ImageAvailable);

        VkSemaphore s_RenderFinished = VK_NULL_HANDLE;
        m_Engine.CreateSemaphore(&s_RenderFinished);
        m_sRenderFinished.push_back(s_RenderFinished);
    }
}

void EDX::Viewer::DestroySyncPrimitives()
{
    for (uint32_t i = 0; i < FRAMES_IN_FLIGHT; i++) {
        m_Engine.DestroySemaphore(m_sRenderFinished[i]);
        m_Engine.DestroySemaphore(m_sImageAvailable[i]);
        m_Engine.DestroyFence(m_fFences[i]);
    }
}

void EDX::Viewer::CreateCommandObjects()
{
    m_Engine.CreateCommandPool(&m_CommandPool, VK_COMMAND_POOL_CREATE_TRANSIENT_BIT, m_Engine.QueueFamilyIndex());

    m_CommandBuffers.resize(FRAMES_IN_FLIGHT);
}

void EDX::Viewer::DestroyCommandObjects()
{
    m_Engine.DestroyCommandPool(m_CommandPool);
}


void EDX::Viewer::CmdTransitionImageLayout(VkCommandBuffer cmd, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t layerCount, uint32_t mipLevels)
{

    VkImageMemoryBarrier barrier = {
        VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        nullptr,
        0,
        0,
        oldLayout,
        newLayout,
        m_Engine.QueueFamilyIndex(),
        m_Engine.QueueFamilyIndex(),
        image,
        {
            VK_IMAGE_ASPECT_COLOR_BIT,
            0,
            1,
            0,
            1,
        }
    };

    VkPipelineStageFlags srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    VkPipelineStageFlags dstStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;

    if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

        if (false) {
            barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
        }
    }
    else {
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    }

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        dstStage - VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        dstStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    }

    vkCmdPipelineBarrier(cmd, srcStage, dstStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
}
