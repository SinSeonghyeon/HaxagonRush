#pragma once
class DrillSpiner : public  Muscle::IComponents
{
public:
	DrillSpiner(std::shared_ptr<Muscle::GameObject> GameObject);

	virtual ~DrillSpiner();

private:
	std::shared_ptr<Muscle::Transform> _rendererTransform; // �ڽ��� �������� ������ �ִ�. ĳ���ؼ� ����.

public:

	virtual void Start() override;

	virtual void Update() override;

	virtual void Finalize() override;
};

