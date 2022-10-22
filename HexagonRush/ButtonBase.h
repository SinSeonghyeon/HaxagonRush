#pragma once

// ���õǸ�, ������ �̺�Ʈ�� �����Ѵ�.
class ButtonBase : public Muscle::IComponents
{
public:
	enum class BUTTON_STATE
	{
		NONE,

		SELECTED,

		ON_CLICKED,
	};

public:
	ButtonBase(std::shared_ptr<Muscle::GameObject> gameObj);

	virtual ~ButtonBase();

private:
	std::shared_ptr<Muscle::UIRenderer> _uiRenderer;

	BUTTON_STATE _currButtonState;

	std::function<void(void)> _callBack;

public:
	void RegistCallBack(std::function<void(void)> callBack);

	inline BUTTON_STATE GetButtonState() { return _currButtonState; }

	inline void SetButtonState(BUTTON_STATE state) { _currButtonState = state; }

	virtual void Start() override;

	virtual void Update() override;
};

// On Button ���ñ� ���� �Ǵµ� .. ������ ���� �����θ� ������ �� ������ �� �������.