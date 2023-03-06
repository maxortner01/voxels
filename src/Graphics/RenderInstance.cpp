#include "livre/livre.h"

namespace livre
{
namespace Graphics
{
    bool RenderInstance::_validationSupport() const
    {
        // For now only this validation layer. Do an allocation for 
        // an array if others are needed.
        const char* validationLayer = "VK_LAYER_KHRONOS_validation";

        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        VkLayerProperties* availableLayers = (VkLayerProperties*)std::malloc(sizeof(VkLayerProperties) * layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers);

        for (int i = 0; i < layerCount; i++)
            if (strcmp(availableLayers[i].layerName, validationLayer))
            { std::free(availableLayers); return true; }
        
        std::free(availableLayers);
        return false;
    }

#ifdef LIVRE_LOGGING
    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr) {
            return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
        } else {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr) {
            func(instance, debugMessenger, pAllocator);
        }
    }

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData) {

        auto logger = spdlog::get("vulkan");

        switch(messageSeverity)
        {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            logger->trace(pCallbackData->pMessage); break;

        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            logger->info(pCallbackData->pMessage); break;
        
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            logger->warn(pCallbackData->pMessage); break;

        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            logger->error(pCallbackData->pMessage); break;
        };

        return VK_FALSE;
    }

    void RenderInstance::_initDebugMessenger()
    {
        auto logger = spdlog::get("vulkan");

        VkDebugUtilsMessengerEXT messenger;
        
        VkDebugUtilsMessengerCreateInfoEXT createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;
        createInfo.pUserData = nullptr; // Optional

        if (CreateDebugUtilsMessengerEXT((VkInstance)instance, &createInfo, nullptr, &messenger) != VK_SUCCESS) {
            ERROR_LOG("Failed to setup debug messenger!");
            return;
        }

        debugMessenger = messenger;
        INFO_LOG("Debug messenger successfully created.");
    }
#else
    void RenderInstance::_initDebugMessenger() { debugMessenger = nullptr; return; }
