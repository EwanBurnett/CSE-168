#ifndef __VIEWER_H
#define __VIEWER_H

#include "Viewer/Window.h"
#include "Viewer/vkengine.h"
#include "Utils/Timer.h"
#include "Image.h"
#include <vector>

namespace EDX {
    class Viewer {
    public:
        void Init(); 
        void Shutdown(); 

        bool PollEvents(); 
        void Update();

        void SetImageHandle(EDX::Image* pImage); 
    private:
        void BeginFrame(); 
        void EndFrame(); 

        void CreateSyncPrimitives(); 
        void DestroySyncPrimitives(); 

        void CreateCommandObjects(); 
        void DestroyCommandObjects(); 

        void CmdTransitionImageLayout(VkCommandBuffer cmd, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t layerCount, uint32_t mipLevels); 
    private:
        Window m_Window; 
        VkEngine m_Engine; 

        Timer m_Timer; 
        uint64_t m_FrameIdx; 
        Image* m_pImage; 

        std::vector<VkFence> m_fFences; 
        std::vector<VkSemaphore> m_sImageAvailable;
        std::vector<VkSemaphore> m_sRenderFinished; 

        std::vector<uint32_t> m_ImageIdx; 
        
        VkCommandPool m_CommandPool;
        std::vector<VkCommandBuffer> m_CommandBuffers; 


        VkBuffer m_ImageBuffer;
        VkDeviceSize m_ImageBufferSize; 
        VmaAllocation m_ImageBufferAlloc; 

        VkImage m_Image;
        VkImageView m_ImageView;
        VmaAllocation m_ImageAlloc; 
    };
}

#endif