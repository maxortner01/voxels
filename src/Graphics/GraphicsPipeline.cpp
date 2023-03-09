#include "livre/livre.h"

namespace livre
{
    void GraphicsPipeline::_initShaders()
    {
        shader_count = 2;
        shaders = (Shader**)std::malloc(sizeof(Shader*) * shader_count);
        
        shaders[0] = new Shader(_instance, Shader::TYPE::VERTEX);
        shaders[1] = new Shader(_instance, Shader::TYPE::FRAGMENT);
    }

    Shader& GraphicsPipeline::getVertexShader()
    { return *getShader(Shader::TYPE::VERTEX); }

    Shader& GraphicsPipeline::getFragmentShader()
    { return *getShader(Shader::TYPE::FRAGMENT); }

    Pipeline::STATUS GraphicsPipeline::bind() 
    {
#   ifdef LIVRE_LOGGING
        auto logger = spdlog::get("livre");
#   endif

        if (!getVertexShader().isComplete() || !getFragmentShader().isComplete())
#   ifdef LIVRE_LOGGING
        { ERROR_LOG("Failed to bind pipeline! Shader not complete!"); return SHADER_NOT_COMPLETE; }
#   else
            return SHADER_NOT_COMPLETE;
#   endif

        VkPipelineShaderStageCreateInfo* shaderStages = (VkPipelineShaderStageCreateInfo*)std::malloc(sizeof(VkPipelineShaderStageCreateInfo) * 2);
        getVertexShader()  .getShaderStageInfo((void*)(shaderStages + 0));
        getFragmentShader().getShaderStageInfo((void*)(shaderStages + 1));
        
        std::free(shaderStages);

        return SUCCESS;
    }
}