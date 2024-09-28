#pragma	once

#include "../Util/UIOuterWindow.h"

class COuterInterface;
class CUIEditBoxMan;
class GLGaeaClient;

class CSecPassCheckPage : public CUIOuterWindow
{
	enum
	{
		CHECKSECPASS_OK = ET_CONTROL_NEXT,
		CHECKSECPASS_CANCEL,
		CHECKSECPASS_REINPUT,
		CHECKSECPASS_EDIT_MAN,
		SECPASS_CHECKPAGE_EDIT_ID,
		SECPASS_CHECKPAGE_EDIT_PW,
	};

public:
	static int nLIMIT_ID;
	static int nLIMIT_PW;

public:
	CSecPassCheckPage(GLGaeaClient* pGaeaClient, COuterInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CSecPassCheckPage();

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
	CUIEditBoxMan * m_pEditBoxMan;
    GLGaeaClient* m_pGaeaClient;
};