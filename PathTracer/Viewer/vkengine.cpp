#define VMA_IMPLEMENTATION

#include "vkengine.h"
#include "../Utils/Logger.h"
#include "window.h"

#include <fstream>
#include <filesystem>

#define PIPELINE_CACHE_PATH "_PipelineCache.bin"

VkEngine::VkEngine()
{
    m_Instance = VK_NULL_HANDLE;
    m_DebugMessenger = VK_NULL_HANDLE;
    m_PhysicalDevice = VK_NULL_HANDLE;
    m_Device = VK_NULL_HANDLE;
    m_Swapchain = VK_NULL_HANDLE;
    m_Surface = VK_NULL_HANDLE;

    m_Queue = VK_NULL_HANDLE;
    m_QueueFamilyIndex = -1;

    m_PipelineCache = VK_NULL_HANDLE;
    m_pInitInfo = nullptr;
}

VkEngine::~VkEngine()
{

}

void VkEngine::Init(const VkEngineInitInfo& initInfo, const Window* pWindow)
{
    m_pInitInfo = &initInfo;

    CreateInstance();
    SelectPhysicalDevice();
    CreateDevice();
    glfwCreateWindowSurface(m_Instance, pWindow->GetHandle(), nullptr, &m_Surface);
    CreateSwapchain(pWindow->GetHandle(), pWindow->Width(), pWindow->Height());
    CreatePipelineCache();
    CreateVMAAllocator();
}

void VkEngine::Shutdown()
{
    DestroyVMAAllocator();
    DumpPipelineCache();
    DestroySwapchain();
    DestroyDevice();
    DestroyInstance();
}

VkInstance VkEngine::Instance() const
{
    if (m_Instance == VK_NULL_HANDLE) {
       EDX::Log::Error(__FILE__, __LINE__, __PRETTY_FUNCTION__, "Instance was VK_NULL_HANDLE!\n");
    }
    return m_Instance;
}

VkPhysicalDevice VkEngine::PhysicalDevice() const
{
    if (m_PhysicalDevice == VK_NULL_HANDLE) {
       EDX::Log::Error(__FILE__, __LINE__, __PRETTY_FUNCTION__, "Physical Device was VK_NULL_HANDLE!\n");
    }
    return m_PhysicalDevice;
}

VkDevice VkEngine::Device() const
{
    if (m_Device == VK_NULL_HANDLE) {
       EDX::Log::Error(__FILE__, __LINE__, __PRETTY_FUNCTION__, "Device was VK_NULL_HANDLE!\n");
    }
    return m_Device;
}

VkSwapchainKHR VkEngine::Swapchain() const
{
    if (m_Swapchain == VK_NULL_HANDLE) {
       EDX::Log::Error(__FILE__, __LINE__, __PRETTY_FUNCTION__, "Swapchain was VK_NULL_HANDLE!\n");
    }
    return m_Swapchain;
}

const VkExtent2D VkEngine::SwapchainExtents() const
{
    return m_SwapchainExtent;
}

const VkFormat VkEngine::SwapchainImageFormat() const
{
    return m_SwapchainImageFormat;
}

const std::vector<VkImage> VkEngine::SwapchainImages() const
{
    return m_SwapchainImages;
}

const std::vector<VkImageView> VkEngine::SwapchainImageViews() const
{
    return m_SwapchainImageViews;
}

VmaAllocator VkEngine::VMAAllocator() const
{
    return m_VmaAllocator; 
}

VkSurfaceKHR VkEngine::Surface() const
{
    return m_Surface;
}

VkQueue VkEngine::Queue() const
{
    return m_Queue;
}

const uint32_t VkEngine::QueueFamilyIndex() const
{
    return m_QueueFamilyIndex;
}


void VkEngine::GLFWFramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    void* pEngine = glfwGetWindowUserPointer(window);
    reinterpret_cast<VkEngine*>(pEngine)->RecreateSwapchain(window, width, height);
}

