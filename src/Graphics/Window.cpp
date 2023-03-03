#include "livre/livre.h"

#include <iostream>

#define win GLFWwindow*

namespace livre
{
    Window::Window(uint16_t width, uint16_t height)
    {
        // Init GLFW and create window
        glfwInit();
        window = (void*)glfwCreateWindow(width, height, "", NULL, NULL);

        // Make sure window is all good and make context current
        if (window == NULL)
        {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return;
        }
        glfwMakeContextCurrent((win)window);

        // Set up GLEW and get it ready (must be done after context is made current)
        uint32_t code = glewInit();
        if (code != GLEW_OK)
        {
            std::cout << "Glew Failed to init\n code: " << code <<"\n";
        }
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