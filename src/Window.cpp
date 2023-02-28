#include "livre/livre.h"

#include <GLFW/glfw3.h>
#include <iostream>

#define win GLFWwindow*

namespace livre
{
    Window::Window(uint16_t width, uint16_t height)
    {
        glfwInit();
        window = (void*)glfwCreateWindow(width, height, "", NULL, NULL);

        if (window == NULL)
        {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
        }
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