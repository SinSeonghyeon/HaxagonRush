#pragma once

class SceneFader;

/// <summary>
/// ���� �� Ư�� Ű�� ������ �� ȭ�鿡 ������ Object.
/// 
/// �� ������ �ش� Ű ���� �� PausePanel Enable ���ָ� �� �� ?
/// 
/// �ƴϸ� �׳� ������������ ���� .. ���� Ÿ���� ���߾�� �Ѵ�.
/// </summary>
class PausePanel_Prefab : public IPrefab
{
public:
	PausePanel_Prefab(std::string currSceneName);

	virtual ~PausePanel_Prefab();

private:
	std::string _currSceneName;

	std::shared_ptr<SceneFader> _fader;
};