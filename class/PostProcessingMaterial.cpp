#include "PostProcessingMaterial.h"
#include "../vulkan/RenderPass.h"
#include "../vulkan/GlobalDeviceObjects.h"
#include "../vulkan/CommandBuffer.h"
#include "../vulkan/PerFrameResource.h"
#include "../vulkan/Framebuffer.h"
#include "../vulkan/GlobalVulkanStates.h"
#include "../vulkan/SharedIndirectBuffer.h"
#include "../vulkan/DescriptorSet.h"
#include "../vulkan/Image.h"
#include "../vulkan/GraphicPipeline.h"
#include "../vulkan/SwapChain.h"
#include "../vulkan/DescriptorPool.h"
#include "../vulkan/DescriptorSetLayout.h"
#include "RenderPassBase.h"
#include "RenderPassDiction.h"
#include "RenderWorkManager.h"
#include "GBufferPass.h"
#include "FrameBufferDiction.h"
#include "../common/Util.h"

std::shared_ptr<PostProcessingMaterial> PostProcessingMaterial::CreateDefaultMaterial()
{
	SimpleMaterialCreateInfo simpleMaterialInfo = {};
	simpleMaterialInfo.shaderPaths = { L"../data/shaders/screen_quad.vert.spv", L"", L"", L"", L"../data/shaders/post_processing.frag.spv", L"" };
	simpleMaterialInfo.vertexFormat = VertexFormatNul;
	simpleMaterialInfo.vertexFormatInMem = VertexFormatNul;
	simpleMaterialInfo.subpassIndex = 0;
	simpleMaterialInfo.frameBufferType = FrameBufferDiction::FrameBufferType_PostProcessing;
	simpleMaterialInfo.pRenderPass = RenderPassDiction::GetInstance()->GetPipelineRenderPass(RenderPassDiction::PipelineRenderPassPostProcessing);
	simpleMaterialInfo.depthTestEnable = false;
	simpleMaterialInfo.depthWriteEnable = false;

	std::shared_ptr<PostProcessingMaterial> pPostProcessMaterial = std::make_shared<PostProcessingMaterial>();

	VkGraphicsPipelineCreateInfo createInfo = {};

	std::vector<VkPipelineColorBlendAttachmentState> blendStatesInfo;
	uint32_t colorTargetCount = (uint32_t)FrameBufferDiction::GetInstance()->GetFrameBuffer(simpleMaterialInfo.frameBufferType)->GetColorTargets().size();

	for (uint32_t i = 0; i < colorTargetCount; i++)
	{
		blendStatesInfo.push_back(
			{
				VK_FALSE,							// blend enabled

				VK_BLEND_FACTOR_SRC_ALPHA,			// src color blend factor
				VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,// dst color blend factor
				VK_BLEND_OP_ADD,					// color blend op

				VK_BLEND_FACTOR_ONE,				// src alpha blend factor
				VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,// dst alpha blend factor
				VK_BLEND_OP_ADD,					// alpha blend factor

				0xf,								// color mask
			}
		);
	}

	VkPipelineColorBlendStateCreateInfo blendCreateInfo = {};
	blendCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	blendCreateInfo.logicOpEnable = VK_FALSE;
	blendCreateInfo.attachmentCount = (uint32_t)blendStatesInfo.size();
	blendCreateInfo.pAttachments = blendStatesInfo.data();

	VkPipelineDepthStencilStateCreateInfo depthStencilCreateInfo = {};
	depthStencilCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencilCreateInfo.depthTestEnable = VK_FALSE;
	depthStencilCreateInfo.depthWriteEnable = VK_FALSE;

	VkPipelineInputAssemblyStateCreateInfo assemblyCreateInfo = {};
	assemblyCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	assemblyCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

	VkPipelineMultisampleStateCreateInfo multiSampleCreateInfo = {};
	multiSampleCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multiSampleCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	VkPipelineRasterizationStateCreateInfo rasterizerCreateInfo = {};
	rasterizerCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizerCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizerCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizerCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizerCreateInfo.lineWidth = 1.0f;
	rasterizerCreateInfo.depthClampEnable = VK_FALSE;
	rasterizerCreateInfo.rasterizerDiscardEnable = VK_FALSE;
	rasterizerCreateInfo.depthBiasEnable = VK_FALSE;

	VkPipelineViewportStateCreateInfo viewportStateCreateInfo = {};
	viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportStateCreateInfo.viewportCount = 1;
	viewportStateCreateInfo.pScissors = nullptr;
	viewportStateCreateInfo.scissorCount = 1;
	viewportStateCreateInfo.pViewports = nullptr;

	std::vector<VkDynamicState>	 dynamicStates =
	{
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};

	VkPipelineDynamicStateCreateInfo dynamicStatesCreateInfo = {};
	dynamicStatesCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicStatesCreateInfo.dynamicStateCount = (uint32_t)dynamicStates.size();
	dynamicStatesCreateInfo.pDynamicStates = dynamicStates.data();

	VkPipelineVertexInputStateCreateInfo vertexInputCreateInfo = {};
	vertexInputCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

	createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	createInfo.pColorBlendState = &blendCreateInfo;
	createInfo.pDepthStencilState = &depthStencilCreateInfo;
	createInfo.pInputAssemblyState = &assemblyCreateInfo;
	createInfo.pMultisampleState = &multiSampleCreateInfo;
	createInfo.pRasterizationState = &rasterizerCreateInfo;
	createInfo.pViewportState = &viewportStateCreateInfo;
	createInfo.pDynamicState = &dynamicStatesCreateInfo;
	createInfo.pVertexInputState = &vertexInputCreateInfo;
	createInfo.renderPass = simpleMaterialInfo.pRenderPass->GetRenderPass()->GetDeviceHandle();
	createInfo.subpass = simpleMaterialInfo.subpassIndex;

	if (pPostProcessMaterial.get() && pPostProcessMaterial->Init(pPostProcessMaterial, simpleMaterialInfo.shaderPaths, simpleMaterialInfo.pRenderPass, createInfo, simpleMaterialInfo.materialUniformVars, simpleMaterialInfo.vertexFormat, simpleMaterialInfo.vertexFormatInMem))
		return pPostProcessMaterial;

	return nullptr;
}

