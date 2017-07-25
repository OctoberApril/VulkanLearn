#pragma once

#include "DeviceObjectBase.h"
#include <map>
#include <functional>
#include <mutex>
#include "../thread/ThreadWorker.hpp"

class CommandBuffer;
class Fence;
class PerFrameResource;
class CommandBuffer;
class Semaphore;
class Queue;
class ThreadTaskQueue;

class FrameManager : public DeviceObjectBase<FrameManager>
{
	typedef struct _SubmissionInfo
	{
		std::shared_ptr<Queue>						pQueue;
		std::vector<std::shared_ptr<CommandBuffer>> cmdBuffers;
		std::vector<std::shared_ptr<Semaphore>>		waitSemaphores;
		std::vector<VkPipelineStageFlags>			waitStages;
		std::vector<std::shared_ptr<Semaphore>>		signalSemaphores;
		bool										waitUtilQueueIdle;
		bool										submitted;
	}SubmissionInfo;

	typedef struct _JobStatus
	{
		uint32_t numJobs = 0;
		std::function<void(uint32_t)> callback;
		bool submissionEnded = false;
		bool waitForPresent = false;

		void Reset()
		{
			numJobs = 0;
			submissionEnded = false;
			waitForPresent = false;
		}
	}JobStatus;

	typedef std::map<uint32_t, std::vector<std::shared_ptr<PerFrameResource>>> FrameResourceTable;
	typedef std::map<uint32_t, std::vector<SubmissionInfo>> SubmissionInfoTable;

public:
	std::shared_ptr<PerFrameResource> AllocatePerFrameResource(uint32_t frameIndex);
	uint32_t FrameIndex() const { return m_frameBin[m_currentFrameBinIndex]; }
	uint32_t FrameIndex(uint32_t binIndex) const { return m_frameBin[binIndex]; }

	void CacheSubmissioninfo(
		const std::shared_ptr<Queue>& pQueue,
		const std::vector<std::shared_ptr<CommandBuffer>>& cmdBuffer,
		const std::vector<VkPipelineStageFlags>& waitStages,
		bool waitUtilQueueIdle);

	void CacheSubmissioninfo(
		const std::shared_ptr<Queue>& pQueue,
		const std::vector<std::shared_ptr<CommandBuffer>>& cmdBuffer,
		const std::vector<std::shared_ptr<Semaphore>>& waitSemaphores,
		const std::vector<VkPipelineStageFlags>& waitStages,
		const std::vector<std::shared_ptr<Semaphore>>& signalSemaphores,
		bool waitUtilQueueIdle);

	// Thread related
	void AddJobToFrame(ThreadJobFunc jobFunc);
	void JobDone(uint32_t frameIndex);
	void IncBinIndex();
	void SetFrameIndex(uint32_t index);

protected:
	bool Init(const std::shared_ptr<Device>& pDevice, uint32_t maxFrameCount, const std::shared_ptr<FrameManager>& pSelf);
	static std::shared_ptr<FrameManager> Create(const std::shared_ptr<Device>& pDevice, uint32_t maxFrameCount);

	std::shared_ptr<Fence> GetCurrentFrameFence() const { return m_frameFences[m_currentFrameBinIndex]; }
	std::shared_ptr<Fence> GetFrameFence(uint32_t frameBinIndex) const { return m_frameFences[frameBinIndex]; }
	void WaitForFence();
	void WaitForFence(uint32_t binIndex);

	void FlushCachedSubmission(uint32_t frameBinIndex);
	void EndJobSubmission(std::function<void(uint32_t)>);

	void WaitForGPUWork(uint32_t binIndex);

	std::shared_ptr<Semaphore> GetAcqurieDoneSemaphore() const { return m_acquireDoneSemaphores[m_currentFrameBinIndex]; }
	std::shared_ptr<Semaphore> GetRenderDoneSemaphore() const { return m_renderDoneSemahpres[m_currentFrameBinIndex]; }
	std::shared_ptr<Semaphore> GetAcqurieDoneSemaphore(uint32_t frameBinIndex) const { return m_acquireDoneSemaphores[frameBinIndex]; }
	std::shared_ptr<Semaphore> GetRenderDoneSemaphore(uint32_t frameBinIndex) const { return m_renderDoneSemahpres[frameBinIndex]; }

	void CacheSubmissioninfoInternal(
		const std::shared_ptr<Queue>& pQueue,
		const std::vector<std::shared_ptr<CommandBuffer>>& cmdBuffer,
		const std::vector<std::shared_ptr<Semaphore>>& waitSemaphores,
		const std::vector<VkPipelineStageFlags>& waitStages,
		const std::vector<std::shared_ptr<Semaphore>>& signalSemaphores,
		bool waitUtilQueueIdle);

private:
	FrameResourceTable						m_frameResTable;
	std::vector<std::shared_ptr<Fence>>		m_frameFences;
	std::vector<std::shared_ptr<Semaphore>>	m_acquireDoneSemaphores;
	std::vector<std::shared_ptr<Semaphore>>	m_renderDoneSemahpres;

	std::vector<uint32_t>					m_frameBin;
	uint32_t								m_currentFrameBinIndex;

	SubmissionInfoTable						m_pendingSubmissionInfoTable;
	SubmissionInfoTable						m_submissionInfoTable;

	uint32_t m_maxFrameCount;

	std::vector<std::shared_ptr<ThreadTaskQueue>>	m_threadTaskQueues;
	std::vector<JobStatus>							m_jobStatus;
	uint32_t										m_currentPresentBin;
	std::mutex										m_mutex;

	friend class SwapChain;
	friend class Queue;
};