#version 460

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (location = 0) in vec2 inUv;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec3 inTangent;
layout (location = 3) in vec3 inBitangent;
layout (location = 4) flat in int perMaterialIndex;

layout (location = 0) out vec4 outGBuffer0;
layout (location = 1) out vec4 outGBuffer1;
layout (location = 2) out vec4 outGBuffer2;

#include "uniform_layout.h"

struct PBRTextures
{
	vec4 albedoRougness;
	vec2 AOMetalic;

	float albedoRoughnessIndex;
	float normalAOIndex;
	float metallicIndex;
};

layout(set = 3, binding = 1) buffer MaterialUniforms
{
	PBRTextures textures[];
};

void main() 
{
	float metalic = 1.0f;
	if (textures[perMaterialIndex].metallicIndex < 0)
		metalic *= textures[perMaterialIndex].AOMetalic.g;
	else
		metalic = texture(R8_1024_MIP_2DARRAY, vec3(inUv.st, textures[perMaterialIndex].metallicIndex), 0.0).r;

	vec4 normal_ao = vec4(vec3(0), 1);
	if (textures[perMaterialIndex].normalAOIndex < 0)
	{
		normal_ao.xyz = normalize(inNormal);
		normal_ao.w = textures[perMaterialIndex].AOMetalic.x;
	}
	else
	{
		normal_ao = texture(RGBA8_1024_MIP_2DARRAY, vec3(inUv.st, textures[perMaterialIndex].normalAOIndex), 0.0);

		vec3 pertNormal = normal_ao.xyz * 2.0 - 1.0;
		mat3 TBN = mat3(normalize(inTangent), normalize(inBitangent), normalize(inNormal));
		pertNormal = TBN * pertNormal;

		normal_ao.xyz = pertNormal.xyz;
	}

	vec4 albedo_roughness = textures[perMaterialIndex].albedoRougness;
	if (textures[perMaterialIndex].albedoRoughnessIndex >= 0)
		albedo_roughness = texture(RGBA8_1024_MIP_2DARRAY, vec3(inUv.st, textures[perMaterialIndex].albedoRoughnessIndex), 0.0);

	outGBuffer0.xyz = normal_ao.xyz * 0.5f + 0.5f;

	outGBuffer1 = vec4(albedo_roughness.rgb, 0);
	outGBuffer2 = vec4(vec3(albedo_roughness.w, metalic, 0), normal_ao.a);
}