bool PostProcessingMaterial::Init(const std::shared_ptr<PostProcessingMaterial>& pSelf,
	const std::vector<std::wstring>	shaderPaths,
	const std::shared_ptr<RenderPassBase>& pRenderPass,
	const VkGraphicsPipelineCreateInfo& pipelineCreateInfo,
	const std::vector<UniformVar>& materialUniformVars,
	uint32_t vertexFormat,
	uint32_t vertexFormatInMem)
{
	if (!Material::Init(pSelf, shaderPaths, pRenderPass, pipelineCreateInfo, materialUniformVars, vertexFormat, vertexFormatInMem, false))
		return false;

	std::vector<CombinedImage> resultTargets;
	std::vector<CombinedImage> motionNeighborMaxs;
	for (uint32_t j = 0; j < GetSwapChain()->GetSwapChainImageCount(); j++)
	{
		std::shared_ptr<FrameBuffer> pCombineResult = FrameBufferDiction::GetInstance()->GetFrameBuffers(FrameBufferDiction::FrameBufferType_CombineResult)[j];

		resultTargets.push_back({
			pCombineResult->GetColorTarget(0),
			pCombineResult->GetColorTarget(0)->CreateLinearClampToEdgeSampler(),
			pCombineResult->GetColorTarget(0)->CreateDefaultImageView()
			});

		std::shared_ptr<FrameBuffer> pMotionNeighborMax = FrameBufferDiction::GetInstance()->GetFrameBuffers(FrameBufferDiction::FrameBufferType_MotionNeighborMax)[j];

		motionNeighborMaxs.push_back({
			pMotionNeighborMax->GetColorTarget(0),
			pMotionNeighborMax->GetColorTarget(0)->CreateLinearClampToEdgeSampler(),
			pMotionNeighborMax->GetColorTarget(0)->CreateDefaultImageView()
		});
	}

	m_pUniformStorageDescriptorSet->UpdateImages(MaterialUniformStorageTypeCount , resultTargets);
	m_pUniformStorageDescriptorSet->UpdateImages(MaterialUniformStorageTypeCount + 1, motionNeighborMaxs);

	return true;
}

void PostProcessingMaterial::CustomizeMaterialLayout(std::vector<UniformVarList>& materialLayout)
{
	m_materialVariableLayout.push_back(
	{
		CombinedSampler,
		"Combine Result",
		{},
		GetSwapChain()->GetSwapChainImageCount()
	});

	m_materialVariableLayout.push_back(
	{
		CombinedSampler,
		"Motion Neighbor Max",
		{},
		GetSwapChain()->GetSwapChainImageCount()
	});
}

void PostProcessingMaterial::CustomizePoolSize(std::vector<uint32_t>& counts)
{
	counts[VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER] += (GetSwapChain()->GetSwapChainImageCount() * 2);
}

void PostProcessingMaterial::AttachResourceBarriers(const std::shared_ptr<CommandBuffer>& pCmdBuffer, BarrierInsertionPoint barrierInsertionPoint, uint32_t pingpong)
{
	if (barrierInsertionPoint == Material::BarrierInsertionPoint::AFTER_DISPATCH)
		return;

	std::vector<VkImageMemoryBarrier> barriers;

	std::shared_ptr<Image> pCombineResult = FrameBufferDiction::GetInstance()->GetFrameBuffers(FrameBufferDiction::FrameBufferType_CombineResult)[FrameWorkMgr()->FrameIndex()]->GetColorTarget(0);
	std::shared_ptr<Image> pMotionNeighborMax = FrameBufferDiction::GetInstance()->GetFrameBuffers(FrameBufferDiction::FrameBufferType_MotionNeighborMax)[FrameWorkMgr()->FrameIndex()]->GetColorTarget(0);

	VkImageSubresourceRange subresourceRange = {};
	subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	subresourceRange.baseMipLevel = 0;
	subresourceRange.levelCount = pCombineResult->GetImageInfo().mipLevels;
	subresourceRange.layerCount = pCombineResult->GetImageInfo().arrayLayers;

	VkImageMemoryBarrier imgBarrier = {};
	imgBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	imgBarrier.image = pCombineResult->GetDeviceHandle();
	imgBarrier.subresourceRange = subresourceRange;
	imgBarrier.oldLayout = VK_IMAGE_LAYOUT_GENERAL;
	imgBarrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
	imgBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imgBarrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
	imgBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	imgBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

	barriers.push_back(imgBarrier);

	subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	subresourceRange.baseMipLevel = 0;
	subresourceRange.levelCount = pMotionNeighborMax->GetImageInfo().mipLevels;
	subresourceRange.layerCount = pMotionNeighborMax->GetImageInfo().arrayLayers;

	imgBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	imgBarrier.image = pMotionNeighborMax->GetDeviceHandle();
	imgBarrier.subresourceRange = subresourceRange;
	imgBarrier.oldLayout = VK_IMAGE_LAYOUT_GENERAL;
	imgBarrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
	imgBarrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
	imgBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

	barriers.push_back(imgBarrier);

	pCmdBuffer->AttachBarriers
	(
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
		VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
		{},
		{},
		barriers
	);
}