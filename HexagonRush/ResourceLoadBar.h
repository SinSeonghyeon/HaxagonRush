#pragma once
class ResourceLoadBar : public Muscle::IComponents
{
public:
	ResourceLoadBar(std::shared_ptr<Muscle::GameObject> gameObject);

	~ResourceLoadBar() = default;
private:
	std::shared_ptr<Muscle::UIRenderer> _uiRenderer; // ĳ��
public:
	void Start() override;

	void LateUpdate() override;
};