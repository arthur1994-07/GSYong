#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../../../EngineLib/DxLand/DxLandGateMan.h"
#include <vector>

class	CBasicTextBox;
class	CBasicTextButton;
class	GLGaeaClient;

class CMapMoveDisplay : public CUIGroup
{
private:
	enum
	{
		MAPMOVE_OKBUTTON = NO_ID + 1,
	};

public:
	CMapMoveDisplay(GLGaeaClient* pGaeaClient, EngineDeviceMan* pEngineDevice);
	virtual	~CMapMoveDisplay ();

public:
	void	CreateSubControl ();
	CBasicTextButton* CreateTextButton ( char* szButton, UIGUID ControlID , char* szText );

public:
	void	DoMAPMOVE ();

public:
	void	SetMapName ( CString strName );

public:
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

private:
	CBasicTextBox*		m_pTextBox;
	CBasicTextButton*	m_pMoveOKButton;
	GLGaeaClient*		m_pGaeaClient;	
};

class CBasicScrollBarEx;
class CMultiMapMoveDisplay : public CUIGroupHelper
{
private:
	enum
	{
		MULTIMAPMOVE_OKBUTTON = NO_ID + 1,

		MULTIMAPMOVE_TEXT_BOX_0,
		MULTIMAPMOVE_TEXT_BOX_1,
		MULTIMAPMOVE_TEXT_BOX_2,

		MULTIMAPMOVE_VISIBLE_NLINE = 3,
	};
	typedef std::vector<CString> MapNameVector;

public:
	CMultiMapMoveDisplay(GLGaeaClient* pGaeaClient, EngineDeviceMan* pEngineDevice);
	virtual	~CMultiMapMoveDisplay ();

public:
	void	CreateSubControl ();

public:
	void AddMapName(const CString& strName);
	void Done(void);

public:
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
	virtual	void SetVisibleSingle(BOOL bVisible);

private:
	CBasicScrollBarEx*	m_pScrollBar;
	CUIControl*				m_pSelectBox[MULTIMAPMOVE_VISIBLE_NLINE];
	CBasicTextBox*		m_pTextBox[MULTIMAPMOVE_VISIBLE_NLINE];
	CBasicTextButton*	m_pMoveOKButton;
	GLGaeaClient*			m_pGaeaClient;

	MapNameVector		m_mapNameVector;
	DWORD					m_nSelectIndex;
	DWORD					m_nViewIndex;
	bool						m_bDone;
};


