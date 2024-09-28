#pragma once

#include "../Util/UIPage.h"
#include "../../../RanLogicClient/InterfaceBaseType.h"

#include <string>
#include <vector>
#include <boost/function.hpp>

//-----------------------------------------------------------------------------------------------//

class CBasicTextBox;
class CBasicTextButton;
class CInnerInterface;
class CSwapImage;
class CUITextButton;

class CUIColorPannelPage : public CUIPage
{

public:
    enum
    {
        COLORPANNEL_PAGE = PAGE_NEXT,

        COLORPANNEL_PAGE_BUTTON_RANDOM,
        COLORPANNEL_PAGE_BUTTON_RESET,
        COLORPANNEL_PAGE_BUTTON_CANCEL,
        COLORPANNEL_PAGE_BUTTON_OK,

        COLORPANNEL_PAGE_PANNEL_BEGIN,
    };

public:
    enum
    {
        SELECT_NULL = -1,
        COLOR_NULL  = 0xFF000000,
        MAX_PANNEL  = UI::SCOLORPANNELGROUP_INFO::MAX_PANNEL,
    };

public:
    static const DWORD dwPANNELEXPANDSIZE_X = 97;
    static const DWORD dwPANNELGROUPGAP_Y   = 3;

public:
    struct SPANNELGROUP
    {
        CBasicTextBox*  pLABEL;
        CSwapImage*     pPANNEL[MAX_PANNEL];
        CUIControl*     pCOLOR[MAX_PANNEL];
        DWORD           dwNUMPANNEL;
        BOOL            bENABLE;

        SPANNELGROUP()
            : pLABEL(NULL)
            , dwNUMPANNEL(0)
            , bENABLE(FALSE)
        {
            memset(pPANNEL, 0, sizeof(pPANNEL));
            memset(pCOLOR, 0, sizeof(pCOLOR));
        }
    };


public:
    typedef std::vector<DWORD>                        COLORARRAY;
    typedef std::vector<SPANNELGROUP>                 SPANNELGROUPARRAY;
    typedef std::vector<SPANNELGROUP>::iterator       SPANNELGROUPARRAY_ITER;

public:
    CUIColorPannelPage( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    virtual ~CUIColorPannelPage();

public:
    void         CreateSubControl  ( const UI::SPANNELGROUPINFOARRAY& sInfoArray );

public:
    virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
    virtual	void SetVisibleSingle( BOOL bVisible );

public:
    void         SetRestrict( const BOOL bRestrict );
    void         SelectPannel( const int nGroup, const int nPannel );
    void         SetAttribute( const UI::SPANNELGROUPINFOARRAY& sInInfoArray );
    void         GetAttribute( UI::SPANNELGROUPINFOARRAY&       sOutInfoArray );
    void         SetSelectColor( const DWORD dwColor );
    const DWORD  GetSelectColor();
    const int    GetSelectGroupIndex()  { return m_nSelectGroup;  }
    const int    GetSelectPannelIndex() { return m_nSelectPannel; }

protected:
    virtual void OpenSubControl();

protected:
    virtual void GetOrgLocalPos ( UIRECT& OutRect ) { OutRect = m_rcPage; }

protected:
    void	     AddPannelGroup( const UI::SCOLORPANNELGROUP_INFO& sInfo );
    void         UnselectAllPannel();
    void         RandomAllPannelColor();
    const DWORD  GetRandomColor();

private:
    SPANNELGROUPARRAY       m_PannelGroupArray;

    CUITextButton*          m_pButton_Random;
    CUITextButton*          m_pButton_Reset;
    CUITextButton*          m_pButton_Cancel;
    CUITextButton*          m_pButton_Ok;

private:
    int                     m_nSelectGroup;
    int                     m_nSelectPannel;
    UIRECT                  m_rcPage;
    DWORD                   m_nMaxPannelCount;
    BOOL                    m_bRestrict;

protected:
    CInnerInterface*        m_pInterface;

};
