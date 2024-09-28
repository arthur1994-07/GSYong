#pragma	once

#include "../Util/UIWindowEx.h"
//#include "../../../RanLogicClient/InterfaceBaseDefine.h"
#include "../../../EngineLib/GUInterface/InterfaceBaseDefine.h"

#include "../../InnerInterface.h"

class CInnerInterface;
class GLGaeaClient;
class CBasicLineBoxSmart;
class CBasicTextButton;
class CBasicTextBox;
class CCTFResultRanking;

class CCTFResultWindow : public CUIWindowEx
{
protected:
    enum
    {
        CTF_RESULT_WINDOW_NONE = ET_CONTROL_NEXT,
		CTF_RESULT_SORTBUTTON_TOTAL,
		CTF_RESULT_SORTBUTTON_SM,
		CTF_RESULT_SORTBUTTON_HA,
		CTF_RESULT_SORTBUTTON_BH,

		CTF_RESULT_RANKING,

		CTF_RESULT_BUTTON_CLOSE,
    };

protected:
	struct SORTBUTTON : public CUIGroup
	{
		CBasicLineBoxSmart* ACTIVE;
		CBasicLineBoxSmart* INACTIVE;
		CBasicTextBox*      TEXTBOX;
		bool                ENABLE;

		SORTBUTTON(EngineDeviceMan* pEngineDevice)
			: CUIGroup(pEngineDevice)
			, ACTIVE(NULL)
			, INACTIVE(NULL)
			, TEXTBOX(NULL)
			, ENABLE(FALSE)
		{
		}

		void CreateSubControl ( const std::string& strLabel );
		void SetEnable        ( const bool bEnable );
	};	

public:
    CCTFResultWindow ( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    virtual	~CCTFResultWindow ();

public:
    virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

public:
    void CreateSubControl ();

public:
	void Show();
	void Update();
	void UpdateTime();
	void SelectSortButton( const UIGUID cID );

protected:
	CBasicLineBoxSmart* CreateLineBox ( const std::string& strControlName, const std::string& strTexInfo, const UIGUID cID=NO_ID );

private:
	SORTBUTTON*	m_pSortButton_Total;
	SORTBUTTON*	m_pSortButton_SM;
	SORTBUTTON*	m_pSortButton_HA;
	SORTBUTTON*	m_pSortButton_BH;

	CCTFResultRanking* m_pRanking;
	CBasicTextBox* m_pText_Time;

	CBasicTextButton* m_pCloseButton;

protected:
    GLGaeaClient* m_pGaeaClient;

};

class MyCTFResultWindow : public ICTFResultWindow, private CCTFResultWindow
{
public:
    MyCTFResultWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    virtual ~MyCTFResultWindow() {};

public:
    virtual void CreateUIWindowAndRegisterOwnership();
	virtual void Show();
	virtual void Update();
};
