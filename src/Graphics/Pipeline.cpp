#include "livre/livre.h"

namespace livre
{
    Pipeline::Pipeline(const Renderer& renderer) :
        _renderer(renderer), shaders(nullptr), _pipeline(nullptr), _pipelineLayout(nullptr)
    { }

    Pipeline::~Pipeline() 
    {
#   ifdef LIVRE_LOGGING
        auto logger = spdlog::get("vulkan");
#   endif

        if (shaders)
        {
            for (int i = 0; i < shader_count; i++)
                delete shaders[i];

            std::free(shaders);
            shaders = nullptr;
        }

        if (_pipelineLayout)
        {
            TRACE_LOG("Destroying pipeline layout...");
            vkDestroyPipelineLayout((VkDevice)_renderer.getInstance().getLogicalDevice(), (VkPipelineLayout)_pipelineLayout, nullptr);
            TRACE_LOG("...done");
            _pipelineLayout = nullptr;
        }
    }

    const void* Pipeline::getPipeline() const 
    { return _pipeline; }

    bool Pipeline::isComplete() const
    { return (_pipeline != nullptr && _pipelineLayout != nullptr); }

    Shader* Pipeline::getShader(const Shader::TYPE& type)
    {
        for (int i = 0; i < shader_count; i++)
            if (shaders[i]->getType() == type)
                return shaders[i];

        return nullptr;
    }
}