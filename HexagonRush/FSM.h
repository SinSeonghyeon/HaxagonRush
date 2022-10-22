#pragma once

/// <summary>
/// �� ������ ��� �������� ������? ��� �������� ���ø����� ����!
/// ������ �÷��̾ ������..
/// 2022.08.09 �ż���
/// </summary>
template<class T>
class FSM : public std::enable_shared_from_this<T>
{
public:
	FSM() = default;

	~FSM() 
	{
		int a = 0;
	};

public:
	void Finalize()
	{
		_currentStatus.reset();
	}

protected:
	std::shared_ptr<IState<T>> _currentStatus;

	void SetState(std::shared_ptr<IState<T>> state)
	{
		// FSM�� ������ ��, state�� Prev�� Curr�� ������ return�Ѵ�. ����, ���� ���Դٴ� ����
		// State�� Transition�� �Ͼ�ٴ� �� ..!! => ���� OnExit �߻� !
		if (_currentStatus)
		{
			_currentStatus->OnExit();

			_currentStatus.reset();
		}

		_currentStatus = state;

		_currentStatus->OnEnter(this->shared_from_this());
	};
};