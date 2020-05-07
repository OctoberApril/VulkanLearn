#include "UniformData.h"
#include "Material.h"
#include "../vulkan/GlobalDeviceObjects.h"
#include "FrameWorkManager.h"
#include "../vulkan/Buffer.h"
#include "../vulkan/DescriptorSetLayout.h"
#include "../vulkan/DescriptorSet.h"
#include "../vulkan/DescriptorPool.h"
#include "../vulkan/SwapChain.h"
#include "GlobalTextures.h"
#include "GBufferInputUniforms.h"
#include "FrameEventManager.h"

bool UniformData::Init()
{
	if (!Singleton<UniformData>::Init())
		return false;

	m_uniformStorageBuffers.resize(UniformStorageType::PerObjectMaterialVariableBuffer);

	for (uint32_t i = 0; i < UniformStorageType::PerObjectMaterialVariableBuffer; i++)
	{
		switch ((UniformStorageType)i)
		{
		case UniformStorageType::GlobalVariableBuffer:		m_uniformStorageBuffers[i] = GlobalUniforms::Create(); break;
		case UniformStorageType::PerBoneBuffer:				m_uniformStorageBuffers[i] = PerBoneUniforms::Create(); break;
		case UniformStorageType::PerBoneIndirectBuffer:		m_uniformStorageBuffers[i] = BoneIndirectUniform::Create(PerBoneBuffer); break;
		case UniformStorageType::PerFrameBoneIndirectBuffer:m_uniformStorageBuffers[i] = BoneIndirectUniform::Create(PerFrameBoneBuffer); break;
		case UniformStorageType::PerMeshUniformBuffer:		m_uniformStorageBuffers[i] = PerMeshUniforms::Create(); break;
		case UniformStorageType::PerPlanetBuffer:			m_uniformStorageBuffers[i] = PerPlanetUniforms::Create(); break;
		case UniformStorageType::PerAnimationUniformBuffer:	m_uniformStorageBuffers[i] = PerAnimationUniforms::Create(); break;
		case UniformStorageType::PerFrameBoneBuffer:		m_uniformStorageBuffers[i] = PerBoneUniforms::Create(); break;
		case UniformStorageType::PerFrameVariableBuffer:	m_uniformStorageBuffers[i] = PerFrameUniforms::Create(); break;
		case UniformStorageType::PerObjectVariableBuffer:	m_uniformStorageBuffers[i] = PerObjectUniforms::Create(); break;
		default:
			break;
		}
	}

	m_uniformTextures.resize(UniformTextureTypeCount);
	for (uint32_t i = 0; i < UniformTextureType::UniformTextureTypeCount; i++)
	{
		switch ((UniformTextureType)i)
		{
		case UniformTextureType::GlobalUniformTextures: m_uniformTextures[i] = GlobalTextures::Create(); break;
		case UniformTextureType::GlobalGBufferInputUniforms: m_uniformTextures[i] = GBufferInputUniforms::Create(); break;
		default:
			break;
		}
	}

	BuildDescriptorSets();

	for (uint32_t frameIndex = 0; frameIndex < GetSwapChain()->GetSwapChainImageCount(); frameIndex++)
	{
		std::vector<uint32_t> offsets;
		for (uint32_t i = 0; i < UniformStorageType::PerObjectMaterialVariableBuffer; i++)
			offsets.push_back(m_uniformStorageBuffers[i]->GetFrameOffset() * frameIndex);

		m_cachedFrameOffsets.push_back(offsets);
	}

	FrameEventManager::GetInstance()->Register(m_pInstance);

	return true;
}

void UniformData::OnPostSceneTraversal()
{
	SyncDataBuffer();
}

void UniformData::SyncDataBuffer()
{
	for (auto& var : m_uniformStorageBuffers)
		var->SyncBufferData();
}

std::vector<std::vector<UniformVarList>> UniformData::GenerateUniformVarLayout() const
{
	std::vector<std::vector<UniformVarList>> layout;
	for (uint32_t i = 0; i < UniformStorageType::PerObjectMaterialVariableBuffer; i++)
		layout.push_back(m_uniformStorageBuffers[i]->PrepareUniformVarList());
	return layout;
}

std::vector<std::vector<uint32_t>> UniformData::GetCachedFrameOffsets() const
{
	return m_cachedFrameOffsets;
}

