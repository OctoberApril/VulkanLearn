#pragma once
#include "../Base/BaseComponent.h"

class Mesh;
class Material;
class MaterialInstance;
class DescriptorSet;
class DescriptorPool;

class MeshRenderer : public BaseComponent
{
public:
	static std::shared_ptr<MeshRenderer> Create(const std::shared_ptr<Mesh> pMesh, const std::shared_ptr<MaterialInstance>& pMaterialInstance);
	static std::shared_ptr<MeshRenderer> Create(const std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<MaterialInstance>>& materialInstances);
	static std::shared_ptr<MeshRenderer> Create();

	~MeshRenderer();

public:
	void Update() override;
	void LateUpdate() override;
	void Draw(const std::shared_ptr<PerFrameResource>& pPerFrameRes) override;

	std::shared_ptr<Mesh> GetMesh() const { return m_pMesh; }

protected:
	bool Init(const std::shared_ptr<MeshRenderer>& pSelf, const std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<MaterialInstance>>& materialInstances);

protected:
	std::shared_ptr<Mesh>							m_pMesh;
	uint32_t										m_perObjectBufferIndex;

	// First: material instance
	// Second: Key reference of material instance to this mesh renderer, used in destructor to remove weak reference in material instance
	std::vector<std::pair<std::shared_ptr<MaterialInstance>, uint32_t>> m_materialInstances;
};