void VkEngine::RecreateSwapchain(GLFWwindow* window, int width, int height)
{
    vkDeviceWaitIdle(m_Device);

    //Clean up swapchain resources
    for (auto& img : m_SwapchainImages) {
        vkDestroyImage(m_Device, img, nullptr);
        img = VK_NULL_HANDLE;
    }
    m_SwapchainImages.clear();

    for (auto& imgView : m_SwapchainImageViews) {
        vkDestroyImageView(m_Device, imgView, nullptr);
        imgView = VK_NULL_HANDLE;
    }
    m_SwapchainImageViews.clear();

    //TODO: Figure out swapchain recreation
    //vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
    //m_Surface = VK_NULL_HANDLE; 

    //vkDestroySwapchainKHR(m_Device, m_Swapchain, nullptr); 

    CreateSwapchain(window, width, height);

}

VkResult VkEngine::CreateSemaphore(VkSemaphore* pSemaphore) const
{
    VkSemaphoreCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;

    return vkCreateSemaphore(Device(), &createInfo, nullptr, pSemaphore);
}

void VkEngine::DestroySemaphore(VkSemaphore& semaphore) const
{
    vkDestroySemaphore(Device(), semaphore, nullptr);
}

VkResult VkEngine::CreateFence(VkFence* pFence, bool createSignaled) const
{
    VkFenceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = createSignaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;

    return vkCreateFence(Device(), &createInfo, nullptr, pFence);
}

void VkEngine::DestroyFence(VkFence& fence) const
{
    vkDestroyFence(Device(), fence, nullptr);
}

VkResult VkEngine::CreateRenderPass(VkRenderPass* pRenderPass, std::vector<VkAttachmentDescription> attachments, std::vector<VkSubpassDependency> dependencies, std::vector<VkSubpassDescription> subpasses) const
{
    VkRenderPassCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.attachmentCount = attachments.size();
    createInfo.pAttachments = attachments.data();
    createInfo.dependencyCount = dependencies.size();
    createInfo.pDependencies = dependencies.data();
    createInfo.subpassCount = subpasses.size();
    createInfo.pSubpasses = subpasses.data();

    return vkCreateRenderPass(Device(), &createInfo, nullptr, pRenderPass);
}

void VkEngine::DestroyRenderPass(VkRenderPass& renderPass) const
{
    vkDestroyRenderPass(Device(), renderPass, nullptr);
}

VkResult VkEngine::CreateShaderModule(VkShaderModule* pShaderModule, std::vector<char> blob) const
{
    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.codeSize = blob.size();
    createInfo.pCode = (uint32_t*)blob.data();

    return vkCreateShaderModule(Device(), &createInfo, nullptr, pShaderModule);
}

void VkEngine::DestroyShaderModule(VkShaderModule& shaderModule) const
{
    vkDestroyShaderModule(Device(), shaderModule, nullptr);
}

VkResult VkEngine::CreateGraphicsPipelines(VkPipeline* pGraphicsPipelines, std::vector<VkGraphicsPipelineCreateInfo> createInfos) const
{
    return vkCreateGraphicsPipelines(Device(), m_PipelineCache, createInfos.size(), createInfos.data(), nullptr, pGraphicsPipelines);
}

void VkEngine::DestroyPipeline(VkPipeline& pipeline) const
{
    vkDestroyPipeline(Device(), pipeline, nullptr);
}

VkResult VkEngine::CreatePipelineLayout(VkPipelineLayout* pPipelineLayout, std::vector<VkDescriptorSetLayout> setLayouts, std::vector<VkPushConstantRange> pushConstantRanges) const
{
    VkPipelineLayoutCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.setLayoutCount = setLayouts.size();
    createInfo.pSetLayouts = setLayouts.data();
    createInfo.pushConstantRangeCount = pushConstantRanges.size();
    createInfo.pPushConstantRanges = pushConstantRanges.data();

    return vkCreatePipelineLayout(Device(), &createInfo, nullptr, pPipelineLayout);
}

