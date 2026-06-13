module;
#include <vulkan/vulkan_core.h>

module crumb.render.vulkan;
import :context;

import crumb.core;
import std;
import vulkan;
import vk_mem_alloc;

namespace Crumb::Render{

VKContext::VKContext() : m_deviceExtensions(m_requiredDeviceExtensions) {
    
}

VKContext::~VKContext() = default;

bool VKContext::CreateContext(Window& window, const std::string& appName, bool enableValidationLayers) {
    m_window = &window;

    Log::Info("[VKContext] Initializing Vulkan...");
    if (!CreateInstance(appName, enableValidationLayers)) {
        Log::Error("[VKContext] Failed to create instance");
        return false;
    }
    Log::Info("[VKContext] Instance created");
    if (!SetupDebugMessenger(enableValidationLayers)) {
        Log::Error("[VKContext] Failed to create debug messenger");
        return false;
    }
    if (enableValidationLayers) Log::Info("[VKContext] Debug messenger attached");
    if (!CreateSurface()) {
        Log::Error("[VKContext] Failed to create surface");
        return false;
    }
    Log::Info("[VKContext] Surface created");
    if (!PickPhysicalDevice()) {
        Log::Error("[VKContext] Failed to pick physical device");
        return false;
    }
    if (!CreateLogicalDevice(enableValidationLayers)) {
        Log::Error("[VKContext] Failed to create logical device");
        return false;
    }
    Log::Info("[VKContext] Device + VMA allocator ready");
    return true;
}

bool VKContext::CheckValidationLayerSupport() const {
    auto layers = m_context.enumerateInstanceLayerProperties();
    for (const auto* layerName : m_validationLayers) {
        bool found = false;
        for (const auto& lp : layers) {
            if (std::strcmp(layerName, lp.layerName) == 0) {
                found = true;
                break;
            }
        }
        if (!found) return false;
    }
    return true;
}

std::vector<const char*> VKContext::GetInstanceExtensions() const {
    std::uint32_t count = 0;
    auto exts = m_window->GetInstanceExtensions(&count);
    if (!exts) {
        throw std::runtime_error("[VKContext] Window::GetInstanceExtensions failed");
    }
    return {exts, exts + count};
}

bool VKContext::CreateInstance(const std::string& appName, bool enableValidationLayers) {
    try {
        vk::ApplicationInfo appInfo;
        appInfo.setPApplicationName(appName.c_str())
               .setApplicationVersion(vk::makeApiVersion(1, 0, 0, 0))
               .setPEngineName("Crumb Engine")
               .setEngineVersion(vk::makeApiVersion(1, 0, 0, 0))
               .setApiVersion(vk::ApiVersion14);

        auto extensions = GetInstanceExtensions();
        if (enableValidationLayers) {
            extensions.push_back(vk::EXTDebugUtilsExtensionName);
        }

        vk::InstanceCreateInfo createInfo;
        createInfo.setPApplicationInfo(&appInfo)
                  .setEnabledExtensionCount(static_cast<std::uint32_t>(extensions.size()))
                  .setPpEnabledExtensionNames(extensions.data());

        if (enableValidationLayers) {
            if (!CheckValidationLayerSupport()) {
                Log::Error("[VKContext] Validation layers unavailable");
                return false;
            }
            createInfo.setPEnabledLayerNames(m_validationLayers)
                      .setEnabledLayerCount(static_cast<std::uint32_t>(m_validationLayers.size()));
        }

        m_instance = vk::raii::Instance(m_context, createInfo);
        return true;
    } catch (const std::exception& e) {
        Log::Error("[VKContext] createInstance exception: {}", e.what());
        return false;
    }
}

bool VKContext::SetupDebugMessenger(bool enableValidationLayers) {
    if (!enableValidationLayers) return true;
    try {
        vk::DebugUtilsMessengerCreateInfoEXT info;
        info.setMessageSeverity(vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning
                                | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError)
            .setMessageType(vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral
                            | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation
                            | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance)
            .setPfnUserCallback([](vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
                                   vk::DebugUtilsMessageTypeFlagsEXT,
                                   const vk::DebugUtilsMessengerCallbackDataEXT* pData,
                                   void*) -> vk::Bool32 {
                if (severity >= vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning) {
                    Log::Debug("[Vulkan] {}", pData->pMessage);
                }
                return vk::False;
            });
        m_debugMessenger = vk::raii::DebugUtilsMessengerEXT(m_instance, info);
        return true;
    } catch (const std::exception& e) {
        Log::Error("[VKContext] SetupDebugMessenger exception: {}", e.what());
        return false;
    }
}

bool VKContext::CreateSurface() {
    VkInstance rawInstance = static_cast<VkInstance>(*m_instance);
    void* rawSurface = m_window->CreateSurface(rawInstance);
    if (!rawSurface) {
        Log::Error("[VKContext] Window::CreateSurface failed");
        return false;
    }
    m_surface = vk::raii::SurfaceKHR(m_instance, reinterpret_cast<VkSurfaceKHR>(rawSurface));
    return true;
}

QueueFamilyIndices VKContext::FindQueueFamilies(const vk::raii::PhysicalDevice& pd) const {
    QueueFamilyIndices indices;
    auto queueFamilies = pd.getQueueFamilyProperties();

    for (std::uint32_t i = 0; i < queueFamilies.size(); ++i) {
        const auto& qf = queueFamilies[i];

        if ((qf.queueFlags & vk::QueueFlagBits::eGraphics) && !indices.graphicsFamily.has_value()) {
            indices.graphicsFamily = i;
        }
        if ((qf.queueFlags & vk::QueueFlagBits::eCompute) && !indices.computeFamily.has_value()) {
            indices.computeFamily = i;
        }
        if (!indices.presentFamily.has_value() && pd.getSurfaceSupportKHR(i, *m_surface)) {
            indices.presentFamily = i;
        }
        if ((qf.queueFlags & vk::QueueFlagBits::eTransfer)
            && !(qf.queueFlags & vk::QueueFlagBits::eGraphics)
            && !indices.transferFamily.has_value()) {
            indices.transferFamily = i;
        }

        if (indices.IsComplete() && indices.transferFamily.has_value()) break;
    }

    if (!indices.transferFamily.has_value() && indices.graphicsFamily.has_value()) {
        indices.transferFamily = indices.graphicsFamily;
    }
    return indices;
}

SwapChainSupportDetails VKContext::QuerySwapChainSupport(const vk::raii::PhysicalDevice& pd) const {
    SwapChainSupportDetails details;
    details.capabilities = pd.getSurfaceCapabilitiesKHR(*m_surface);
    details.formats = pd.getSurfaceFormatsKHR(*m_surface);
    details.presentModes = pd.getSurfacePresentModesKHR(*m_surface);
    return details;
}

bool VKContext::CheckDeviceExtensionSupport(vk::raii::PhysicalDevice& pd) {
    auto available = pd.enumerateDeviceExtensionProperties();
    std::set<std::string> required(m_requiredDeviceExtensions.begin(), m_requiredDeviceExtensions.end());

    for (const auto& ext : available) {
        required.erase(ext.extensionName);
    }

    if (!required.empty()) {
        Log::Error("[VKContext] Missing required device extensions:");
        for (const auto& name : required) {
            Log::Info("  {}", name);
        }
        return false;
    }
    return true;
}

bool VKContext::IsDeviceSuitable(vk::raii::PhysicalDevice& pd) {
    auto indices = FindQueueFamilies(pd);
    bool extensionsOk = CheckDeviceExtensionSupport(pd);

    bool swapChainOk = false;
    if (extensionsOk) {
        auto support = QuerySwapChainSupport(pd);
        swapChainOk = !support.formats.empty() && !support.presentModes.empty();
    }

    auto features = pd.getFeatures2<vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceVulkan13Features, vk::PhysicalDeviceVulkan14Features>();
    bool dynamicRenderingOk = features.get<vk::PhysicalDeviceVulkan13Features>().dynamicRendering;
    bool maintenance5Ok = features.get<vk::PhysicalDeviceVulkan14Features>().maintenance5;

    return indices.IsComplete() && extensionsOk && swapChainOk && dynamicRenderingOk && maintenance5Ok;
}

bool VKContext::PickPhysicalDevice() {
    try {
        auto devices = m_instance.enumeratePhysicalDevices();
        if (devices.empty()) {
            Log::Error("[VKContext] No Vulkan-capable GPU found");
            return false;
        }

        std::multimap<int, vk::raii::PhysicalDevice> candidates;

        for (auto& dev : devices) {
            auto props = dev.getProperties();
            Log::Info("[VKContext] Checking device: {} (type: {})", static_cast<const char*>(props.deviceName),
                         vk::to_string(props.deviceType));

            if (props.apiVersion < vk::ApiVersion14) {
                Log::Info("  - Does not support Vulkan 1.4");
                continue;
            }
            if (!IsDeviceSuitable(dev)) continue;

            int score = 0;
            if (props.deviceType == vk::PhysicalDeviceType::eDiscreteGpu) {
                score += 1000;
            } else if (props.deviceType == vk::PhysicalDeviceType::eIntegratedGpu) {
                score += 100;
            }

            auto mem = dev.getMemoryProperties();
            for (std::uint32_t i = 0; i < mem.memoryHeapCount; ++i) {
                if (mem.memoryHeaps[i].flags & vk::MemoryHeapFlagBits::eDeviceLocal) {
                    score += static_cast<int>(mem.memoryHeaps[i].size / (1024 * 1024 * 1024));
                    break;
                }
            }
            Log::Info("  - score: {}", score);
            candidates.emplace(score, dev);
        }

        if (candidates.empty()) {
            Log::Error("[VKContext] No suitable GPU found");
            return false;
        }

        m_physicalDevice = std::move(candidates.rbegin()->second);
        auto props = m_physicalDevice.getProperties();
        Log::Info("[VKContext] Selected: {} (score: {})", static_cast<const char*>(props.deviceName),
                     candidates.rbegin()->first);

        m_queueFamilyIndices = FindQueueFamilies(m_physicalDevice);
        AddSupportedOptionalExtensions();
        return true;
    } catch (const std::exception& e) {
        Log::Error("[VKContext] PickPhysicalDevice exception: {}", e.what());
        return false;
    }
}

void VKContext::AddSupportedOptionalExtensions() {
    auto available = m_physicalDevice.enumerateDeviceExtensionProperties();
    std::set<std::string> availSet;
    for (const auto& e : available) {
        availSet.insert(e.extensionName);
    }
    for (const auto* opt : m_optionalDeviceExtensions) {
        if (availSet.contains(opt)) {
            m_deviceExtensions.push_back(opt);
            Log::Info("[VKContext] Added optional extension: {}", opt);
        }
    }
}

bool VKContext::CreateLogicalDevice(bool enableValidationLayers) {
    try {
        std::set<std::uint32_t> uniqueFamilies = {
            m_queueFamilyIndices.graphicsFamily.value(),
            m_queueFamilyIndices.presentFamily.value(),
            m_queueFamilyIndices.computeFamily.value(),
            m_queueFamilyIndices.transferFamily.value(),
        };

        float priority = 1.0f;
        std::vector<vk::DeviceQueueCreateInfo> queueInfos;
        for (auto family : uniqueFamilies) {
            vk::DeviceQueueCreateInfo qInfo;
            qInfo.setQueueFamilyIndex(family)
                 .setQueueCount(1)
                 .setPQueuePriorities(&priority);
            queueInfos.push_back(qInfo);
        }

        auto supported = m_physicalDevice.getFeatures2<
            vk::PhysicalDeviceFeatures2,
            vk::PhysicalDeviceVulkan11Features,
            vk::PhysicalDeviceVulkan12Features,
            vk::PhysicalDeviceVulkan13Features,
            vk::PhysicalDeviceVulkan14Features,
            vk::PhysicalDeviceRobustness2FeaturesEXT>();

        const auto& coreF = supported.get<vk::PhysicalDeviceFeatures2>().features;
        const auto& v11F = supported.get<vk::PhysicalDeviceVulkan11Features>();
        const auto& v12F = supported.get<vk::PhysicalDeviceVulkan12Features>();
        const auto& v13F = supported.get<vk::PhysicalDeviceVulkan13Features>();
        const auto& v14F = supported.get<vk::PhysicalDeviceVulkan14Features>();
        const auto& r2F  = supported.get<vk::PhysicalDeviceRobustness2FeaturesEXT>();

        if (!coreF.samplerAnisotropy ||
            !v11F.shaderDrawParameters ||
            !v12F.runtimeDescriptorArray || !v12F.descriptorIndexing ||
            !v12F.timelineSemaphore || !v12F.vulkanMemoryModel || !v12F.bufferDeviceAddress ||
            !v13F.dynamicRendering || !v13F.synchronization2 ||
            !v14F.maintenance5 || !v14F.pushDescriptor) {
            throw std::runtime_error("Physical device lacks required features");
        }

        vk::PhysicalDeviceFeatures2 baseFeatures;
        baseFeatures.features.samplerAnisotropy = vk::True;
        baseFeatures.features.depthBiasClamp = coreF.depthBiasClamp ? vk::True : vk::False;
        if (coreF.shaderSampledImageArrayDynamicIndexing) {
            baseFeatures.features.shaderSampledImageArrayDynamicIndexing = vk::True;
        }

        vk::PhysicalDeviceVulkan11Features v11Enable;
        v11Enable.setShaderDrawParameters(vk::True);

        vk::PhysicalDeviceVulkan12Features v12Enable;
        v12Enable.setRuntimeDescriptorArray(vk::True)
                 .setDescriptorIndexing(vk::True)
                 .setTimelineSemaphore(vk::True)
                 .setVulkanMemoryModel(vk::True)
                 .setVulkanMemoryModelDeviceScope(
                     v12F.vulkanMemoryModelDeviceScope ? vk::True : vk::False)
                 .setBufferDeviceAddress(vk::True)
                 .setShaderSampledImageArrayNonUniformIndexing(vk::True)
                 .setDescriptorBindingPartiallyBound(
                     v12F.descriptorBindingPartiallyBound ? vk::True : vk::False)
                 .setDescriptorBindingUpdateUnusedWhilePending(
                     v12F.descriptorBindingUpdateUnusedWhilePending ? vk::True : vk::False)
                 .setDescriptorBindingSampledImageUpdateAfterBind(
                     v12F.descriptorBindingSampledImageUpdateAfterBind ? vk::True : vk::False)
                 .setDescriptorBindingUniformBufferUpdateAfterBind(
                     v12F.descriptorBindingUniformBufferUpdateAfterBind ? vk::True : vk::False)
                 .setDescriptorBindingStorageBufferUpdateAfterBind(
                     v12F.descriptorBindingStorageBufferUpdateAfterBind ? vk::True : vk::False);
        if (v12F.storageBuffer8BitAccess)
            v12Enable.setStorageBuffer8BitAccess(vk::True);

        vk::PhysicalDeviceVulkan13Features v13Enable;
        v13Enable.setDynamicRendering(vk::True)
                 .setSynchronization2(vk::True);

        vk::PhysicalDeviceVulkan14Features v14Enable;
        v14Enable.setMaintenance5(vk::True)
                 .setPushDescriptor(vk::True);

        baseFeatures.pNext = &v11Enable;
        v11Enable.pNext = &v12Enable;
        v12Enable.pNext = &v13Enable;
        v13Enable.pNext = &v14Enable;

        auto hasExt = [&](const char* name) {
            return std::find_if(m_deviceExtensions.begin(), m_deviceExtensions.end(),
                                [&](const char* e) { return std::strcmp(e, name) == 0; })
                   != m_deviceExtensions.end();
        };

        vk::PhysicalDeviceRobustness2FeaturesEXT r2Enable{};
        if (hasExt(vk::EXTRobustness2ExtensionName)) {
            if (r2F.robustBufferAccess2) {
                r2Enable.robustBufferAccess2 = vk::True;
                baseFeatures.features.robustBufferAccess = vk::True;
            }
            if (r2F.robustImageAccess2) r2Enable.robustImageAccess2 = vk::True;
            if (r2F.nullDescriptor) r2Enable.nullDescriptor = vk::True;
            v14Enable.pNext = &r2Enable;
        }

        vk::DeviceCreateInfo deviceInfo;
        deviceInfo.setPNext(&baseFeatures)
                  .setQueueCreateInfoCount(static_cast<std::uint32_t>(queueInfos.size()))
                  .setPQueueCreateInfos(queueInfos.data())
                  .setEnabledExtensionCount(static_cast<std::uint32_t>(m_deviceExtensions.size()))
                  .setPpEnabledExtensionNames(m_deviceExtensions.data());

        Log::Info("[VKContext] Creating logical device...");
        m_device = vk::raii::Device(m_physicalDevice, deviceInfo);
        Log::Info("[VKContext] Logical device created");

        m_graphicsQueue = vk::raii::Queue(m_device, m_queueFamilyIndices.graphicsFamily.value(), 0);
        m_presentQueue = vk::raii::Queue(m_device, m_queueFamilyIndices.presentFamily.value(), 0);
        m_computeQueue = vk::raii::Queue(m_device, m_queueFamilyIndices.computeFamily.value(), 0);
        m_transferQueue = vk::raii::Queue(m_device, m_queueFamilyIndices.transferFamily.value(), 0);

        m_descriptorIndexingEnabled = v12Enable.shaderSampledImageArrayNonUniformIndexing;

        Log::Info("[VKContext] Creating VMA allocator...");

        vma::AllocatorCreateInfo allocatorInfo{};
        allocatorInfo.setPhysicalDevice(*m_physicalDevice)
                     .setInstance(*m_instance)
                     .setDevice(*m_device)
                     .setVulkanApiVersion(vk::ApiVersion14);

        vma::raii::Allocator allocatorResult = (vma::createAllocator(allocatorInfo),
            vma::raii::detail::wrap<vma::raii::Allocator>(
                *m_device,
                vma::raii::detail::placeholder,
                allocatorInfo.pAllocationCallbacks,
                m_device.getDispatcher()));

        if (allocatorResult == nullptr) {
            Log::Error("[VKContext] VMA allocator creation failed");
            return false;
        }
        m_vmaAllocator = std::move(allocatorResult);
        Log::Info("[VKContext] VMA allocator created");

        return true;
    } catch (const std::exception& e) {
        Log::Error("[VKContext] CreateLogicalDevice exception: {}", e.what());
        return false;
    }
}

}  // namespace Crumb::Render