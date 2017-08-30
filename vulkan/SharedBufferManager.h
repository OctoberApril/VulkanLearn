#pragma once

#include "Buffer.h"
#include "GlobalDeviceObjects.h"

class SharedBufferManager;

class BufferKey
{
public:
	static std::shared_ptr<BufferKey> Create(const std::shared_ptr<SharedBufferManager>& pSharedBufMgr, uint32_t index);
	~BufferKey();
private:
	bool Init(const std::shared_ptr<SharedBufferManager>& pSharedBufMgr, uint32_t index);

private:
	uint32_t								m_index;
	std::shared_ptr<SharedBufferManager>	m_pSharedBufMgr;

	friend class SharedBufferManager;
};

class SharedBufferManager : public DeviceObjectBase<SharedBufferManager>
{
protected:
	bool Init(const std::shared_ptr<Device>& pDevice, 
		const std::shared_ptr<SharedBufferManager>& pSelf,
		VkBufferUsageFlags usage,
		uint32_t numBytes);

	void FreeBuffer(uint32_t index);

public:
	static std::shared_ptr<SharedBufferManager> Create(const std::shared_ptr<Device>& pDevice,
		VkBufferUsageFlags usage,
		uint32_t numBytes);

public:
	std::shared_ptr<Buffer> GetBuffer() const { return m_pBuffer; }
	std::shared_ptr<BufferKey> AllocateBuffer(uint32_t numBytes);

protected:
	std::shared_ptr<Buffer>					m_pBuffer;
	std::vector<VkDescriptorBufferInfo>		m_bufferTable;

	friend class BufferKey;
};