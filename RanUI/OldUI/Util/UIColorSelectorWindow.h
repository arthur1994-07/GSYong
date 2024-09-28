
// bjju.sns

#pragma	once

#include "./UIWindowObjectController.h"
#include "../../../RanLogicClient/InterfaceBaseType.h"

#include <vector>
#include <boost/function.hpp>

//-----------------------------------------------------------------------------------------------//

class CInnerInterface;
class GLGaeaClient;
class CBasicTextButton;
class CBasicLineBox;
class CBasicTextBox;

class CUIWindowObject;
class CUIColorPannelPage;
class CUIColorPickerPage;

//class CSNSFacebookPage;
//class CSNSTwitterPage;

class CUIColorSelectorWindow : public CUIWindowObjectController
{

protected:
    enum
    {
        COLORSELECTOR_WINDOW_NONE   = NO_ID,
        COLORSELECTOR_WINDOW_OBJECT,

        COLORSELECTOR_WINDOW_PAGE_PANNEL,
        COLORSELECTOR_WINDOW_PAGE_PICKER,
    };

public:
    enum
    {
        PAGE_REQ_NONE = 0,
        PAGE_REQ_PANNEL_SELECT,
        PAGE_REQ_PANNEL_RANDOM,
        PAGE_REQ_PANNEL_RESET,
        PAGE_REQ_PANNEL_OK,
        PAGE_REQ_PANNEL_CANCEL,
        PAGE_REQ_PICKER_UPDATE,
        PAGE_REQ_PICKER_SELECT_OK,
        PAGE_REQ_PICKER_SELECT_CANCEL,
        /*
        SNS_PAGE_REQ_NONE = 0,
        SNS_PAGE_REQ_MENU_PAGE_OPEN,
        SNS_PAGE_REQ_FACEBOOK_PAGE_OPEN,
        SNS_PAGE_REQ_TWITTER_PAGE_OPEN,
        */
    };

public:
    typedef std::vector<DWORD>                                       COLORARRAY;

public:
    typedef boost::function<void (UI::SPANNELGROUPINFOARRAY&)>       FuncCreate;
    typedef boost::function<void (UI::SPANNELGROUPINFOARRAY&)>       FuncInit;
    typedef boost::function<void (const UI::SPANNELGROUPINFOARRAY&)> FuncUpdate;
    typedef boost::function<void (const UI::SPANNELGROUPINFOARRAY&)> FuncCommit;

public:
    CUIColorSelectorWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
    virtual ~CUIColorSelectorWindow();

public:
    virtual void		Update				( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    virtual void		UpdatePageRequest	( UIGUID ControlID, const SPAGEREQUEST& sRequest );
    virtual	void		TranslateUIMessage	( UIGUID ControlID, DWORD dwMsg );

public:
    void				CreateSubControl ( LPDIRECT3DDEVICEQ pd3dDevice, const std::string& strTitle, FuncCreate _1f, FuncInit _2f, FuncUpdate _3f, FuncCommit _4f );

public:
    virtual	void		SetVisibleSingle( BOOL bVisible );

public:
    void                SetRestrict( const bool bRestrict );

public:
   // CSNSMenuPage*		GetMenuPage     () { return m_pPage_Menu;	  }
   // CSNSFacebookPage*	GetFacebookPage () { return m_pPage_Facebook; }
   // CSNSTwitterPage*	GetTwitterPage  () { return m_pPage_Twitter;  }


private:
    CUIWindowObject*	        m_pUIObject;

private:
    CUIColorPannelPage*	        m_pPage_Pannel;
    CUIColorPickerPage*         m_pPage_Picker;
    // CSNSFacebookPage*	m_pPage_Facebook;
    //CSNSTwitterPage*	m_pPage_Twitter;

private:
    UI::SPANNELGROUPINFOARRAY   m_InfoArray;
    UI::SPANNELGROUPINFOARRAY   m_InfoArray_Org;
    DWORD                       m_dwColorBackup;

private:
    FuncCreate                  m_FuncCreate;
    FuncInit                    m_FuncInit;
    FuncUpdate                  m_FuncUpdate;
    FuncCommit                  m_FuncCommit;

protected:
    GLGaeaClient*               m_pGaeaClient;

};
