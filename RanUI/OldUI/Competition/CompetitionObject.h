#pragma	once

#include "../Util/UIWindowObject.h"
#include "../Util/TapSelectorEx.h"

class CInnerInterface;
class CBasicLineBoxSmart;
class CBasicTextBox;
class CBasicTextButton;

////////////////////////////////////////////////////////////////////
//	사용자 메시지 정의
const DWORD UIMSG_COMPETITION_OBJECT_TAP_CHANGE = UIMSG_WINDOW_OBJECT_NEXT;
////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------//

// struct SCOMPETITION_OBJECT_TAPBUTTON : public CUIGroup
// {
// 	CBasicLineBoxSmart* ACTIVE;
// 	CBasicLineBoxSmart* INACTIVE;
// 	CBasicTextBox*      TEXTBOX;
// 	bool                ENABLE;
// 
// 	SCOMPETITION_OBJECT_TAPBUTTON(EngineDeviceMan* pEngineDevice)
// 		: CUIGroup(pEngineDevice)
// 		, ACTIVE(NULL)
// 		, INACTIVE(NULL)
// 		, TEXTBOX(NULL)
// 		, ENABLE(FALSE)
// 	{
// 	}
// 
// 	void CreateSubControl ( const std::string& strLabel );
// 	void SetEnable        ( const bool bEnable );
// };

//---------------------------------------------------------------------//

class CCompetitionObject : public CUIWindowObject, private CTapSelectorEx<DEFAULT_TAPBUTTON>
{

protected:
	typedef CTapSelectorEx<DEFAULT_TAPBUTTON>::BUTTON		STAPBUTTON;
//	typedef CTapSelectorEx<STAPBUTTON>						TAPSELECTOR;

protected:
	enum
	{
		COMPETITION_OBJECT_NONE = WINDOW_OBJECT_NEXT,

		COMPETITION_OBJECT_TAP_CTF,
		COMPETITION_OBJECT_TAP_PVE,
		COMPETITION_OBJECT_TAP_TOURNAMENT,

		COMPETITION_OBJECT_CLOSE,
	};

public:
	enum
	{
		TAP_CTF	  = COMPETITION_OBJECT_TAP_CTF,
		TAP_PVE   = COMPETITION_OBJECT_TAP_PVE,
		TAP_TOURNAMENT = COMPETITION_OBJECT_TAP_TOURNAMENT,

		TAP_MAX     = 3,
		TAP_DEFAULT = TAP_CTF,
	};

public:
	CCompetitionObject ( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual	~CCompetitionObject ();

public:
	virtual void		Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void		TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

public:
	void		        SelectTapCTF () { TapSelect(TAP_CTF); }
	const UIGUID		GetSelectedTapID    () { return GetTapID(); }

protected:
	virtual void		CreateSubControlEx ();

private:
// 	virtual void	    TapEnable  ( TAPSELECTOR::BUTTON* );
// 	virtual void		TapDisable ( TAPSELECTOR::BUTTON* );

private:
	CBasicLineBoxSmart* CreateLineBox( const std::string& strControl, const std::string& strTexInfo );

private:
	STAPBUTTON*         m_pTapButton[TAP_MAX];
	CBasicTextButton*	m_pCloseButton;

};
