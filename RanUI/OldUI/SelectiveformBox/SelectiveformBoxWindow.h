
#pragma once

#include "../Util/UIWindowEx.h"

class GLGaeaClient;
class CInnerInterface;
class CBasicTextBox;
class CBasicTextButton;
struct SINVENITEM;

class SelectiveformBoxPage;


// UI Cfg;
// GUI/UiInner/Selectiveform/Selectiveform.xml;

class SelectiveformBoxWindow : public CUIWindowEx
{
protected:	// UI Id
	enum
	{
		// SELFORMW
		// SELect
		//    Form
		//        Window
		SELFORMW_PAGE = ET_CONTROL_NEXT,
		SELFORMW_SELECT,
		SELFORMW_CANCEL,
	};

private:
	GLGaeaClient*	m_pGaeaClient;

	CBasicTextButton*	m_pBtnSelect;
	CBasicTextButton*	m_pBtnCancel;
	CBasicTextBox*		m_pExplainTextBox;
	SelectiveformBoxPage* m_pPage;

public:
	SelectiveformBoxWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual ~SelectiveformBoxWindow(void);

public:
	void CreateSubControl(void);

	virtual	HRESULT	RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual	void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID cID, DWORD dwMsg );
	CBasicTextButton* CreateTextButton ( char* szButton, UIGUID ControlID, char* szText );
	
	void Open(std::vector<ITEM::SBOX_ITEM>& vecItemID);
	BOOL GetSelectPos(INT& nPos) const;

private:
	
};