#pragma once
#include "../Base/BaseComponent.h"
#include "../Maths/Matrix.h"
#include "../class/InputHub.h"

enum CharMoveDir
{
	Forward = 1,
	Backward = 2,
	Leftward = 4,
	Rightward = 8
};

typedef struct _CharacterVariable
{
	double moveSpeed;
	double rotateSpeed;
}CharacterVariable;

class Character : public BaseComponent, public IInputListener
{
	DECLARE_CLASS_RTTI(Character);

public:
	static std::shared_ptr<Character> Create(const CharacterVariable& charVar);

	// For input listener
	void ProcessKey(KeyState keyState, uint8_t keyCode) override;
	void ProcessMouse(KeyState keyState, MouseButton mouseButton, const Vector2d& mousePosition) override;
	void ProcessMouse(const Vector2d& mousePosition) override;

	void Move(uint32_t dir, double delta);

	void OnRotateStart(const Vector2d& v);

	void OnRotate(const Vector2d& v, bool started);
	void OnRotate(uint32_t dir, double delta);

	void OnRotateEnd(const Vector2d& v);

	//input para v needs to be the ratio compared to camera size
	//this function should be used by those "OnRotate*" functions
	//but if user of this class decided to handle rotate logic himself, call this function directly
	void Rotate(const Vector2d& v);

	void SetCharacterVariable(const CharacterVariable& var) { m_charVars = var; }
	CharacterVariable GetCharacterVariable() const { return m_charVars; }

	void Update() override;

protected:
	void Move(const Vector3d& v, double delta);

protected:
	// Character movement variables
	Matrix3d			m_rotationStartMatrix;
	CharacterVariable	m_charVars;
	bool				m_rotationStarted;
	Vector2d			m_rotationStartPos;
	double				m_startTargetToH;
	Vector3d			m_lastTarget;

	// Flags for normal move & rotation
	uint32_t			m_moveFlag = 0;
	uint32_t			m_rotateFlag = 0;

	bool				m_isControlInRotation = false;

	Vector2d			m_lastSampleCursorPosition;
};