#pragma once
#include "../common/Singleton.h"
#include "vulkan.h"
#include "Instance.h"
#include "PhysicalDevice.h"
#include "Device.h"
#include "SwapChain.h"
#include <vector>
#include <memory>
#include "DeviceMemoryManager.h"
#include "Buffer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "UniformBuffer.h"
#include "GlobalDeviceObjects.h"
#include "DepthStencilBuffer.h"
#include "SwapChainImage.h"
#include "RenderPass.h"
#include "DescriptorSetLayout.h"
#include "PipelineLayout.h"
#include "DescriptorPool.h"
#include "DescriptorSet.h"
#include "ShaderModule.h"
#include "GraphicPipeline.h"

typedef struct _swapchainImg
{
	std::vector<VkImage>				images;
	std::vector<VkImageView>			views;
}SwapchainImg;

typedef struct _GlobalUniforms
{
	float	model[16];
	float	view[16];
	float	projection[16];
	float	vulkanNDC[16];
	float	mvp[16];
	float   camPos[3];
	float	roughness;
}GlobalUniforms;


typedef struct _buffer
{
	_buffer() { info = {}; info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO; }
	Buffer								buffer;
	VkBufferCreateInfo					info;
	VkVertexInputBindingDescription		bindingDesc;
	uint32_t							count;
	std::vector<VkVertexInputAttributeDescription>	attribDesc;
}Buffer1;

class VulkanGlobal : public Singleton<VulkanGlobal>
{
public:
#if defined(_WIN32)
	void Init(HINSTANCE hInstance, WNDPROC wndproc);
#endif
	void InitVulkanInstance();
	void InitPhysicalDevice(HINSTANCE hInstance, HWND hWnd);
	void InitVulkanDevice();
	void InitQueue();
#if defined(_WIN32)
	void SetupWindow(HINSTANCE hInstance, WNDPROC wndproc);
	void HandleMsg(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
#endif
	void InitSurface();
	void InitSwapchain();

	void InitCommandPool();
	void InitSetupCommandBuffer();
	void InitMemoryMgr();
	void InitSwapchainImgs();
	void InitDepthStencil();
	void InitRenderpass();
	void InitFrameBuffer();
	void InitVertices();
	void InitUniforms();
	void InitDescriptorSetLayout();
	void InitPipelineCache();
	void InitPipeline();
	void InitDescriptorPool();
	void InitDescriptorSet();
	void InitDrawCmdBuffers();
	void InitSemaphore();
	void EndSetup();

	void Draw();
	void Update();

	void InitShaderModule(const char* vertShaderPath, const char* fragShaderPath);

public:
	static const uint32_t				WINDOW_WIDTH = 1024;
	static const uint32_t				WINDOW_HEIGHT = 768;

protected:
	std::shared_ptr<Instance>			m_pVulkanInstance;
	std::shared_ptr<PhysicalDevice>		m_pPhysicalDevice;
	std::shared_ptr<Device>				m_pDevice;

	//VkQueue								m_queue;

	VkCommandPool						m_commandPool;
	VkCommandBuffer						m_setupCommandBuffer;

	std::shared_ptr<DepthStencilBuffer>	m_pDSBuffer;

	std::shared_ptr<RenderPass>			m_pRenderPass;
	std::vector<VkFramebuffer>			m_framebuffers;

	std::shared_ptr<VertexBuffer>		m_pVertexBuffer;
	std::shared_ptr<IndexBuffer>		m_pIndexBuffer;
	std::shared_ptr<UniformBuffer>		m_pUniformBuffer;

	GlobalUniforms						m_globalUniforms;

	std::shared_ptr<DescriptorSetLayout>m_pDescriptorSetLayout;
	std::shared_ptr<PipelineLayout>		m_pPipelineLayout;

	std::shared_ptr<GraphicPipeline>	m_pPipeline;
	VkPipelineCache						m_pipelineCache;

	std::shared_ptr<DescriptorPool>		m_pDescriptorPool;
	std::shared_ptr<DescriptorSet>		m_pDescriptorSet;

	std::shared_ptr<ShaderModule>		m_pVertShader;
	std::shared_ptr<ShaderModule>		m_pFragShader;

	std::vector<VkCommandBuffer>		m_drawCmdBuffers;
	VkCommandBuffer						m_prePresentCmdBuffer;
	VkCommandBuffer						m_postPresentCmdBuffer;

	VkSemaphore							m_swapchainAcquireDone;
	VkSemaphore							m_renderDone;

	uint32_t							m_currentBufferIndex = 0;

	float								m_roughness = 0.1;
#if defined(_WIN32)
	HINSTANCE							m_hPlatformInst;
	HWND								m_hWindow;
#endif
};