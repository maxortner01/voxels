#pragma once

#include "livre/header.h"

namespace livre
{
    class DLLOUT Window
    {
        void* window;

        std::string title;

    public:
        Window(uint16_t width, uint16_t height);

        void setTitle(const std::string& title);

        const bool open() const;
        const void pollEvents() const;
        const void update() const;
    };
}