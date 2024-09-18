#pragma once
#include <vulkan/vulkan.h>
#include <vector> 
#include <GLFW/glfw3.h>
#include <vk_mem_alloc.h>

class Window;

struct VkEngineInitInfo {
    std::vector<const char*> requestedInstanceExtensions;
    std::vector<const char*> requestedInstanceLayers;
    std::vector<const char*> requestedDeviceExtensions;

    const char* appName = "Vulkan Application";
    uint32_t appVersion = VK_MAKE_API_VERSION(1, 1, 0, 0);
    const char* engineName = "Vulkan Engine";
    uint32_t engineVersion = VK_MAKE_API_VERSION(1, 1, 0, 0);
    uint32_t apiVersion = VK_API_VERSION_1_0;

    VkPhysicalDeviceFeatures requiredFeatures = {};

    bool enableValidationLayers = false;
    bool enableDebugUtils = false;
};

class VkEngine {
public:
    VkEngine();
    ~VkEngine();

    void Init(const VkEngineInitInfo& initInfo, const Window* pWindow);
    void Shutdown();

    VkInstance Instance() const;
    VkPhysicalDevice PhysicalDevice() const;
    VkDevice Device() const;
    VkSwapchainKHR Swapchain() const;
    const VkExtent2D SwapchainExtents() const; 
    const VkFormat SwapchainImageFormat() const; 
    const std::vector<VkImage> SwapchainImages() const; 
    const std::vector<VkImageView> SwapchainImageViews() const;
    VmaAllocator VMAAllocator() const; 
    
    VkSurfaceKHR Surface() const; 

    VkQueue Queue() const; 
    const uint32_t QueueFamilyIndex() const; 

    static void GLFWFramebufferSizeCallback(GLFWwindow* window, int width, int height);
    void RecreateSwapchain(GLFWwindow* window, int width, int height);

    //Resource Creation / Destruction Helpers

    VkResult CreateSemaphore(VkSemaphore* pSemaphore) const;
    void DestroySemaphore(VkSemaphore& semaphore) const;

    VkResult CreateFence(VkFence* pFence, bool createSignaled) const;
    void DestroyFence(VkFence& fence) const;

    VkResult CreateRenderPass(VkRenderPass* pRenderPass, std::vector<VkAttachmentDescription> attachments, std::vector<VkSubpassDependency> dependencies, std::vector<VkSubpassDescription> subpasses) const; 
    void DestroyRenderPass(VkRenderPass& renderPass) const; 


    VkResult CreateShaderModule(VkShaderModule* pShaderModule, std::vector<char> blob) const; 
    void DestroyShaderModule(VkShaderModule& shaderModule) const; 

    VkResult CreateGraphicsPipelines(VkPipeline* pGraphicsPipelines, std::vector<VkGraphicsPipelineCreateInfo> createInfos) const;
    void DestroyPipeline(VkPipeline& pipeline) const;


    VkResult CreatePipelineLayout(VkPipelineLayout* pPipelineLayout, std::vector<VkDescriptorSetLayout> setLayouts, std::vector<VkPushConstantRange> pushConstantRanges) const; 
    void DestroyPipelineLayout(VkPipelineLayout& pipelineLayout) const; 


    VkResult CreateFramebuffer(VkFramebuffer* pFrameBuffer, VkRenderPass renderPass, std::vector<VkImageView> attachments, VkExtent2D extents, uint32_t layers) const; 
    void DestroyFramebuffer(VkFramebuffer& frameBuffer) const; 

    VkResult CreateCommandPool(VkCommandPool* pCommandPool, VkCommandPoolCreateFlags flags, uint32_t queueFamilyIndex) const; 
    void DestroyCommandPool(VkCommandPool& commandPool) const; 

    VkResult AllocateCommandBuffers(VkCommandBuffer* pCommandBuffers, const uint32_t count, VkCommandPool commandPool, VkCommandBufferLevel level) const; 
    void FreeCommandBuffers(VkCommandBuffer* pCommandBuffers, const uint32_t count, VkCommandPool commandPool) const; 

    VkResult CreateDescriptorSetLayout(VkDescriptorSetLayout* pLayout, std::vector<VkDescriptorSetLayoutBinding> bindings) const; 
    void DestroyDescriptorSetLayout(VkDescriptorSetLayout& layout) const; 

    VkResult CreateDescriptorPool(VkDescriptorPool* pDescriptorPool, const uint32_t maxSets, std::vector<VkDescriptorPoolSize> poolSizes) const;
    void DestroyDescriptorPool(VkDescriptorPool& descriptorPool) const;

    VkResult AllocateDescriptorSets(VkDescriptorSet* pDescriptorSets, const uint32_t count, VkDescriptorSetLayout* pSetLayouts, VkDescriptorPool descriptorPool) const; 
    void FreeDescriptorSets(VkDescriptorSet* pDescriptorSets, const uint32_t count, VkDescriptorPool descriptorPool) const;

    VkResult CreateBuffer(VkBuffer* pBuffer, VmaAllocation* pAllocation, VkDeviceSize size, VkBufferUsageFlags bufferUsage, VmaMemoryUsage memoryUsage, const uint32_t memoryFlags) const;
    void DestroyBuffer(VkBuffer& buffer, VmaAllocation allocation) const;

    VkResult CreateImage(VkImage* pImage, VmaAllocation* pAllocation, VkImageType type, VkExtent3D extents, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VmaMemoryUsage memoryUsage, uint32_t memoryFlags) const; 
    void DestroyImage(VkImage& image, VmaAllocation& allocation) const; 

    VkResult CreateImageView(VkImageView* pImageView, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags) const; 
    void DestroyImageView(VkImageView& imageView) const; 

private:
    void CreateInstance();
    void SelectPhysicalDevice();
    void CreateDevice();
    void CreateSwapchain(GLFWwindow* window, int width, int height);
    void CreatePipelineCache(); 
    void CreateVMAAllocator(); 

    void DestroyInstance();
    void DestroyDevice();
    void DestroySwapchain();
    void DumpPipelineCache(); 
    void DestroyVMAAllocator(); 

    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugLogCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);

    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);

    VkResult DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT* pDebugMessenger, const VkAllocationCallbacks* pAllocator);

private:
    VkInstance m_Instance;
    VkPhysicalDevice m_PhysicalDevice;
    VkDevice m_Device;
    VkSwapchainKHR m_Swapchain;
    std::vector<VkImage> m_SwapchainImages;
    std::vector<VkImageView> m_SwapchainImageViews; 
    VkExtent2D m_SwapchainExtent; 

    VkSurfaceKHR m_Surface; 
    VkFormat m_SwapchainImageFormat; 

    VkDebugUtilsMessengerEXT m_DebugMessenger;
    VkQueue m_Queue; 
    uint32_t m_QueueFamilyIndex; 

    const VkEngineInitInfo* m_pInitInfo;

    VkPipelineCache m_PipelineCache; 
    VmaAllocator m_VmaAllocator; 

};