#pragma once
#include "Material.h"
#include "FrameBufferDiction.h"
#include "RenderPassDiction.h"

class Image;

class CustomizedComputeMaterial : public Material
{
public:
	typedef struct _TextureBarrier
	{
		bool								enableBarrier;
		VkPipelineStageFlagBits				srcPipelineStages;
		VkImageLayout						oldImageLayout;
		VkAccessFlagBits					srcAccessFlags;

		VkPipelineStageFlagBits				dstPipelineStages;
		VkImageLayout						newImageLayout;
		VkAccessFlagBits					dstAccessFlags;
	}TextureBarrier;

	typedef struct _TextureUnit
	{
		uint32_t							bindingIndex;

		std::vector<std::shared_ptr<Image>>	textures;
		VkImageAspectFlags					aspectMask;
		Vector4ui							textureSubresRange;	// Base miplevel, mipLevel count, base array layer, array layer count
		bool								isStorageImage;		// Storage image is for compute

		// Barrier info
		enum TextureSelector
		{
			BY_FRAME,	// A texture will be selected by frame index
			ALL,		// All textures will be selected
			NONE,		// No textures will be selected, no need for barriers
			COUNT
		}textureSelector;

		TextureBarrier						textureBarrier[Material::BarrierInsertionPoint::COUNT];

	}TextureUnit;

	typedef struct _Variables
	{
		std::wstring	shaderPath;
		Vector3ui		groupSize;

		std::vector<TextureUnit>	textureUnits;

		// Push constants
		std::vector<uint8_t>	pushConstantData;
	}Variables;

public:
	static std::shared_ptr<CustomizedComputeMaterial> CreateMaterial(const CustomizedComputeMaterial::Variables& variables);

public:
	void Draw(const std::shared_ptr<CommandBuffer>& pCmdBuf, const std::shared_ptr<FrameBuffer>& pFrameBuffer, uint32_t pingpong = 0, bool overrideVP = false) override {}

protected:
	bool Init(const std::shared_ptr<CustomizedComputeMaterial>& pSelf, const CustomizedComputeMaterial::Variables& variables);

	void CustomizeMaterialLayout(std::vector<UniformVarList>& materialLayout) override;
	void CustomizePoolSize(std::vector<uint32_t>& counts) override;
	void CustomizeCommandBuffer(const std::shared_ptr<CommandBuffer>& pCmdBuf, const std::shared_ptr<FrameBuffer>& pFrameBuffer, uint32_t pingpong = 0) override;

	void AttachResourceBarriers(const std::shared_ptr<CommandBuffer>& pCmdBuffer, BarrierInsertionPoint barrierInsertionPoint, uint32_t pingpong = 0) override;

	static void AssembleBarrier(const TextureUnit& textureUnit, uint32_t textureIndex, BarrierInsertionPoint barrierInsertPoint, VkImageMemoryBarrier& barrier, VkImageSubresourceRange& subresRange);

private:
	Variables	m_variables;
};