void VkEngine::DestroyPipelineLayout(VkPipelineLayout& pipelineLayout) const
{
    vkDestroyPipelineLayout(Device(), pipelineLayout, nullptr);
}

VkResult VkEngine::CreateFramebuffer(VkFramebuffer* pFrameBuffer, VkRenderPass renderPass, std::vector<VkImageView> attachments, VkExtent2D extents, uint32_t layers) const
{
    VkFramebufferCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.renderPass = renderPass;
    createInfo.attachmentCount = attachments.size();
    createInfo.pAttachments = attachments.data();
    createInfo.width = extents.width;
    createInfo.height = extents.height;
    createInfo.layers = layers;

    return vkCreateFramebuffer(Device(), &createInfo, nullptr, pFrameBuffer);

}

void VkEngine::DestroyFramebuffer(VkFramebuffer& frameBuffer) const
{
    vkDestroyFramebuffer(Device(), frameBuffer, nullptr);
}

VkResult VkEngine::CreateCommandPool(VkCommandPool* pCommandPool, VkCommandPoolCreateFlags flags, uint32_t queueFamilyIndex) const
{
    VkCommandPoolCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = flags;
    createInfo.queueFamilyIndex = queueFamilyIndex;

    return vkCreateCommandPool(Device(), &createInfo, nullptr, pCommandPool);
}

void VkEngine::DestroyCommandPool(VkCommandPool& commandPool) const
{
    vkDestroyCommandPool(Device(), commandPool, nullptr);
}

VkResult VkEngine::AllocateCommandBuffers(VkCommandBuffer* pCommandBuffers, const uint32_t count, VkCommandPool commandPool, VkCommandBufferLevel level) const
{
    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.pNext = nullptr;
    allocInfo.commandBufferCount = count;
    allocInfo.level = level;
    allocInfo.commandPool = commandPool;

    return vkAllocateCommandBuffers(Device(), &allocInfo, pCommandBuffers);
}

void VkEngine::FreeCommandBuffers(VkCommandBuffer* pCommandBuffers, const uint32_t count, VkCommandPool commandPool) const
{
    vkFreeCommandBuffers(Device(), commandPool, count, pCommandBuffers);
}

VkResult VkEngine::CreateDescriptorSetLayout(VkDescriptorSetLayout* pLayout, std::vector<VkDescriptorSetLayoutBinding> bindings) const
{
    VkDescriptorSetLayoutCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.bindingCount = bindings.size();
    createInfo.pBindings = bindings.data();

    return vkCreateDescriptorSetLayout(Device(), &createInfo, nullptr, pLayout);
}

void VkEngine::DestroyDescriptorSetLayout(VkDescriptorSetLayout& layout) const
{
    vkDestroyDescriptorSetLayout(Device(), layout, nullptr);
}

VkResult VkEngine::CreateDescriptorPool(VkDescriptorPool* pDescriptorPool, const uint32_t maxSets, std::vector<VkDescriptorPoolSize> poolSizes) const
{
    VkDescriptorPoolCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.maxSets = maxSets;
    createInfo.poolSizeCount = poolSizes.size();
    createInfo.pPoolSizes = poolSizes.data();

    return vkCreateDescriptorPool(Device(), &createInfo, nullptr, pDescriptorPool);
}

void VkEngine::DestroyDescriptorPool(VkDescriptorPool& descriptorPool) const
{
    vkDestroyDescriptorPool(Device(), descriptorPool, nullptr); 
}

VkResult VkEngine::AllocateDescriptorSets(VkDescriptorSet* pDescriptorSets, const uint32_t count, VkDescriptorSetLayout* pSetLayouts, VkDescriptorPool descriptorPool) const
{
    VkDescriptorSetAllocateInfo allocInfo = {}; 
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO; 
    allocInfo.pNext = nullptr; 
    allocInfo.descriptorPool = descriptorPool; 
    allocInfo.descriptorSetCount = count; 
    allocInfo.pSetLayouts = pSetLayouts; 

    return vkAllocateDescriptorSets(Device(), &allocInfo, pDescriptorSets); 
}

