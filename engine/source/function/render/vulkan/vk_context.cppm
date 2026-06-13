export module crumb.render.vulkan:context;

import crumb.render;
import vulkan;
import crumb.platform.window;
import std;
import vk_mem_alloc;

export namespace Crumb::Render {

struct SwapChainSupportDetails {
    vk::SurfaceCapabilitiesKHR capabilities;
    std::vector<vk::SurfaceFormatKHR> formats;
    std::vector<vk::PresentModeKHR> presentModes;
};

class VKContext {
public:
    explicit VKContext();
    ~VKContext();

    bool CreateContext(Window& window, const std::string& appName, bool enableValidationLayers = true);

    // 设备与队列访问器
    const QueueFamilyIndices& GetQueueFamilyIndices() const { return m_queueFamilyIndices; }
    vk::raii::Device& GetDevice() { return m_device; }
    vk::raii::PhysicalDevice& GetPhysicalDevice() { return m_physicalDevice; }
    vk::raii::SurfaceKHR& GetSurface() { return m_surface; }
    vk::raii::Queue& GetGraphicsQueue() { return m_graphicsQueue; }
    vk::raii::Queue& GetPresentQueue() { return m_presentQueue; }
    vk::raii::Instance& GetInstance() { return m_instance; }

    vma::raii::Allocator& GetAllocator() { return m_vmaAllocator; }

    // 特性查询
    bool IsDescriptorIndexingEnabled() const { return m_descriptorIndexingEnabled; }

private:
    // 实例层
    bool CheckValidationLayerSupport() const;
    std::vector<const char*> GetInstanceExtensions() const;
    bool CreateInstance(const std::string& appName, bool enableValidationLayers);
    bool SetupDebugMessenger(bool enableValidationLayers);

    // 设备层
    bool CreateSurface();
    bool PickPhysicalDevice();
    QueueFamilyIndices FindQueueFamilies(const vk::raii::PhysicalDevice& device) const;
    SwapChainSupportDetails QuerySwapChainSupport(const vk::raii::PhysicalDevice& device) const;
    bool CheckDeviceExtensionSupport(vk::raii::PhysicalDevice& device);
    bool IsDeviceSuitable(vk::raii::PhysicalDevice& device);
    void AddSupportedOptionalExtensions();
    bool CreateLogicalDevice(bool enableValidationLayers);

    vk::raii::Context m_context;
    vk::raii::Instance m_instance = nullptr;
    vk::raii::DebugUtilsMessengerEXT m_debugMessenger = nullptr;
    vk::raii::SurfaceKHR m_surface = nullptr;
    vk::raii::PhysicalDevice m_physicalDevice = nullptr;
    vk::raii::Device m_device = nullptr;

    // 队列
    vk::raii::Queue m_graphicsQueue = nullptr;
    vk::raii::Queue m_presentQueue = nullptr;
    vk::raii::Queue m_computeQueue = nullptr;
    vk::raii::Queue m_transferQueue = nullptr;

    QueueFamilyIndices m_queueFamilyIndices;

    vma::raii::Allocator m_vmaAllocator = nullptr;

    Window* m_window = nullptr;

    // 特性标志
    bool m_descriptorIndexingEnabled = false;

    const std::vector<const char*> m_validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };
    const std::vector<const char*> m_requiredDeviceExtensions = {
        vk::KHRSwapchainExtensionName
    };
    const std::vector<const char*> m_optionalDeviceExtensions = {
        vk::KHRDynamicRenderingExtensionName,
        vk::KHRGetPhysicalDeviceProperties2ExtensionName,
        vk::EXTDescriptorIndexingExtensionName,
        vk::EXTRobustness2ExtensionName,
    };

    std::vector<const char*> m_deviceExtensions;
};

} // namespace Crumb::Render
