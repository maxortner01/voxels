#include "livre/livre.h"

#include <iostream>

using namespace livre;

int main()
{
    Window window(100, 100);

    while (window.open())
    {
        window.pollEvents();
        window.update();
    }

    return 0;
}