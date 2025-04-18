#pragma	once

#include "../Util/UIWindowEx.h"

class CInnerInterface;
class CBasicTextButton;
class CBasicVideoOption;
class CBasicAudioOption;
class CBasicGamePlayOption;
class GLGaeaClient;

class CBasicHWOptionWindow : public CUIWindowEx
{
protected:
	enum
	{
		HWOPTION_VIDEO = ET_CONTROL_NEXT,
		HWOPTION_AUDIO,
		HWOPTION_GAMEPLAY,
		HWOPTION_VIDEOPAGE,
		HWOPTION_AUDIOPAGE,
        HWOPTION_GAMEPLAYPAGE,
		HWOPTION_APPLY,
		HWOPTION_OK
	};

public:
	CBasicHWOptionWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CBasicHWOptionWindow ();

protected:
    GLGaeaClient* m_pGaeaClient;

public:
	void	CreateSubControl ();

protected:
	void	SetVisiblePage ( int nPage );
	void	GamePlayOption_OK();
	void	AudioOption_OK();
	void	VideoOption_OK();

public:
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
	virtual	void SetVisibleSingle ( BOOL bVisible );

private:
	CBasicTextButton* m_pVideoButton;
	CBasicTextButton* m_pAudioButton;
	CBasicTextButton* m_pGamePlayButton;

private:
	CBasicVideoOption*		m_pVideoOption;
	CBasicAudioOption*		m_pAudioOption;
	CBasicGamePlayOption*	m_pGamePlayOption;
};