void VkEngine::FreeDescriptorSets(VkDescriptorSet* pDescriptorSets, const uint32_t count, VkDescriptorPool descriptorPool) const
{
    vkFreeDescriptorSets(Device(), descriptorPool, count, pDescriptorSets); 
}

VkResult VkEngine::CreateBuffer(VkBuffer* pBuffer, VmaAllocation* pAllocation, VkDeviceSize size, VkBufferUsageFlags bufferUsage, VmaMemoryUsage memoryUsage, const uint32_t memoryFlags) const
{
    const uint32_t qfi = QueueFamilyIndex(); 

    VkBufferCreateInfo createInfo = {}; 
    createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO; 
    createInfo.pNext = nullptr; 
    createInfo.size = size; 
    createInfo.usage = bufferUsage; 
    createInfo.queueFamilyIndexCount = 1; 
    createInfo.pQueueFamilyIndices = &qfi;
    createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo allocInfo = {}; 
    allocInfo.flags = memoryFlags; 
    allocInfo.usage = memoryUsage;

    return vmaCreateBuffer(m_VmaAllocator, &createInfo, &allocInfo, pBuffer, pAllocation, nullptr); 
}

void VkEngine::DestroyBuffer(VkBuffer& buffer, VmaAllocation allocation) const
{
    vmaDestroyBuffer(m_VmaAllocator, buffer, allocation); 
}

VkResult VkEngine::CreateImage(VkImage* pImage, VmaAllocation* pAllocation, VkImageType type, VkExtent3D extents, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VmaMemoryUsage memoryUsage, uint32_t memoryFlags) const
{
    uint32_t queueFamilyIndex = QueueFamilyIndex();

    VkImageCreateInfo createInfo = {}; 
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    createInfo.pNext = nullptr; 
    createInfo.flags = 0; 
    createInfo.format = format; 
    createInfo.extent = extents; 
    createInfo.arrayLayers = 1; 
    createInfo.imageType = type; 
    createInfo.usage = usage; 
    createInfo.tiling = tiling; 
    createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE; 
    createInfo.queueFamilyIndexCount = 1;
    createInfo.pQueueFamilyIndices = &queueFamilyIndex;
    createInfo.mipLevels = 1; 
    createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; 
    createInfo.samples = VK_SAMPLE_COUNT_1_BIT; 

    VmaAllocationCreateInfo allocInfo = {}; 
    allocInfo.flags = memoryFlags; 
    allocInfo.usage = memoryUsage; 

    return vmaCreateImage(m_VmaAllocator, &createInfo, &allocInfo, pImage, pAllocation, nullptr); 
}

void VkEngine::DestroyImage(VkImage& image, VmaAllocation& allocation) const
{
    vmaDestroyImage(m_VmaAllocator, image, allocation); 
}

VkResult VkEngine::CreateImageView(VkImageView* pImageView, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags) const
{
    VkImageViewCreateInfo createInfo = {}; 
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO; 
    createInfo.pNext = nullptr; 
    createInfo.flags = 0; 
    createInfo.format = format; 
    createInfo.image = image; 
    createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D; 
    createInfo.subresourceRange.aspectMask = aspectFlags; 
    createInfo.subresourceRange.baseArrayLayer = 0; 
    createInfo.subresourceRange.baseMipLevel = 0; 
    createInfo.subresourceRange.layerCount = 1; 
    createInfo.subresourceRange.levelCount = 1; 

    return vkCreateImageView(Device(), &createInfo, nullptr, pImageView); 
}

void VkEngine::DestroyImageView(VkImageView& imageView) const
{
    vkDestroyImageView(Device(), imageView, nullptr); 
}


//PRIVATE