#endif

    struct QueueFamilyIndices
    {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;
    };

    QueueFamilyIndices _findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface)
    {
#   ifdef LIVRE_LOGGING
        auto logger = spdlog::get("vulkan");
#   endif

        QueueFamilyIndices queueFamilies;

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        if (!queueFamilyCount) { ERROR_LOG("No queue families for this device!"); return queueFamilies; }

        VkQueueFamilyProperties* properties = (VkQueueFamilyProperties*)std::malloc(sizeof(VkQueueFamilyProperties) * queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, properties);

        for (int i = 0; i < queueFamilyCount; i++)
        {
            if (properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
                queueFamilies.graphicsFamily = i;

            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
            if (presentSupport) queueFamilies.presentFamily = i;
        }

        std::free(properties);

        return queueFamilies;
    }

    const uint32_t requiredCount = 1;
    const char* requiredExtensions[] = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    void RenderInstance::_pickPhysicalDevice()
    {
#   ifdef LIVRE_LOGGING
        auto logger = spdlog::get("vulkan");
#   endif

        VkPhysicalDevice device = VK_NULL_HANDLE;

        uint32_t deviceCount;
        vkEnumeratePhysicalDevices((VkInstance)instance, &deviceCount, nullptr);

        if (!deviceCount) { ERROR_LOG("No devices with Vulkan support!"); return; }

        VkPhysicalDevice* devices = (VkPhysicalDevice*)std::malloc(sizeof(VkPhysicalDevice) * deviceCount);
        vkEnumeratePhysicalDevices((VkInstance)instance, &deviceCount, devices);

        for (int i = 0; i < deviceCount; i++)
        {
            // Physical properties
            VkPhysicalDeviceProperties deviceProperties;
            vkGetPhysicalDeviceProperties(devices[i], &deviceProperties);

            // Make sure the queues are present
            QueueFamilyIndices queueFamilies = _findQueueFamilies(devices[i], (VkSurfaceKHR)surface);

            // Make sure required extensions are supported
            bool extSupported = false;
            uint32_t extensionCount;
            vkEnumerateDeviceExtensionProperties(devices[i], nullptr, &extensionCount, nullptr);
            
            VkExtensionProperties* extProperties = (VkExtensionProperties*)std::malloc(sizeof(VkExtensionProperties) * extensionCount);
            vkEnumerateDeviceExtensionProperties(devices[i], nullptr, &extensionCount, extProperties);

            // Go through each of the supported extensions and make sure the 
            // required extensions are a subset of that 
            bool missingExt = false;
            for (int i = 0; i < requiredCount; i++)
            {
                bool found = false;
                for (int j = 0; j < extensionCount; j++)
                    if (strcmp(extProperties[j].extensionName, requiredExtensions[i])) { found = true; break; }

                if (!found) { missingExt = true; break; }
            }

            // If we're not missing any then the extensions are supported
            if (!missingExt) extSupported = true;

            std::free(extProperties);

            // See if the device has the graphics and presentation queue family as well as the required extensions
            if (queueFamilies.graphicsFamily.has_value() && queueFamilies.presentFamily.has_value() && extSupported)
                device = devices[i];
        }

        if (device == VK_NULL_HANDLE) { ERROR_LOG("No suitable devices found!"); return; }

        physicalDevice = device;

        std::free(devices);
        INFO_LOG("Physical device selected successfully.");
    }

    void RenderInstance::_initLogicalDevice()
    {
#   ifdef LIVRE_LOGGING
        auto logger = spdlog::get("vulkan");
#   endif 

        VkDevice device;

        QueueFamilyIndices indices = _findQueueFamilies((VkPhysicalDevice)physicalDevice, (VkSurfaceKHR)surface);

        const uint32_t uniqueQueueFamilies[] = {
            indices.graphicsFamily.value(),
            indices.presentFamily.value()
        };
        uint32_t uniqueFamiliesCount = sizeof(uniqueQueueFamilies) / sizeof(uint32_t);
        if (uniqueQueueFamilies[1] == uniqueQueueFamilies[0]) uniqueFamiliesCount--;

        VkDeviceQueueCreateInfo* queueCreateInfos = (VkDeviceQueueCreateInfo*)std::malloc(sizeof(VkDeviceQueueCreateInfo) * uniqueFamiliesCount);
        std::memset(queueCreateInfos, 0, sizeof(VkDeviceQueueCreateInfo) * uniqueFamiliesCount);

        float queuePriority = 1.0f;
        for (int i = 0; i < uniqueFamiliesCount; i++)
        {
            queueCreateInfos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfos[i].queueFamilyIndex = uniqueQueueFamilies[i];
            queueCreateInfos[i].queueCount = 1;
            queueCreateInfos[i].pQueuePriorities = &queuePriority;
        }

        VkPhysicalDeviceFeatures deviceFeatures{};

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

        createInfo.pQueueCreateInfos = queueCreateInfos;
        createInfo.queueCreateInfoCount = uniqueFamiliesCount;

        createInfo.pEnabledFeatures = &deviceFeatures;

        int present_index = 0;

#   ifdef __APPLE__
        int extra_index = 1;
#   else
        int extra_index = 0;
#   endif
        
        const char** devExtensions = (const char**)std::malloc(sizeof(const char*) * (requiredCount + extra_index));
        std::memcpy(devExtensions, requiredExtensions, sizeof(const char*) * requiredCount);

#   ifdef __APPLE__
        devExtensions[requiredCount + present_index++] = "VK_KHR_portability_subset";
#   endif

        createInfo.ppEnabledExtensionNames = devExtensions;
        createInfo.enabledExtensionCount = requiredCount + extra_index;

#   ifdef LIVRE_LOGGING
        const char** validation = (const char**)std::malloc(sizeof(const char*));
        validation[0] = "VK_LAYER_KHRONOS_validation";

        createInfo.enabledLayerCount = 1;
        createInfo.ppEnabledLayerNames = validation;
#   else
        createInfo.enabledLayerCount = 0;
#   endif
        
    
        TRACE_LOG("Creating logical device...");
        VkResult result = vkCreateDevice((VkPhysicalDevice)physicalDevice, &createInfo, nullptr, &device);
        TRACE_LOG("...done");

        std::free(queueCreateInfos);

#   ifdef LIVRE_LOGGING
        if (result != VK_SUCCESS) { ERROR_LOG("Failed to create logical device!"); return; }
        std::free(validation);
        std::free(devExtensions);
#   endif

        logicalDevice = device;

        VkQueue present;
        TRACE_LOG("Retreiving present queue...");
        vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &present);
        presentationQueue = present;
        TRACE_LOG("...done");

        VkQueue graphics;
        TRACE_LOG("Retreiving graphics queue...");
        vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphics);
        graphicsQueue = graphics;
        TRACE_LOG("...done");

        INFO_LOG("Logical device set up successfully.");
    }

    void RenderInstance::_initSurface(void* window)
    {
#   ifdef LIVRE_LOGGING
        auto logger = spdlog::get("vulkan");
#   endif

        VkSurfaceKHR _surface;

        VkResult result = glfwCreateWindowSurface((VkInstance)instance, (GLFWwindow*)window, nullptr, &_surface);
#   ifdef LIVRE_LOGGING
        if (result != VK_SUCCESS) { ERROR_LOG("GLFW failed to create window surface."); return; }
#   endif

        surface = _surface;

        INFO_LOG("Surface initialized successfully.");
    }

    RenderInstance::RenderInstance(void* window, const std::string& title)
    {
#   ifdef LIVRE_LOGGING
        auto logger = spdlog::get("vulkan");
#   endif

        VkInstance inst;

        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        INFO_LOG("{} vulkan extensions supported.", extensionCount);
        
        // Setting up the application info
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = title.c_str();
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "livre";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        // Collecting parameters to create instance
        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        // Getting extensions from glfw
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;

        // Actually get the instance string from glfw
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        int present_index = 0;
        int extra_index = 0;

#   ifdef __APPLE__
        extra_index += 2;
#   endif

#   ifdef LIVRE_LOGGING
        extra_index += 1;
#   endif   
        
        // Allocate a new string list that includes 
        const char** requiredExtensions = (const char**)std::malloc(sizeof(const char*) * (glfwExtensionCount + extra_index));
        std::memcpy(requiredExtensions, glfwExtensions, sizeof(const char*) * glfwExtensionCount);

        // For apple devices we need to add the portability enumeration extension name
#   ifdef __APPLE__
        TRACE_LOG("Apple device being used.");
        requiredExtensions[glfwExtensionCount + present_index++] = VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME;
        requiredExtensions[glfwExtensionCount + present_index++] = "VK_KHR_get_physical_device_properties2";
        createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#   endif
#   ifdef LIVRE_LOGGING
        requiredExtensions[glfwExtensionCount + present_index++] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
#   endif

        createInfo.enabledExtensionCount = glfwExtensionCount + extra_index;
        createInfo.ppEnabledExtensionNames = requiredExtensions;


        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
        // Validation layer things
#   ifdef LIVRE_LOGGING
        TRACE_LOG("Checking validation layers.");
        if (!_validationSupport())
            ERROR_LOG("Validation layers not supoorted");
        else
            INFO_LOG("Validation layer supported");

        const char** validation = (const char**)std::malloc(sizeof(const char*));
        validation[0] = "VK_LAYER_KHRONOS_validation";

        debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        debugCreateInfo.pfnUserCallback = debugCallback;

        // If other validation layers needed, have a const object
        // that is used both here and in _validationSupport()
        createInfo.enabledLayerCount = 1;
        createInfo.ppEnabledLayerNames = validation;
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;

        INFO_LOG("Added validation layers.");
#   else
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
#   endif

        // create the instance
        VkResult result = vkCreateInstance(&createInfo, nullptr, &inst);
#   ifdef LIVRE_LOGGING
        if (result != VK_SUCCESS) ERROR_LOG("Failed to create instance ({})!", string_VkResult(result));
#   endif

        if (result == VK_SUCCESS) INFO_LOG("Vulkan instance created successfully.");
        // If the VkInstanceCreateinfo object is needed after the instance is created
        // then DO NOT free here... otherwise, we'll wait for errors
        std::free(requiredExtensions);
#   ifdef LIVRE_LOGGING
        std::free(validation);
#   endif

        instance = inst;

        TRACE_LOG("Setting up render surface.");
        _initSurface(window);

        TRACE_LOG("Setting up debug messenger.");
        _initDebugMessenger();

        TRACE_LOG("Setting up physical devices.");
        _pickPhysicalDevice();

        TRACE_LOG("Setting up logical device.");
        _initLogicalDevice();
    }

    RenderInstance::~RenderInstance()
    {
#   ifdef LIVRE_LOGGING
        auto logger = spdlog::get("vulkan");

        TRACE_LOG("Destroying debug messenger...");
        DestroyDebugUtilsMessengerEXT((VkInstance)instance, (VkDebugUtilsMessengerEXT)debugMessenger, nullptr);
        TRACE_LOG("... done");
#   endif

        TRACE_LOG("Destroying surface...");
        vkDestroySurfaceKHR((VkInstance)instance, (VkSurfaceKHR)surface, nullptr);
        TRACE_LOG("...done");

        TRACE_LOG("Destroying logical device...");
        vkDestroyDevice((VkDevice)logicalDevice, nullptr);
        TRACE_LOG("...done");

        TRACE_LOG("Destroying instance...");
        vkDestroyInstance((VkInstance)instance, nullptr);
        TRACE_LOG("...done");
    }
}
}