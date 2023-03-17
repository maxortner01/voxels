#include "livre/livre.h"

#include <iostream>

#define win GLFWwindow*

namespace livre
{
    Window::Window(uint16_t width, uint16_t height) :
        instance(nullptr)
    {
        // Init GLFW and create window
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        window = (void*)glfwCreateWindow(width, height, "", NULL, NULL);
        
        // Initialize spdlog
#       ifdef LIVRE_LOGGING
        spdlog::stdout_color_st("livre");
        spdlog::stdout_color_st("vulkan");
        spdlog::set_level(spdlog::level::trace);
        auto logger = spdlog::get("livre");
#       endif

        // Make sure window is all good and make context current
        if (window == NULL)
        {
#       ifdef LIVRE_LOGGING
            logger->error("Window creation failed!");
#       endif
            glfwTerminate();
            return;
        }
#       ifdef LIVRE_LOGGING
        else
            logger->info("Window created successfully!");
#       endif

        // Initialize the render instance
        instance = new Graphics::RenderInstance(window);
    }

    Window::~Window()
    {
#   ifdef LIVRE_LOGGING
        auto logger = spdlog::get("livre");
#   endif

        if (instance)
        {
            delete instance;
            instance = nullptr;
        }

        if (window)
        {
            TRACE_LOG("Destroying window...");
            glfwDestroyWindow((win)window);
            glfwTerminate();
            TRACE_LOG("...done");
            window = nullptr;
        }
    }

    void Window::clear() const
    {
        
    }
    
    void Window::setTitle(const std::string& title)
    {
        glfwSetWindowTitle((win)window, title.c_str());
    }

    const bool Window::open() const
    {
        return !glfwWindowShouldClose((win)window);
    }

    const void Window::pollEvents() const
    {
        glfwPollEvents();
    }

    const void Window::update() const
    {
        glfwSwapBuffers((win)window);
    }

    const Graphics::RenderInstance& Window::getInstance() const
    {
        return *instance;
    }

    void Window::waitForIdle() const
    {
        vkDeviceWaitIdle((VkDevice)instance->getLogicalDevice());
    }
} 

#undef win