void VkEngine::CreateInstance()
{
    EDX::Log::Status("[Engine]\tCreating Vulkan Instance.\n");

    //Evaluate requested instance extensions / layers
    std::vector<const char*> instanceLayers;
    if (m_pInitInfo->enableValidationLayers) {
        EDX::Log::Status("Enabling Instance Layer <VK_LAYER_KHRONOS_validation>.\n");
        instanceLayers.push_back("VK_LAYER_KHRONOS_validation");
    }

    {
        //Retrieve a list of supported instance layers
        uint32_t layerCount = 0;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
        std::vector<VkLayerProperties> layerProps(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, layerProps.data());

        //Evaluate Instance Layer Support
        for (const auto& layer : m_pInitInfo->requestedInstanceLayers) {
            bool layerSupported = false;

            for (const auto& props : layerProps) {
                if (strcmp(props.layerName, layer) == 0) {
                    layerSupported = true;
                    break;
                }
            }

            //Add the layer to our internal list, if supported. 
            if (layerSupported) {
                EDX::Log::Status("Enabling Instance Layer <%s>.\n", layer);
                instanceLayers.push_back(layer);
            }
            else {
                EDX::Log::Status("Layer <%s> is not supported by the Vulkan Instance!\n", layer);
            }
        }
    }


    std::vector<const char*> instanceExtensions;
    if (m_pInitInfo->enableDebugUtils) {
        EDX::Log::Status("Enabling Instance Extension <%s>.\n", VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        instanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    {
        //Retrieve a list of supported instance extensions
        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        std::vector<VkExtensionProperties> extensionProps(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensionProps.data());

        //Evaluate Instance Extension Support
        for (const auto& extension : m_pInitInfo->requestedInstanceExtensions) {
            bool extensionSupported = false;

            for (const auto& props : extensionProps) {
                if (strcmp(props.extensionName, extension) == 0) {
                    extensionSupported = true;
                    break;
                }
            }

            //Add the extension to our internal list, if supported. 
            if (extensionSupported) {
                EDX::Log::Status("Enabling Instance Extension <%s>.\n", extension);
                instanceExtensions.push_back(extension);
            }
            else {
                EDX::Log::Status("Extension <%s> is not supported by the Vulkan Instance!\n", extension);
            }
        }
    }


    //Application Info
    VkApplicationInfo applicationInfo = {};
    applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    applicationInfo.pNext = nullptr;
    applicationInfo.pApplicationName = m_pInitInfo->appName;
    applicationInfo.applicationVersion = m_pInitInfo->appVersion;
    applicationInfo.apiVersion = m_pInitInfo->apiVersion;
    applicationInfo.pEngineName = m_pInitInfo->engineName;
    applicationInfo.engineVersion = m_pInitInfo->engineVersion;

    //Optionally create a Debug messenger
    VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo = {};
    debugMessengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugMessengerCreateInfo.pNext = nullptr;
    debugMessengerCreateInfo.pUserData = this;
    debugMessengerCreateInfo.pfnUserCallback = DebugLogCallback;     debugMessengerCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debugMessengerCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;

    //Create the Vulkan Instance
    VkInstanceCreateInfo instanceCreateInfo;
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.pNext = (m_pInitInfo->enableDebugUtils) ? &debugMessengerCreateInfo : nullptr;
    instanceCreateInfo.flags = 0;
    instanceCreateInfo.enabledExtensionCount = instanceExtensions.size();
    instanceCreateInfo.ppEnabledExtensionNames = instanceExtensions.data();
    instanceCreateInfo.enabledLayerCount = instanceLayers.size();
    instanceCreateInfo.ppEnabledLayerNames = instanceLayers.data();
    instanceCreateInfo.pApplicationInfo = &applicationInfo;

    VkResult result = vkCreateInstance(&instanceCreateInfo, nullptr, &m_Instance);

    if (m_pInitInfo->enableDebugUtils) {
        CreateDebugUtilsMessengerEXT(m_Instance, &debugMessengerCreateInfo, nullptr, &m_DebugMessenger);
    }

}

void VkEngine::SelectPhysicalDevice()
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(m_Instance, &deviceCount, nullptr);
    std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
    vkEnumeratePhysicalDevices(m_Instance, &deviceCount, physicalDevices.data());

    const auto& lastDevice = physicalDevices.end();
    for (const auto& device : physicalDevices) {


        //Evaluate device feature compatibility
        VkPhysicalDeviceFeatures deviceFeatures = {};
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

        bool featuresValid = true;

        for (size_t i = 0; i < sizeof(VkPhysicalDeviceFeatures) / sizeof(VkBool32); i++) {
            VkBool32* a = ((VkBool32*)&m_pInitInfo->requiredFeatures) + i;
            VkBool32* b = ((VkBool32*)&deviceFeatures) + i;

            if (*a == VK_TRUE) {
                if (*b != VK_TRUE) {
                    featuresValid = false;
                }
            }
        }

        if (!featuresValid) {
            EDX::Log::Status("Not all required Physical Device Features were available!\n");
            continue;
        }

        //Prefer a Discrete GPU. 
        VkPhysicalDeviceProperties deviceProperties = {};
        vkGetPhysicalDeviceProperties(device, &deviceProperties);

        if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            m_PhysicalDevice = device;
            break;
        }

        //Continue searching
        else {
            m_PhysicalDevice = device;
            continue;
        }

    }

    if (m_PhysicalDevice == VK_NULL_HANDLE) {
        EDX::Log::Status("No Suitable Physical Device found!\n");
        return;
    }

    VkPhysicalDeviceProperties deviceProperties = {};
    vkGetPhysicalDeviceProperties(m_PhysicalDevice, &deviceProperties);

    EDX::Log::Status("Selected Physical Device:\t%s\n", deviceProperties.deviceName);
}

