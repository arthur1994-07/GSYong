
#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include <map>

class CBasicTextButton;

class CTapSelector
{

public:
	enum EMTAPSELECTORRESULT
	{
		EMTAPSELECTORRESULT_NONE,	 // 아무일도 없음
		EMTAPSELECTORRESULT_SELECT, // 선택됨
	};

public:
	CTapSelector ();
	virtual	~CTapSelector ();

public:
	CBasicTextButton* CreateTapButton ( CUIGroup* pParent, const char* szKeyword, const char* szControl, const char* szText, int nType, UIGUID WndID, EngineDeviceMan* pEngineDevice );

public:
	virtual	void			TapSelectorUIMessage ( const UIGUID ControlID, const DWORD dwMsg, EMTAPSELECTORRESULT& emOutResult );
;
public:
	const BOOL 				TapSelect    ( const UIGUID ControlID );

public:
	CBasicTextButton*		GetTapButton ( const UIGUID dwID ) { return m_MapTapButton[dwID]; }
	const DWORD				GetTapID  ()					   { return m_SelectedTapID;	  }

private:
	void					TapEnable  ( CBasicTextButton* );
	void					TapDisable ( CBasicTextButton* );

private:
	UIGUID					m_SelectedTapID;

private:
	std::map<UIGUID, CBasicTextButton*>	m_MapTapButton;

};