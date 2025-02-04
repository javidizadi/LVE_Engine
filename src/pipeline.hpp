#pragma once
#include "device.hpp"

#include <cstdint>
#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace lve {

struct PipelineConfigInfo {
  VkViewport viewport;
  VkRect2D scissor;
  VkPipelineViewportStateCreateInfo viewportInfo;
  VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
  VkPipelineRasterizationStateCreateInfo rasterizationInfo;
  VkPipelineMultisampleStateCreateInfo multisampleInfo;
  VkPipelineColorBlendAttachmentState colorBlendAttachment;
  VkPipelineColorBlendStateCreateInfo colorBlendInfo;
  VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
  VkPipelineLayout pipelineLayout = nullptr;
  VkRenderPass renderPass = nullptr;
  uint32_t subpass = 0;
};

class Pipeline {

private:
  Device &device;
  VkPipeline graphicsPipeline;
  VkShaderModule vertexShaderModule;
  VkShaderModule fragmentShaderModule;

  static std::vector<char> readFile(const std::string &filePath);

  void createGraphicsPipeline(Device &device, const std::string &vertexFilePath,
                              const std::string &fragmentFilePath,
                              const PipelineConfigInfo &configInfo);

  void createShaderModule(const std::vector<char> &code,
                          VkShaderModule *shaderModule);

public:
  Pipeline(Device &device, const std::string &vertexFilePath,
           const std::string &fragmentFilePath,
           const PipelineConfigInfo &configInfo);
  ~Pipeline();

  Pipeline(const Pipeline &) = delete;
  Pipeline &operator=(const Pipeline &) = delete;

  static PipelineConfigInfo createDefaultPipelineConfig(uint32_t width,
                                                        uint32_t height);
};

}; // namespace lve
