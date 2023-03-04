#include "livre/livre.h"

#include <iostream>

#define win GLFWwindow*

namespace livre
{
    Window::Window(uint16_t width, uint16_t height)
    {
        // Init GLFW and create window
        glfwInit();
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        window = (void*)glfwCreateWindow(width, height, "", NULL, NULL);
        
#       ifdef LIVRE_LOGGING
        spdlog::stdout_color_st("db_logger");
        spdlog::set_level(spdlog::level::trace);
        auto logger = spdlog::get("db_logger");
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

        TRACE_LOG("Making window context current.");
        glfwMakeContextCurrent((win)window);
        TRACE_LOG("...done");

        // Set up GLEW and get it ready (must be done after context is made current)
#   ifdef __APPLE__
        glewExperimental = GL_TRUE; 
#   endif

        uint32_t code = glewInit();
#   ifdef LIVRE_LOGGING
        if (code != GLEW_OK)
            logger->error("Glew failed to initialize (code {})", code);
        else
            logger->info("Glew initialized successfully!");
#   endif
    }

    void Window::clear() const
    {
        glClear(GL_COLOR_BUFFER_BIT);
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
} 

#undef win