void UniformData::BuildDescriptorSets()
{
	// Setup global uniform var list, including one global var buffer, one global texture list, one global input attachment list
	std::vector<UniformVarList> globalUniformVars = m_uniformStorageBuffers[UniformStorageType::GlobalVariableBuffer]->PrepareUniformVarList();

	std::vector<UniformVarList> perBoneVars = m_uniformStorageBuffers[UniformStorageType::PerBoneBuffer]->PrepareUniformVarList();
	globalUniformVars.insert(globalUniformVars.end(), perBoneVars.begin(), perBoneVars.end());

	std::vector<UniformVarList> perBoneIndirectVars = m_uniformStorageBuffers[UniformStorageType::PerBoneIndirectBuffer]->PrepareUniformVarList();
	globalUniformVars.insert(globalUniformVars.end(), perBoneIndirectVars.begin(), perBoneIndirectVars.end());

	std::vector<UniformVarList> perFrameBoneIndirectVars = m_uniformStorageBuffers[UniformStorageType::PerFrameBoneIndirectBuffer]->PrepareUniformVarList();
	globalUniformVars.insert(globalUniformVars.end(), perFrameBoneIndirectVars.begin(), perFrameBoneIndirectVars.end());

	std::vector<UniformVarList> perMeshVars = m_uniformStorageBuffers[UniformStorageType::PerMeshUniformBuffer]->PrepareUniformVarList();
	globalUniformVars.insert(globalUniformVars.end(), perMeshVars.begin(), perMeshVars.end());

	std::vector<UniformVarList> perPlanetVars = m_uniformStorageBuffers[UniformStorageType::PerPlanetBuffer]->PrepareUniformVarList();
	globalUniformVars.insert(globalUniformVars.end(), perPlanetVars.begin(), perPlanetVars.end());

	std::vector<UniformVarList> perAnimationVars = m_uniformStorageBuffers[UniformStorageType::PerAnimationUniformBuffer]->PrepareUniformVarList();
	globalUniformVars.insert(globalUniformVars.end(), perAnimationVars.begin(), perAnimationVars.end());

	std::vector<UniformVarList> globalTextureVars = m_uniformTextures[UniformTextureType::GlobalUniformTextures]->PrepareUniformVarList();
	globalUniformVars.insert(globalUniformVars.end(), globalTextureVars.begin(), globalTextureVars.end());

	/*
	std::vector<UniformVarList> globalInputAttachVars = m_uniformTextures[UniformTextureType::GlobalGBufferInputUniforms]->PrepareUniformVarList();
	globalUniformVars.insert(globalUniformVars.end(), globalInputAttachVars.begin(), globalInputAttachVars.end());*/

	// Setup per frame uniform var list
	std::vector<UniformVarList> perFrameUniformVars = m_uniformStorageBuffers[UniformStorageType::PerFrameVariableBuffer]->PrepareUniformVarList();

	// Setup per frame bone var list
	std::vector<UniformVarList> perFrameBoneVars = m_uniformStorageBuffers[UniformStorageType::PerFrameBoneBuffer]->PrepareUniformVarList();
	perFrameUniformVars.insert(perFrameUniformVars.end(), perFrameBoneVars.begin(), perFrameBoneVars.end());

	// Setup per object uniform var list
	std::vector<UniformVarList> perObjectUniformVars = m_uniformStorageBuffers[UniformStorageType::PerObjectVariableBuffer]->PrepareUniformVarList();


	// Put them all together
	std::vector<std::vector<UniformVarList>> uniformVarLists(PerObjectMaterialVariableBufferLocation);
	uniformVarLists[GlobalUniformsLocation]		= globalUniformVars;
	uniformVarLists[PerFrameUniformsLocation]	= perFrameUniformVars;
	uniformVarLists[PerObjectUniformsLocation]	= perObjectUniformVars;

	// Build vulkan layout bindings
	std::vector<std::vector<VkDescriptorSetLayoutBinding>> layoutBindings;
	for (auto & varList : uniformVarLists)
	{
		std::vector<VkDescriptorSetLayoutBinding> bindings;
		for (auto & var : varList)
		{
			switch (var.type)
			{
			case DynamicUniformBuffer:
				bindings.push_back
				({
					(uint32_t)bindings.size(),
					VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
					var.count,
					VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_COMPUTE_BIT,
					nullptr
					});

				break;
			case DynamicShaderStorageBuffer:
				bindings.push_back
				({
					(uint32_t)bindings.size(),
					VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC,
					var.count,
					VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_COMPUTE_BIT,
					nullptr
					});

				break;
			case CombinedSampler:
				bindings.push_back
				({
					(uint32_t)bindings.size(),
					VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
					var.count,
					VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_COMPUTE_BIT,
					nullptr
					});
				break;
			case InputAttachment:
				bindings.push_back
				({
					(uint32_t)bindings.size(),
					VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,
					var.count,
					VK_SHADER_STAGE_FRAGMENT_BIT,
					nullptr
					});
				break;
			default:
				ASSERTION(false);
				break;
			}
		}
		m_descriptorSetLayouts.push_back(DescriptorSetLayout::Create(GetDevice(), bindings));
		layoutBindings.push_back(bindings);
	}

	// Prepare descriptor pool size according to resources used by this material
	std::vector<uint32_t> counts(VK_DESCRIPTOR_TYPE_RANGE_SIZE);
	for (auto & bindings : layoutBindings)
	{
		for (auto & binding : bindings)
		{
			counts[binding.descriptorType] += binding.descriptorCount;
		}
	}

	std::vector<VkDescriptorPoolSize> descPoolSize;
	for (uint32_t i = 0; i < counts.size(); i++)
	{
		if (counts[i] != 0)
			descPoolSize.push_back({ (VkDescriptorType)i, counts[i] });
	}

	VkDescriptorPoolCreateInfo descPoolInfo = {};
	descPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descPoolInfo.pPoolSizes = descPoolSize.data();
	descPoolInfo.poolSizeCount = (uint32_t)descPoolSize.size();
	descPoolInfo.maxSets = PerObjectMaterialVariableBufferLocation;

	m_pDescriptorPool = DescriptorPool::Create(GetDevice(), descPoolInfo);

	// Allocate descriptor sets according to layouts
	for (auto & layout : m_descriptorSetLayouts)
		m_descriptorSets.push_back(m_pDescriptorPool->AllocateDescriptorSet(layout));



	// Setup descriptor sets data

	// 1. Global descriptor set
	uint32_t bindingSlot = 0;
	bindingSlot = m_uniformStorageBuffers[GlobalVariableBuffer]->SetupDescriptorSet(m_descriptorSets[GlobalUniformsLocation], bindingSlot);
	bindingSlot = m_uniformStorageBuffers[PerBoneBuffer]->SetupDescriptorSet(m_descriptorSets[GlobalUniformsLocation], bindingSlot);
	bindingSlot = m_uniformStorageBuffers[PerBoneIndirectBuffer]->SetupDescriptorSet(m_descriptorSets[GlobalUniformsLocation], bindingSlot);
	bindingSlot = m_uniformStorageBuffers[PerFrameBoneIndirectBuffer]->SetupDescriptorSet(m_descriptorSets[GlobalUniformsLocation], bindingSlot);
	bindingSlot = m_uniformStorageBuffers[PerMeshUniformBuffer]->SetupDescriptorSet(m_descriptorSets[GlobalUniformsLocation], bindingSlot);
	bindingSlot = m_uniformStorageBuffers[PerPlanetBuffer]->SetupDescriptorSet(m_descriptorSets[GlobalUniformsLocation], bindingSlot);
	bindingSlot = m_uniformStorageBuffers[PerAnimationUniformBuffer]->SetupDescriptorSet(m_descriptorSets[GlobalUniformsLocation], bindingSlot);
	bindingSlot = m_uniformTextures[GlobalUniformTextures]->SetupDescriptorSet(m_descriptorSets[GlobalUniformsLocation], bindingSlot);

	// 2. Per frame descriptor set
	bindingSlot = 0;
	bindingSlot = m_uniformStorageBuffers[PerFrameVariableBuffer]->SetupDescriptorSet(m_descriptorSets[PerFrameUniformsLocation], bindingSlot);
	bindingSlot = m_uniformStorageBuffers[PerFrameBoneBuffer]->SetupDescriptorSet(m_descriptorSets[PerFrameUniformsLocation], bindingSlot);

	// 3. Per object descriptor set
	m_uniformStorageBuffers[PerObjectVariableBuffer]->SetupDescriptorSet(m_descriptorSets[PerObjectUniformsLocation], 0);

}