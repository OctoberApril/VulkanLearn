#pragma once
#include "../Base/BaseComponent.h"
#include "../Maths/Matrix.h"

class DirectionLight : public BaseComponent
{
	DECLARE_CLASS_RTTI(DirectionLight);

public:
	static const double DEFAULT_SHADOWMAP_SIZE;
	static const double DEFAULT_FRUSTUM_SIZE;
	static const double DEFAULT_FRUSTUM_LENGTH;

protected:
	bool Init(const std::shared_ptr<DirectionLight>& pLight, const Vector3d& lightColor, const Vector3d& frustumSize, const Vector2ui& shadowMapSize);

public:
	static std::shared_ptr<DirectionLight> Create(const Vector3d& lightColor, const Vector3d& frustumSize = { DEFAULT_FRUSTUM_SIZE, DEFAULT_FRUSTUM_SIZE, DEFAULT_FRUSTUM_LENGTH }, const Vector2ui& shadowMapSize = { (uint32_t)DEFAULT_SHADOWMAP_SIZE, (uint32_t)DEFAULT_SHADOWMAP_SIZE });

public:
	void SetLightColor(const Vector3d& lightColor);

	void Update() override;
	void OnPreRender() override;

protected:
	void UpdateData();

protected:
	Vector3d	m_lightColor;
	Vector3d	m_frustumSize;
	Vector2ui	m_shadowMapSize;
	Matrix4d	m_cs2lsProjMatrix;
	Vector3d	m_wsLightDirection;
	Vector3d	m_csLightDirection;
};