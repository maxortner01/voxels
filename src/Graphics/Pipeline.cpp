#include "livre/livre.h"

namespace livre
{
    Pipeline::Pipeline(const Graphics::RenderInstance& instance) :
        InstanceObject(instance), shaders(nullptr)
    { }

    Pipeline::~Pipeline() 
    {
        if (shaders)
        {
            for (int i = 0; i < shader_count; i++)
                delete shaders[i];

            std::free(shaders);
            shaders = nullptr;
        }
    }

    Shader* Pipeline::getShader(const Shader::TYPE& type)
    {
        for (int i = 0; i < shader_count; i++)
            if (shaders[i]->getType() == type)
                return shaders[i];

        return nullptr;
    }
}