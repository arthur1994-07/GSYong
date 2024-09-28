#pragma once



#include "../../InnerInterface.h"
#include "../Util/TapSelectorEx.h"
#include "../Util/UIGroupHelperMessageBox.h"
// 
class GLGaeaClient;
class CMessageBox;
class CBasicScrollBarEx;
class CUIEditBoxMan;

struct SFRIEND;

class RnBlockTab : public CUIGroupHelperMessageBox
{
private:
    typedef std::vector< SFRIEND* > FRIEND_PTR_VECTOR;

    class CItem : public CUIGroupHelper
    {
    public:
        CItem( EngineDeviceMan* pEngineDevice );
        void	            CreateSubControl();
        void                SetData( const SFRIEND* pFriend );  // 친구 이름으로 표시
        void                Reset();

    private:
        EMCHARINDEX         GetGLCCToGLCI( EMCHARCLASS eGLCC );
        CBasicLineBoxSmart* m_pClassIcon[GLCI_NUM_TRICKER];      //온라인 오프라인
        CBasicTextBox*      m_pText_CharName;
    };

private:
    enum
    {
        LIST_Y          = 11,   // 목록 줄 개수
    };

    enum
    {
        ID_NOT_USE = ID_NEXT,
        ID_TEXTBOX_BOX,
        ID_TEXTBOX_EDIT,
        ID_TEXTBOX_BACKGROUND,

        ID_BTN_BLACK,
        ID_BTN_DEL,

        ID_LIST_CONTROL = 100,
        ID_LIST_CONTROL_START = ID_LIST_CONTROL,
        ID_LIST_CONTROL_END = ID_LIST_CONTROL_START + LIST_Y,
    };
public:
    enum eIDENTI
    {
        //nIdentity : MessageBox를 식별하기 위해서 사용한다.
        IDENTI_NOT_USE = 0,
        IDENTI_ADDBLOCK,
        IDENTI_DELBLOCK,
    };

public:
    RnBlockTab( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );    
    void	            CreateSubControl();
    virtual void        Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    virtual void        TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
    virtual void        TranslateMessageBox( unsigned int nIdentity, DWORD dwMsg );
    virtual void        Refresh();

private:
    SFRIEND*            FindFirend( int nIndex );
    std::string         GetEditBoxString();
    GLGaeaClient*       m_pGaeaClient;
    CInnerInterface*    m_pInterface;

    CBasicTextBox*      m_pText_ListCount;
    CItem*              m_pSlot[LIST_Y];
    CUIEditBoxMan*      m_pEditBox;
    
    CBasicScrollBarEx*  m_pScrollBar;
    unsigned int        m_nViewPos;

    FRIEND_PTR_VECTOR   m_vecFriendPtr;

    //선택 박스
    CUIControl*         m_pYLine;
    UIRECT              m_rcSeleted;

private:
    int                 m_nSelectedPos;
    std::string         m_strInputData;


// 
// private:
//     SFRIEND*            FindFriend( const RnFriendSlot::SDATA_POS& sDataPos );
//     std::string         GetEditBoxString();
//     bool                m_bInit;
//     RnFriendSlot::SDATA_POS m_sSelectedData;
//     unsigned int        m_nViewPos;
// 
//     GROUP_FRIEND_MAP    m_mapGroupFirend;
//     float               m_fUpdateTime;
// 
//     int                 m_nCurrentFilter; 
//     std::string         m_strInputData;
};
