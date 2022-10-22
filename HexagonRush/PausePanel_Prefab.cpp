#include "pch.h"
#include "PausePanel_Prefab.h"

#include "SceneFader.h"
#include "ButtonBase.h"
#include "PausePanelController.h"

PausePanel_Prefab::PausePanel_Prefab(std::string currSceneName) : IPrefab(Muscle::CreateGameObject())
{
	// Prefab ��ü�� ���� ī��Ʈ�� ������� ������ ..!
	_currSceneName = currSceneName;

	// SceneFader (Restart, Title ���� ���Ͽ�)
	GetGameObject()->AddComponent<Muscle::UIRenderer>();

	_fader = GetGameObject()->AddComponent<SceneFader>();


	// Panel�� �� �� �̹��� �� ��
	std::shared_ptr<Muscle::GameObject> panelBack = Muscle::CreateGameObject();

	panelBack->AddComponent<Muscle::UIRenderer>();

	panelBack->GetComponent<Muscle::UIRenderer>()->SetSortOrder(3);

	panelBack->GetComponent<Muscle::UIRenderer>()->SetTextureID(ResourceManager::Get()->GetTextureID(TEXT("Pause_Panel")));
	 

	std::shared_ptr<Muscle::GameObject> cursor = Muscle::CreateGameObject();

	cursor->AddComponent<Muscle::UIRenderer>();

	cursor->GetComponent<Muscle::UIRenderer>()->SetSortOrder(2);

	cursor->GetComponent<Muscle::UIRenderer>()->SetTextureID(ResourceManager::Get()->GetTextureID(TEXT("Cursor")));



	// RESUME
	std::shared_ptr<Muscle::GameObject> resumeButton = Muscle::CreateGameObject();

	resumeButton->AddComponent<Muscle::UIRenderer>();

	resumeButton->GetComponent<Muscle::UIRenderer>()->SetSortOrder(2);

	resumeButton->GetComponent<Muscle::UIRenderer>()->SetTextureID(ResourceManager::Get()->GetTextureID(TEXT("Pause_Resume")));
	
	resumeButton->AddComponent<ButtonBase>();

	resumeButton->GetComponent<ButtonBase>()->RegistCallBack(nullptr);
	// RESUME




	// RESTART
	std::shared_ptr<Muscle::GameObject> restartButton = Muscle::CreateGameObject();

	restartButton->AddComponent<Muscle::UIRenderer>();

	restartButton->GetComponent<Muscle::UIRenderer>()->SetSortOrder(2);

	restartButton->GetComponent<Muscle::UIRenderer>()->SetTextureID(ResourceManager::Get()->GetTextureID(TEXT("Pause_Restart")));
	
	restartButton->AddComponent<ButtonBase>();

	// �ð��� �ǵ����� FadeOut �� �ٽ� ���� ���� �����մϴ�.
	auto restart = [&](void) {  Muscle::CTime::SetGameSpeed(1.f, 0.f); 
	_fader->FadeOut(5.f, [&](void) {

			Muscle::MuscleEngine::GetInstance()->GetSceneManager()->LoadScene(_currSceneName);
		});
	};

	restartButton->GetComponent<ButtonBase>()->RegistCallBack(restart);
	// RESTART




	// TITLE
	std::shared_ptr<Muscle::GameObject> titleButton = Muscle::CreateGameObject();

	titleButton->AddComponent<Muscle::UIRenderer>();

	titleButton->GetComponent<Muscle::UIRenderer>()->SetSortOrder(2);

	titleButton->GetComponent<Muscle::UIRenderer>()->SetTextureID(ResourceManager::Get()->GetTextureID(TEXT("Pause_Title")));

	titleButton->AddComponent<ButtonBase>();

	auto title = [&](void) { Muscle::CTime::SetGameSpeed(1.f, 0.f);
	_fader->FadeOut(5.f, [&](void) {
		Muscle::MuscleEngine::GetInstance()->GetSceneManager()->LoadScene("TitleScene");
		
		});
	};

	titleButton->GetComponent<ButtonBase>()->RegistCallBack(title);
	// TITLE





	// EXIT
	std::shared_ptr<Muscle::GameObject> exitButton = Muscle::CreateGameObject();

	exitButton->AddComponent<Muscle::UIRenderer>();

	exitButton->GetComponent<Muscle::UIRenderer>()->SetSortOrder(2);

	exitButton->GetComponent<Muscle::UIRenderer>()->SetTextureID(ResourceManager::Get()->GetTextureID(TEXT("Pause_Exit")));

	exitButton->AddComponent<ButtonBase>();

	auto exitt = [&](void) { _fader->FadeOut(5.f, [&](void) {
			// ���α׷� ���� !
			exit(0);
		});		
	};

	exitButton->GetComponent<ButtonBase>()->RegistCallBack(exitt);
	// EXIT





	// Panel Controller
	GetGameObject()->AddComponent<PausePanelController>();

	// => ���� PausePanelController���� ĳ�� �� ��ư���� ��Ʈ����
	cursor->SetTag("Cursor");
	GetGameObject()->SetChild(cursor);

	panelBack->SetTag("Panel");
	GetGameObject()->SetChild(panelBack);

	resumeButton->SetTag("Resume");
	GetGameObject()->SetChild(resumeButton);

	restartButton->SetTag("Restart");
	GetGameObject()->SetChild(restartButton);

	titleButton->SetTag("Title");
	GetGameObject()->SetChild(titleButton);

	exitButton->SetTag("Exit");
	GetGameObject()->SetChild(exitButton);
}

PausePanel_Prefab::~PausePanel_Prefab()
{
	_fader.reset();
}