#pragma once

#include "DeviceObjectBase.h"
#include <map>

class DescriptorPool;
class DescriptorSetLayout;
class UniformBuffer;
class ShaderStorageBuffer;
class Image;
class Sampler;
class ImageView;
class VKGPUSyncRes;

typedef struct _CombinedImage
{
	std::shared_ptr<Image> pImage;
	std::shared_ptr<Sampler> pSampler;
	std::shared_ptr<ImageView> pImageView;
}CombinedImage;

class DescriptorSet : public DeviceObjectBase<DescriptorSet>
{
public:
	~DescriptorSet();

	bool Init(const std::shared_ptr<Device>& pDevice,
		const std::shared_ptr<DescriptorSet>& pSelf,
		const std::shared_ptr<DescriptorPool>& pDescriptorPool,
		const std::shared_ptr<DescriptorSetLayout>& pDescriptorSetLayout);

public:
	const std::shared_ptr<DescriptorPool> GetDescriptorPool() const { return m_pDescriptorPool; }
	const std::shared_ptr<DescriptorSetLayout> GetDescriptorSetLayout() const { return m_pDescriptorSetLayout; }
	VkDescriptorSet GetDeviceHandle() const { return m_descriptorSet; }

	void UpdateUniformBufferDynamic(uint32_t binding, const std::shared_ptr<UniformBuffer>& pBuffer);
	void UpdateUniformBuffer(uint32_t binding, const std::shared_ptr<UniformBuffer>& pBuffer);
	void UpdateShaderStorageBufferDynamic(uint32_t binding, const std::shared_ptr<ShaderStorageBuffer>& pBuffer);
	void UpdateShaderStorageBuffer(uint32_t binding, const std::shared_ptr<ShaderStorageBuffer>& pBuffer);
	void UpdateImage(uint32_t binding, const std::shared_ptr<Image>& pImage, const std::shared_ptr<Sampler> pSampler, const std::shared_ptr<ImageView> pImageView, bool isStorageImage = false);
	void UpdateImage(uint32_t binding, const CombinedImage& image, bool isStorageImage = false);
	void UpdateImages(uint32_t binding, const std::vector<CombinedImage>& images, bool isStorageImage = false);
	void UpdateInputImage(uint32_t binding, const std::shared_ptr<Image>& pImage, const std::shared_ptr<Sampler> pSampler, const std::shared_ptr<ImageView> pImageView);

	// FIXME: Refactor this when I create texture buffer object class
	void UpdateTexBuffer(uint32_t binding, const VkBufferView& texBufferView);

public:
	static std::shared_ptr<DescriptorSet> Create(const std::shared_ptr<Device>& pDevice,
		const std::shared_ptr<DescriptorPool>& pDescriptorPool,
		const std::shared_ptr<DescriptorSetLayout>& pDescriptorSetLayout);

protected:
	VkDescriptorSet									m_descriptorSet;
	std::shared_ptr<DescriptorPool>					m_pDescriptorPool;
	std::shared_ptr<DescriptorSetLayout>			m_pDescriptorSetLayout;
	std::vector<std::shared_ptr<VKGPUSyncRes>>		m_resourceTable;
};