#include "pipeline.hpp"
#include "model.hpp"

#include <cassert>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>

#include <vulkan/vulkan_core.h>

namespace lve {

std::vector<char> Pipeline::readFile(const std::string &filePath) {
  std::ifstream file(filePath, std::ios::ate | std::ios::binary);

  if (!file.is_open())
    throw std::runtime_error("Failed to open file: " + filePath);

  size_t fileSize = static_cast<size_t>(file.tellg());
  file.seekg(0);

  std::vector<char> buffer(fileSize);
  file.read(buffer.data(), fileSize);
  file.close();

  return buffer;
};

PipelineConfigInfo Pipeline::createDefaultPipelineConfig(uint32_t width,
                                                         uint32_t height) {
  PipelineConfigInfo configInfo{};

  configInfo.inputAssemblyInfo = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
      .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
      .primitiveRestartEnable = VK_FALSE,
  };

  configInfo.viewport = VkViewport{
      .x = 0.0f,
      .y = 0.0f,
      .width = static_cast<float>(width),
      .height = static_cast<float>(height),
      .minDepth = 0.0f,
      .maxDepth = 1.0f,
  };

  configInfo.scissor = VkRect2D{
      .offset = VkOffset2D{.x = 0, .y = 0},
      .extent = VkExtent2D{.width = width, .height = height},
  };

  configInfo.rasterizationInfo = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
      .depthClampEnable = VK_FALSE,
      .rasterizerDiscardEnable = VK_FALSE,
      .polygonMode = VK_POLYGON_MODE_FILL,
      .cullMode = VK_CULL_MODE_NONE,
      .frontFace = VK_FRONT_FACE_CLOCKWISE,
      .depthBiasEnable = VK_FALSE,
      .depthBiasConstantFactor = 0.0f,
      .depthBiasClamp = 0.0f,
      .depthBiasSlopeFactor = 0.0f,
      .lineWidth = 1.0f,
  };

  configInfo.multisampleInfo = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
      .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
      .sampleShadingEnable = VK_FALSE,
      .minSampleShading = 1.0f,
      .pSampleMask = nullptr,
      .alphaToCoverageEnable = VK_FALSE,
      .alphaToOneEnable = VK_FALSE,
  };

  configInfo.colorBlendAttachment = VkPipelineColorBlendAttachmentState{
      .blendEnable = VK_FALSE,
      .srcColorBlendFactor = VK_BLEND_FACTOR_ONE,
      .dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
      .colorBlendOp = VK_BLEND_OP_ADD,
      .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
      .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
      .alphaBlendOp = VK_BLEND_OP_ADD,
      .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
  };

  configInfo.colorBlendInfo = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
      .logicOpEnable = VK_FALSE,
      .logicOp = VK_LOGIC_OP_COPY,
      .attachmentCount = 1,
      .pAttachments = &configInfo.colorBlendAttachment,
      .blendConstants = {0.0f, 0.0f, 0.0f},
  };

  configInfo.depthStencilInfo = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
      .depthTestEnable = VK_TRUE,
      .depthWriteEnable = VK_TRUE,
      .depthCompareOp = VK_COMPARE_OP_LESS,
      .depthBoundsTestEnable = VK_FALSE,
      .stencilTestEnable = VK_FALSE,
      .front = {},
      .back = {},
      .minDepthBounds = 0.0f,
      .maxDepthBounds = 1.0f,
  };

  return configInfo;
}

void Pipeline::createShaderModule(const std::vector<char> &code,
                                  VkShaderModule *shaderModule) {

  VkShaderModuleCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize = code.size();
  createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());

  if (vkCreateShaderModule(device.device(), &createInfo, nullptr,
                           shaderModule) != VK_SUCCESS)
    throw std::runtime_error("Failed to create shader module.");
}

void Pipeline::createGraphicsPipeline(Device &device,
                                      const std::string &vertexFilePath,
                                      const std::string &fragmentFilePath,
                                      const PipelineConfigInfo &configInfo) {

  assert(configInfo.pipelineLayout != VK_NULL_HANDLE &&
         "failed to create graphics pipeline:: no pipelineLayout provided.");

  assert(configInfo.renderPass != VK_NULL_HANDLE &&
         "failed to create graphics pipeline:: no renderPass provided.");

  auto vertex = readFile(vertexFilePath);
  auto fragment = readFile(fragmentFilePath);

  createShaderModule(vertex, &vertexShaderModule);
  createShaderModule(fragment, &fragmentShaderModule);

  VkPipelineShaderStageCreateInfo shaderStages[2];
  shaderStages[0] = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .stage = VK_SHADER_STAGE_VERTEX_BIT,
      .module = vertexShaderModule,
      .pName = "main",
      .pSpecializationInfo = nullptr,
  };

  shaderStages[1] = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
      .module = fragmentShaderModule,
      .pName = "main",
      .pSpecializationInfo = nullptr,
  };

  auto bindingDescriptions = Model::Vertex::getBindingDescriptions();
  auto attributeDescriptions = Model::Vertex::getAttributeDescriptions();
  VkPipelineVertexInputStateCreateInfo vertexInputInfo{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,

      .vertexBindingDescriptionCount =
          static_cast<uint32_t>(bindingDescriptions.size()),

      .pVertexBindingDescriptions = bindingDescriptions.data(),

      .vertexAttributeDescriptionCount =
          static_cast<uint32_t>(attributeDescriptions.size()),

      .pVertexAttributeDescriptions = attributeDescriptions.data(),
  };

  VkPipelineViewportStateCreateInfo viewportInfo{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
      .viewportCount = 1,
      .pViewports = &configInfo.viewport,
      .scissorCount = 1,
      .pScissors = &configInfo.scissor,
  };

  VkGraphicsPipelineCreateInfo pipelineInfo{
      .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
      .stageCount = 2,
      .pStages = shaderStages,
      .pVertexInputState = &vertexInputInfo,
      .pInputAssemblyState = &configInfo.inputAssemblyInfo,
      .pViewportState = &viewportInfo,
      .pRasterizationState = &configInfo.rasterizationInfo,
      .pMultisampleState = &configInfo.multisampleInfo,
      .pDepthStencilState = &configInfo.depthStencilInfo,
      .pColorBlendState = &configInfo.colorBlendInfo,
      .pDynamicState = nullptr,
      .layout = configInfo.pipelineLayout,
      .renderPass = configInfo.renderPass,
      .subpass = configInfo.subpass,
      .basePipelineHandle = VK_NULL_HANDLE,
      .basePipelineIndex = -1,
  };

  if (vkCreateGraphicsPipelines(device.device(), VK_NULL_HANDLE, 1,
                                &pipelineInfo, nullptr,
                                &graphicsPipeline) != VK_SUCCESS)
    throw std::runtime_error("Failed to cerate graphics pipelines.");
}

Pipeline::Pipeline(Device &device, const std::string &vertexFilePath,
                   const std::string &fragmentFilePath,
                   const PipelineConfigInfo &configInfo)
    : device(device) {
  createGraphicsPipeline(device, vertexFilePath, fragmentFilePath, configInfo);
}

Pipeline::~Pipeline() {
  vkDestroyShaderModule(device.device(), vertexShaderModule, nullptr);
  vkDestroyShaderModule(device.device(), fragmentShaderModule, nullptr);
  vkDestroyPipeline(device.device(), graphicsPipeline, nullptr);
}

void Pipeline::bind(VkCommandBuffer commandBuffer) {
  vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                    graphicsPipeline);
}

} // namespace lve
