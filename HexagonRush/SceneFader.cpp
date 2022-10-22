#include "pch.h"
#include "SceneFader.h"

SceneFader::SceneFader(std::shared_ptr<Muscle::GameObject> gameObj) :
	IComponents(gameObj), _fadeTime(0.f), _accumTime(0.f), _currFadeState(FADE_STATE::NONE)
{

}

SceneFader::~SceneFader()
{
	_uiRenderer.reset();
}

void SceneFader::FadeOut(float fadeOutTime, std::function<void(void)> callBack)
{
	_currFadeState = FADE_STATE::FADE_OUT;

	_fadeTime = fadeOutTime;

	_accumTime = 0.f;

	_callBack = callBack;

	SetIsEnable(true);

	if (_uiRenderer != nullptr)
		_uiRenderer->SetIsEnable(true);

	GetGameObject()->SetIsRender(true);
}

void SceneFader::FadeIn(float fadeInTime)
{
	_currFadeState = FADE_STATE::FADE_IN;

	_fadeTime = fadeInTime;

	_accumTime = 0.f;

	SetIsEnable(true);

	if (_uiRenderer != nullptr)
		_uiRenderer->SetIsEnable(true);

	GetGameObject()->SetIsRender(true);
}

void SceneFader::Start()
{
	_uiRenderer = GetGameObject()->GetComponent<Muscle::UIRenderer>();

	_uiRenderer->SetSortOrder(0);

	// ����� ���� ȭ�� ����
	_uiRenderer->GetUIData()->_drawInfo->_color = Vector4(1.f, 1.f, 1.f, 0.f);

	_uiRenderer->SetTextureID(ResourceManager::Get()->GetTextureID(TEXT("Fade")));

	_uiRenderer->SetDrawNDCPosition(Vector2(-1.f, 1.f), Vector2(1.f, -1.f));

	_uiData = _uiRenderer->GetUIData();

	GetGameObject()->SetIsRender(false);
}

void SceneFader::Update()
{
	if (GameProcess::Get()->GetIsPausePanelUp() || _currFadeState == FADE_STATE::NONE)
		return;

	if (_currFadeState == FADE_STATE::FADE_END)
	{
		_accumTime += Muscle::CTime::GetGameDeltaTime();

		// ���̵� ������ �ð� ������ �������� ���� ..
		if (_accumTime - _fadeTime >= 5.f)
		{
			SetIsEnable(false);

			GetGameObject()->SetIsRender(false);

			_uiRenderer->SetIsEnable(false);

			_currFadeState = FADE_STATE::NONE;
		}
	}

	switch (_currFadeState)
	{
		case FADE_STATE::FADE_IN:
		{
			_accumTime += Muscle::CTime::GetGameDeltaTime();

			// ���� �������.
			_uiData->_drawInfo->_color = std::move(Vector4(1.f, 1.f, 1.f, (_fadeTime - _accumTime) / _fadeTime));

			// ����.
			if (_fadeTime <= _accumTime)
			{
				// TO DO : Set Enable �ʿ��� (��� ���� ���� �׸��� �ִ� ���̴ϱ� ..)
				_currFadeState = FADE_STATE::FADE_END;
			}
			
			break;
		}

		case FADE_STATE::FADE_OUT:
		{
			_accumTime += Muscle::CTime::GetGameDeltaTime();

			// ���� ��ο�����.
			_uiData->_drawInfo->_color = std::move(Vector4(1.f, 1.f, 1.f, (_accumTime / _fadeTime)));

			if (_fadeTime <= _accumTime)
			{
				if (_callBack != nullptr)
					_callBack();

				_currFadeState = FADE_STATE::FADE_END;
			}

			break;
		}

		default:
			break;
	}
}