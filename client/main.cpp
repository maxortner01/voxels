#include "livre/livre.h"

#include <iostream>

using namespace livre;

int main()
{
    Window window(1280, 720);
    
    Shader shader(Shader::TYPE::VERTEX | Shader::TYPE::FRAGMENT);
    std::cout << shader.fromFile(Shader::TYPE::VERTEX, "test.txt") << "\n";

    window.setTitle("Main");

    while (window.open())
    {
        window.pollEvents();
        window.update();
    }

    return 0;
}