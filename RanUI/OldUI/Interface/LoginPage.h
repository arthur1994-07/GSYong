#pragma	once

#include "../Util/UIOuterWindow.h"

class COuterInterface;
class CUIEditBoxMan;
class GLGaeaClient;

class CLoginPage : public CUIOuterWindow
{
	enum
	{
		LOGIN_OK = ET_CONTROL_NEXT,
		LOGIN_CANCEL,
		LOGIN_EDIT_MAN,
		LOGIN_EDIT_ID,
		LOGIN_EDIT_PW,
		LOGIN_EDIT_RP,
		LOGIN_PAGE_IDSAVE_BUTTON
	};

public:
	static int nLIMIT_ID;
	static int nLIMIT_PW;
	static int nLIMIT_RP;

public:
	CLoginPage(GLGaeaClient* pGaeaClient, COuterInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CLoginPage();

public:
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );

	virtual	void SetVisibleSingle( BOOL bVisible );

public:
	void CreateSubControl();
	CBasicButton * CreateFlipButton( char* szButton, char* szButtonFlip, UIGUID ControlID );

	void ResetAll();
	void SetCharToEditBox( TCHAR cKey );
	void DelCharToEditBox();
	void GoNextTab();
	void GoThisTab();

private:
	BOOL CheckString( CString strTemp );
	void LoadIDSaveButton();

private:
    GLGaeaClient* m_pGaeaClient;
	CUIEditBoxMan * m_pEditBoxMan;
	CBasicButton * m_pIDSaveButton;
	CBasicTextBox * m_pRandTextBox;

	INT m_nRPUpdateCnt;

};