void VkEngine::CreateDevice()
{
    EDX::Log::Status("[Engine]\tCreating Vulkan Device.\n");

    std::vector<const char*> deviceExtensions;

    //Validate requested device extensions
    {
        uint32_t propertyCount = 0;
        vkEnumerateDeviceExtensionProperties(m_PhysicalDevice, nullptr, &propertyCount, nullptr);
        std::vector<VkExtensionProperties> extensionProperties(propertyCount);
        vkEnumerateDeviceExtensionProperties(m_PhysicalDevice, nullptr, &propertyCount, extensionProperties.data());

        for (const auto& extension : m_pInitInfo->requestedDeviceExtensions) {

            //Evaluate Instance Extension Support
            bool extensionSupported = false;

            for (const auto& property : extensionProperties) {
                if (strcmp(property.extensionName, extension) == 0) {
                    extensionSupported = true;
                    break;
                }
            }

            //Add the extension to our internal list, if supported. 
            if (extensionSupported) {
                EDX::Log::Status("Enabling Device Extension <%s>.\n", extension);
                deviceExtensions.push_back(extension);
            }
            else {
                EDX::Log::Status("Extension <%s> is not supported by the current Vulkan Device!\n", extension);
            }
        }
    }

    auto FindGraphicsQueueFamilyIndex = [](VkPhysicalDevice physicalDevice) -> uint32_t {

        uint32_t propCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &propCount, nullptr);
        std::vector<VkQueueFamilyProperties> properties(propCount);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &propCount, properties.data());

        uint32_t index = 0;
        for (const auto& p : properties) {

            if (p.queueFlags & VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT) {
                return index;
            }
            index++;
        }

        return 0;
    };

    m_QueueFamilyIndex = FindGraphicsQueueFamilyIndex(m_PhysicalDevice);


    //We want one Graphics queue
    float priority = 1.0f;
    VkDeviceQueueCreateInfo queueCreateInfo = {};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.pNext = nullptr;
    queueCreateInfo.flags = 0;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.queueFamilyIndex = m_QueueFamilyIndex;
    queueCreateInfo.pQueuePriorities = &priority;


    //Create the Device
    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.pNext = nullptr;
    deviceCreateInfo.flags = 0;
    deviceCreateInfo.enabledExtensionCount = deviceExtensions.size();
    deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
    deviceCreateInfo.queueCreateInfoCount = 1;
    deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
    deviceCreateInfo.enabledLayerCount = 0;
    deviceCreateInfo.ppEnabledLayerNames = nullptr;
    deviceCreateInfo.pEnabledFeatures = &m_pInitInfo->requiredFeatures;

    vkCreateDevice(m_PhysicalDevice, &deviceCreateInfo, nullptr, &m_Device);

    vkGetDeviceQueue(m_Device, m_QueueFamilyIndex, 0, &m_Queue);

}

