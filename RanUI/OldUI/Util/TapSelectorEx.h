
#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "RnButton.h"

#include <map>

class CBasicLineBoxSmart;
class CBasicTextBox;
class CBasicTextButton;

class CCheckBox;

struct DEFAULT_TAPBUTTON : public CUIGroup
{
public:
	DEFAULT_TAPBUTTON(EngineDeviceMan* pEngineDevice)
		: CUIGroup(pEngineDevice)
		, pActiveLineBox(NULL)
		, pInactive(NULL)
		, pTextBox(NULL)
		, pEnable(FALSE)
	{
	}

	void CreateSubControl ( const std::string& strLabel,const std::string& activeBaseID,
		const std::string& activeSubID,const std::string& inactiveBaseID,
		const std::string& inactiveSubID,const std::string& textBoxID );

	void SetEnable        ( const bool bEnable );

	void SetTabLabel( const std::string& strLabel );

private:
	CBasicLineBoxSmart* pActiveLineBox;
	CBasicLineBoxSmart* pInactive;
	CBasicTextBox*      pTextBox;
	bool                pEnable;
};

struct DEFAULT_RADIO_BUTTON : public CUIGroup
{
public:
	DEFAULT_RADIO_BUTTON(EngineDeviceMan* pEngineDevice) : CUIGroup(pEngineDevice)
	{
		pCheckBox = NULL;
		pTextBox = NULL;
	}

	void CreateSubControl(const std::string& text,DWORD align,const std::string& textID,DWORD textColor,const std::string& radioButtonID,
		const std::string& onButton,const std::string& offButton);
    void CreateSubControl(const std::string& text,DWORD align,const std::string& textID,DWORD textColor,const std::string& radioButtonID,
        const std::string& onButton,const std::string& offButton,CD3DFontPar* pFont);


	void SetEnable        ( const bool bEnable );
    void SetText( const std::string& text, DWORD textColor );

private:
	CCheckBox*     pCheckBox;
	CBasicTextBox* pTextBox;
};

struct TAPBUTTON_RNBUTTON : public RnButton
{
	TAPBUTTON_RNBUTTON(EngineDeviceMan* pEngineDevice) : RnButton(pEngineDevice) {}
	virtual void SetEnable(bool isEnable);
};

template<typename _BUTTONCONTROL>
class CTapSelectorEx
{

public:
    typedef _BUTTONCONTROL                              BUTTON;
    typedef typename std::map<UIGUID, _BUTTONCONTROL*>  BUTTONMAP;
    typedef typename std::pair<UIGUID, _BUTTONCONTROL*> BUTTONMAP_PAIR;
    typedef typename BUTTONMAP::iterator                BUTTONMAP_ITER;

public:
    enum EMTAPSELECTORRESULT
    {
        EMTAPSELECTORRESULT_NONE,	// 아무일도 없음
        EMTAPSELECTORRESULT_SELECT, // 선택됨
    };

public:
    CTapSelectorEx ();
    virtual	~CTapSelectorEx ();

    const bool	            RegisterTapButton ( _BUTTONCONTROL* pControl );
    const bool 				TapSelect ( const UIGUID ControlID );
	const bool				TapOver(const UIGUID ControlID);
    const UIGUID			GetTapID ()					      { return m_TapSelectedID;	    }

protected:
	void	 TapSelectorUIMessage ( const UIGUID ControlID, const DWORD dwMsg, EMTAPSELECTORRESULT& emOutResult );

private:
	// 탭이 선택되면 호출되는 함수
	virtual void EventSelectedTap( UIGUID ControlID ) {}

	virtual void EventOverTap(UIGUID ControlID) {} 

private:
	void TapEnable  ( BUTTON* pButton) 
	{
		if ( pButton )
			pButton->SetEnable( true );
	}

	void TapDisable ( BUTTON* pButton)
	{
		if ( pButton )
			pButton->SetEnable( false );
	}

private:
    UIGUID					m_TapSelectedID;
    _BUTTONCONTROL*         m_TapSelectedControl;

private:
    BUTTONMAP	            m_TapButtonMap;

};

//---------------------------------------------------------------------------------------------//

#include "TapSelectorEx.inl"

//---------------------------------------------------------------------------------------------//