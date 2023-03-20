#include "livre/livre.h"

namespace livre
{
    void GraphicsPipeline::_initShaders()
    {
        shader_count = 2;
        shaders = (Shader**)LIVRE_ALLOC(sizeof(Shader*) * shader_count);
        
        shaders[0] = new Shader(_renderer.getInstance(), Shader::TYPE::VERTEX);
        shaders[1] = new Shader(_renderer.getInstance(), Shader::TYPE::FRAGMENT);
    }

    Pipeline::STATUS GraphicsPipeline::_createLayouts() 
    {
#   ifdef LIVRE_LOGGING
        auto logger = spdlog::get("vulkan");
#   endif
        
        TRACE_LOG("Creating graphice pipeline layout.");
        
        VkDynamicState dynamicStates[] = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };

        VkPipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount = 2;
        dynamicState.pDynamicStates = &dynamicStates[0];

        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = 0;
        vertexInputInfo.pVertexBindingDescriptions = nullptr; // Optional
        vertexInputInfo.vertexAttributeDescriptionCount = 0;
        vertexInputInfo.pVertexAttributeDescriptions = nullptr; // Optional

        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        const VkExtent2D extent = *(const VkExtent2D*)_renderer.getInstance().getSwapChainImages().extent;

        // Dynamic pipeline info
        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float) extent.width;
        viewport.height = (float) extent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = extent;

        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.pViewports = &viewport;
        viewportState.scissorCount = 1;
        viewportState.pScissors = &scissor;

        VkPolygonMode poly_mode;
        if      (_mode == POINTS)    poly_mode = VK_POLYGON_MODE_POINT;
        else if (_mode == FILL)      poly_mode = VK_POLYGON_MODE_FILL;
        else if (_mode == WIREFRAME) poly_mode = VK_POLYGON_MODE_LINE;

        VkPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = poly_mode;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;
        rasterizer.depthBiasConstantFactor = 0.0f; // Optional
        rasterizer.depthBiasClamp = 0.0f; // Optional
        rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisampling.minSampleShading = 1.0f; // Optional
        multisampling.pSampleMask = nullptr; // Optional
        multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
        multisampling.alphaToOneEnable = VK_FALSE; // Optional

        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_TRUE;
        colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
        colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f; // Optional
        colorBlending.blendConstants[1] = 0.0f; // Optional
        colorBlending.blendConstants[2] = 0.0f; // Optional
        colorBlending.blendConstants[3] = 0.0f; // Optional

        VkPipelineLayout pipelineLayout;

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0; // Optional
        pipelineLayoutInfo.pSetLayouts = nullptr; // Optional
        pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
        pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

        VkResult result = vkCreatePipelineLayout((VkDevice)_renderer.getInstance().getLogicalDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout);

        if (result != VK_SUCCESS)
#   ifdef LIVRE_LOGGING
        { ERROR_LOG("Graphics pipeline failed to be created!"); return PIPELINE_LAYOUT_CREATE_FAILED; }
#   else
            return PIPELINE_LAYOUT_CREATE_FAILED;
#   endif

        _pipelineLayout = pipelineLayout;
        INFO_LOG("Graphics pipeline layout created successfully.");

    // HERE
        //STATUS render = _createRenderPass();
        //if (render != SUCCESS) return render;

        VkPipelineShaderStageCreateInfo* shaderStages = (VkPipelineShaderStageCreateInfo*)LIVRE_ALLOC(sizeof(VkPipelineShaderStageCreateInfo) * 2);
        getVertexShader()  .getShaderStageInfo((void*)(shaderStages + 0));
        getFragmentShader().getShaderStageInfo((void*)(shaderStages + 1));

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = 2;
        pipelineInfo.pStages = shaderStages;

        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pDepthStencilState = nullptr; // Optional
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.pDynamicState = &dynamicState;

        pipelineInfo.layout = (VkPipelineLayout)_pipelineLayout;

        pipelineInfo.renderPass = (VkRenderPass)_renderer.getRenderPass();
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
        pipelineInfo.basePipelineIndex = -1; // Optional

        VkPipeline graphicsPipeline;
        TRACE_LOG("Creating graphics pipeline.");
        result = vkCreateGraphicsPipelines((VkDevice)_renderer.getInstance().getLogicalDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline);

        if (result != VK_SUCCESS)
#   ifdef LIVRE_LOGGING
        { ERROR_LOG("Pipeline creation failed!"); return PIPELINE_CREATE_FAILED; }
#   else
            return PIPELINE_CREATE_FAILED;
#   endif

        _pipeline = graphicsPipeline;

        std::free(shaderStages);

        for (int i = 0; i < shader_count; i++)
            delete shaders[i];
        std::free(shaders);
        shaders = nullptr;

        //STATUS status = _createFramebuffers();
        //if (status != SUCCESS) return status;

        return SUCCESS;
    }

    GraphicsPipeline::GraphicsPipeline(const Renderer& instance, const BufferArray* bufferArray) :
        Pipeline(instance), _mode(FILL), _bufferArray(bufferArray)
    { _initShaders(); }

    GraphicsPipeline::~GraphicsPipeline()
    {
#   ifdef LIVRE_LOGGING
        auto logger = spdlog::get("vulkan");
#   endif

        //if (_commandPool)
        //{
        //    TRACE_LOG("Destroying command pool...");
        //    vkDestroyCommandPool((VkDevice)_renderer.getInstance().getLogicalDevice(), (VkCommandPool)_commandPool, nullptr);
        //    TRACE_LOG("...done");
        //    _commandPool = nullptr;
        //}

        if (_pipeline)
        {
            TRACE_LOG("Destroying pipeline...");
            vkDestroyPipeline((VkDevice)_renderer.getInstance().getLogicalDevice(), (VkPipeline)_pipeline, nullptr);
            TRACE_LOG("...done");
            _pipeline = nullptr;
        }
    }

    void GraphicsPipeline::setMode(const MODE& mode)
    { _mode = mode; }

    GraphicsPipeline::MODE GraphicsPipeline::getMode() const 
    { return _mode; }

    Shader& GraphicsPipeline::getVertexShader()
    { return *getShader(Shader::TYPE::VERTEX); }

    Shader& GraphicsPipeline::getFragmentShader()
    { return *getShader(Shader::TYPE::FRAGMENT); }

    Pipeline::STATUS GraphicsPipeline::create() 
    {
#   ifdef LIVRE_LOGGING
        auto logger = spdlog::get("livre");
#   endif

        if (!getVertexShader().isComplete() || !getFragmentShader().isComplete())
#   ifdef LIVRE_LOGGING
        { ERROR_LOG("Failed to create pipeline! Shader not complete!"); return SHADER_NOT_COMPLETE; }
#   else
            return SHADER_NOT_COMPLETE;
#   endif

        return _createLayouts();
    }
}