#pragma once

#include "../Maths/Matrix.h"
#include "ChunkBasedUniforms.h"

class DescriptorSet;

typedef struct _PerMaterialIndirectVariables
{
	uint32_t perObjectIndex = 0;
	uint32_t perMaterialIndex = 0;
}PerMaterialIndirectVariables;


class PerMaterialIndirectUniforms : public ChunkBasedUniforms
{
public:
	bool Init(const std::shared_ptr<PerMaterialIndirectUniforms>& pSelf);
	static std::shared_ptr<PerMaterialIndirectUniforms> Create();

public:
	void SetPerObjectIndex(uint32_t indirectIndex, uint32_t perObjectIndex) { m_perMaterialIndirectIndex[indirectIndex].perObjectIndex = perObjectIndex; SetChunkDirty(indirectIndex); }
	uint32_t GetPerObjectIndex(uint32_t indirectIndex) const { return m_perMaterialIndirectIndex[indirectIndex].perObjectIndex; }
	void SetPerMaterialIndex(uint32_t indirectIndex, uint32_t perMaterialIndex) { m_perMaterialIndirectIndex[indirectIndex].perMaterialIndex = perMaterialIndex; SetChunkDirty(indirectIndex); }
	uint32_t GetPerMaterialIndex(uint32_t indirectIndex) const { return m_perMaterialIndirectIndex[indirectIndex].perMaterialIndex; }

	std::vector<UniformVarList> PrepareUniformVarList() const override;
	uint32_t SetupDescriptorSet(const std::shared_ptr<DescriptorSet>& pDescriptorSet, uint32_t bindingIndex) const override;

protected:
	void UpdateDirtyChunkInternal(uint32_t index) override;
	const void* AcquireDataPtr() const override { return &m_perMaterialIndirectIndex[0]; }
	uint32_t AcquireDataSize() const override { return sizeof(m_perMaterialIndirectIndex); }

protected:
	PerMaterialIndirectVariables	m_perMaterialIndirectIndex[MAXIMUM_OBJECTS];
};