void VkEngine::CreateSwapchain(GLFWwindow* window, int width, int height)
{
    m_SwapchainExtent.width = width;
    m_SwapchainExtent.height = height;

    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_PhysicalDevice, m_Surface, &capabilities);

    uint32_t numFormats = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(m_PhysicalDevice, m_Surface, &numFormats, nullptr);
    if (numFormats != 0) {
        formats.resize(numFormats);
        vkGetPhysicalDeviceSurfaceFormatsKHR(m_PhysicalDevice, m_Surface, &numFormats, formats.data());
    }

    uint32_t numPresentModes = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(m_PhysicalDevice, m_Surface, &numPresentModes, nullptr);
    if (numPresentModes != 0) {
        presentModes.resize(numPresentModes);
        vkGetPhysicalDeviceSurfacePresentModesKHR(m_PhysicalDevice, m_Surface, &numPresentModes, presentModes.data());
    }

    //TODO: We're being lazy and just selecting the first swapchain image format / present mode. 
    m_SwapchainImageFormat = formats[0].format;
    VkPresentModeKHR presentMode = [](std::vector<VkPresentModeKHR> presentModes) {

        for (const auto& mode : presentModes) {
            if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
                return mode; 
            }
        }

        return presentModes[0]; 
        
    }(presentModes);

    VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
    swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainCreateInfo.pNext = nullptr;
    swapchainCreateInfo.flags = 0;
    swapchainCreateInfo.oldSwapchain = m_Swapchain;
    swapchainCreateInfo.presentMode = presentMode;
    swapchainCreateInfo.queueFamilyIndexCount = 1;
    swapchainCreateInfo.pQueueFamilyIndices = &m_QueueFamilyIndex;
    swapchainCreateInfo.imageExtent.width = width;
    swapchainCreateInfo.imageExtent.height = height;
    swapchainCreateInfo.surface = m_Surface;
    swapchainCreateInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    swapchainCreateInfo.clipped = VK_FALSE;
    swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainCreateInfo.imageFormat = formats[0].format;
    swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchainCreateInfo.minImageCount = capabilities.minImageCount + 1;
    swapchainCreateInfo.imageColorSpace = formats[0].colorSpace;
    swapchainCreateInfo.imageArrayLayers = 1;

    auto r = vkCreateSwapchainKHR(m_Device, &swapchainCreateInfo, nullptr, &m_Swapchain);

    uint32_t swapchainImageCount = 0;
    vkGetSwapchainImagesKHR(m_Device, m_Swapchain, &swapchainImageCount, nullptr);
    m_SwapchainImages.resize(swapchainImageCount);
    vkGetSwapchainImagesKHR(m_Device, m_Swapchain, &swapchainImageCount, m_SwapchainImages.data());

    for (const auto& img : m_SwapchainImages) {
        VkImageViewCreateInfo imageViewInfo = {};
        imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewInfo.pNext = nullptr;
        imageViewInfo.flags = 0;
        imageViewInfo.format = swapchainCreateInfo.imageFormat;
        imageViewInfo.image = img;
        imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imageViewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageViewInfo.subresourceRange.baseArrayLayer = 0;
        imageViewInfo.subresourceRange.baseMipLevel = 0;
        imageViewInfo.subresourceRange.layerCount = 1;
        imageViewInfo.subresourceRange.levelCount = 1;

        VkImageView view = VK_NULL_HANDLE;
        vkCreateImageView(m_Device, &imageViewInfo, nullptr, &view);

        m_SwapchainImageViews.push_back(view);
    }

}

