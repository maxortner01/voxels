#pragma once

namespace livre
{
    struct Color
    {
        float r, g, b, a;

        Color(float _r, float _g, float _b, float _a = 1.f) :
            r(_r), g(_g), b(_b), a(_a)
        {  }
    };
}