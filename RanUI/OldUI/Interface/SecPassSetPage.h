#pragma	once

#include "../Util/UIOuterWindow.h"

class COuterInterface;
class CUIEditBoxMan;
class GLGaeaClient;

class CSecPassSetPage : public CUIOuterWindow
{
	enum
	{
		SETSECPASS_OK = ET_CONTROL_NEXT,
		SETSECPASS_CANCEL,
		SETSECPASS_EDIT_MAN,
		SECPASS_SETPAGE_EDIT_ID,
		SECPASS_SETPAGE_EDIT_PW,
		SECPASS_SETPAGE_EDIT_PWCHECK,
	};

public:
	const static int nLIMIT_PW;
	const static int nLIMIT_ID;

public:
	CSecPassSetPage(GLGaeaClient* pGaeaClient, COuterInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CSecPassSetPage();

public:
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );

	virtual	void SetVisibleSingle( BOOL bVisible );

public:
	void CreateSubControl();

	void ResetAll();
	void SetCharToEditBox( TCHAR cKey );
	void DelCharToEditBox();
	void GoNextTab();
	void GoThisTab();

private:
	BOOL CheckString( CString strTemp );

private:
	CUIEditBoxMan* m_pEditBoxMan;
    GLGaeaClient* m_pGaeaClient;
};