void VkEngine::CreatePipelineCache()
{
    std::vector<char> cache;
    //TODO: std::filesystem integration
    std::ifstream binary(PIPELINE_CACHE_PATH, std::ios::binary | std::ios::ate);
    if (binary.is_open()) {
        EDX::Log::Status("Loading Existing Pipeline Cache.\n");
        size_t cacheSize = binary.tellg();
        cache.resize(cacheSize);
        binary.seekg(0);
        binary.read(cache.data(), cacheSize);
        binary.close();
    }

    VkPipelineCacheCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.initialDataSize = cache.size();
    createInfo.pInitialData = cache.data();

    vkCreatePipelineCache(Device(), &createInfo, nullptr, &m_PipelineCache);
}

void VkEngine::CreateVMAAllocator()
{
    VmaAllocatorCreateInfo createInfo = {};
    createInfo.flags = 0;
    createInfo.instance = Instance();
    createInfo.device = Device();
    createInfo.pAllocationCallbacks = nullptr;
    createInfo.pDeviceMemoryCallbacks = nullptr;
    createInfo.pHeapSizeLimit = nullptr;
    createInfo.physicalDevice = PhysicalDevice();

    vmaCreateAllocator(&createInfo, &m_VmaAllocator);
}

void VkEngine::DestroyInstance()
{
    DestroyDebugUtilsMessengerEXT(m_Instance, &m_DebugMessenger, nullptr);
    vkDestroyInstance(m_Instance, nullptr);
}

void VkEngine::DestroyDevice()
{
    vkDestroyDevice(m_Device, nullptr);
}

void VkEngine::DestroySwapchain()
{
    //Clean up swapchain resources
    m_SwapchainImages.clear();

    for (auto& imgView : m_SwapchainImageViews) {
        vkDestroyImageView(m_Device, imgView, nullptr);
        imgView = VK_NULL_HANDLE;
    }
    m_SwapchainImageViews.clear();

    vkDestroySwapchainKHR(m_Device, m_Swapchain, nullptr);
    vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
}

void VkEngine::DumpPipelineCache()
{
    size_t cacheSize = 0;
    vkGetPipelineCacheData(Device(), m_PipelineCache, &cacheSize, nullptr);
    void* pData = new char[cacheSize];
    vkGetPipelineCacheData(Device(), m_PipelineCache, &cacheSize, pData);

    std::ofstream binary(PIPELINE_CACHE_PATH, std::ios::binary);
    if (!binary.is_open() || pData == nullptr) {
        EDX::Log::Status("Failed to create Pipeline Cache!\n");
        return;
    }
    binary.write((char*)pData, cacheSize);
    binary.close();

    vkDestroyPipelineCache(Device(), m_PipelineCache, nullptr);
}

void VkEngine::DestroyVMAAllocator()
{
    vmaDestroyAllocator(m_VmaAllocator);
}

VKAPI_ATTR VkBool32 VKAPI_CALL VkEngine::DebugLogCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
    if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        EDX::Log::Status("[%d : %s] %s\n", pCallbackData->messageIdNumber, pCallbackData->pMessageIdName, pCallbackData->pMessage);
    }

    return VK_FALSE;
}

VkResult VkEngine::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

VkResult VkEngine::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT* pDebugMessenger, const VkAllocationCallbacks* pAllocator)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, *pDebugMessenger, pAllocator);
        return VK_SUCCESS;
    }
    else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}
