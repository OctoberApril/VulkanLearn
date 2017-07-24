#pragma once
#include "Base.h"

class BaseObject;

class BaseComponent : public SelfRefBase<BaseComponent>
{
public:
	virtual ~BaseComponent(void) = 0 {}

	virtual void Update(float delta) {}

	std::shared_ptr<BaseObject> GetObject() const { return m_pObject; }

protected:
	virtual bool Init(const std::shared_ptr<BaseComponent>& pSelf)
	{
		if (!SelfRefBase<BaseComponent>::Init(pSelf))
			return false;

		return true;
	}

	void SetObject(const std::shared_ptr<BaseObject>& pObj) { m_pObject = pObj; }

protected:
	bool m_isDirty = false;

	std::shared_ptr<BaseObject> m_pObject;

	friend class BaseObject;
};

