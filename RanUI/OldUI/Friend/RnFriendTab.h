#pragma once

#include "../Util/TapSelectorEx.h"
#include "../Util/UIGroupHelperMessageBox.h"
#include "./RnFriendSlot.h"

class CBasicLineBoxSmart;
class CBasicTextBoxEx;
class CBasicScrollBarEx;
class CBasicComboBoxRollOver;
class CUIEditBoxMan;

class CMessageBox;

class GLGaeaClient;
class RnFriendSlot;

struct SFRIEND;
struct FRIEND_GROUP;

class RnFriendTab : public CUIGroupHelperMessageBox
{
public:


    typedef std::vector< const std::tr1::shared_ptr<SFRIEND> >           FRIEND_PTR_VECTOR;
    
    struct SGROUP
    {
        SGROUP() : m_bOpen( true ){}
        SGROUP( bool bOpen ) : m_bOpen( bOpen ){}
        FRIEND_PTR_VECTOR m_vecFriendPtr;
        bool m_bOpen;
    };    
    typedef std::map< WORD, SGROUP >                GROUP_FRIEND_MAP;
    typedef GROUP_FRIEND_MAP::iterator              GROUP_FRIEND_MAP_ITER;
    typedef GROUP_FRIEND_MAP::value_type            GROUP_FRIEND_MAP_VALUE;

    enum eFILTERSTATE
    {
        FILTER_ALL      = 0,
        FILTER_ONLINE   = 1,
    };
private:
    enum
    {
        LIST_Y          = 10,   // 목록 줄 개수
        UPDATE_TIME     = 2,    // 업데이트 주기
    };

    enum
    {
        ID_NOT_USE = ID_NEXT,

        ID_LISTCOUNT,
        ID_GROUPCOUNT,

        ID_BTN_ADD,
        ID_BTN_ADDGROUP,
        ID_BTN_ADDPARTY,
        ID_BTN_WHISPER,
        ID_BTN_BLOCK,
        ID_BTN_DELETE,

        ID_COMBO_BUTTON,
        ID_COMBO_TEXT,
        ID_COMBO_ROLLOVER,

        ID_TEXTBOX_BOXMAN,
        ID_TEXTBOX_EDIT,
        ID_TEXTBOX_BACKGROUND,

        ID_POPUP_FRIEND,
        ID_POPUP_GROUP,

        ID_LIST_CONTROL = 100,
        ID_LIST_CONTROL_START = ID_LIST_CONTROL,
        ID_LIST_CONTROL_END = ID_LIST_CONTROL_START + LIST_Y,
    };

public:
    enum eIDENTI
    {
        //nIdentity : MessageBox를 식별하기 위해서 사용한다.
        IDENTI_NOT_USE = 0,
        IDENTI_ADDFRIEND,
        IDENTI_ADDGROUP,
        IDENTI_ADDPARTY,
        IDENTI_ADDCHAT,
        IDENTI_ADDBLOCK,
        IDENTI_DELFRIEND,
        IDENTI_DELGROUP,
        IDENTI_EDITGROUP,
        IDENTI_MOVETOFRIEND,
    };



public:
    RnFriendTab( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    void	            CreateSubControl();
    virtual void        Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    virtual void        TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
    virtual void        TranslateMessageBox( unsigned int nIdentity, DWORD dwMsg );
    virtual void        Refresh();
    virtual void        SetVisibleSingle( bool bFlag );
    void                SetWhisperName( const std::string& strCharName );
    void                LoadGroup();
    void                LoadFriend();

private:
    GLGaeaClient*       m_pGaeaClient;
    CInnerInterface*    m_pInterface;

    CBasicTextBox*      m_pComboBoxText;
    CBasicComboBoxRollOver* m_pComboBoxRollOver;
  
    CBasicTextBox*      m_pText_ListCount;
    CBasicTextBox*      m_pText_GroupCount;
    RnFriendSlot*       m_pSlot[LIST_Y];
    CBasicScrollBarEx*  m_pScrollBar;
    CUIEditBoxMan*      m_pEditBox;

    //선택 박스
    CUIControl*         m_pYLine;
    UIRECT              m_rcSeleted;
private:
    SFRIEND*            FindFriend( const RnFriendSlot::SDATA_POS& sDataPos );
    std::string         GetEditBoxString();
    void                NoConnectMessage();
    void                ShowPartyInfo( RnFriendSlot* pCurrentSlot );
    void                OpenFriendPopup();
    void                OpenGroupPopup();
        
    bool                m_bInit;   
    unsigned int        m_nViewPos;
    RnFriendSlot::SDATA_POS m_sSelectedData;
    int        m_nFriendListSize;
    
    std::vector<FRIEND_GROUP> m_GroupInfo;
    GROUP_FRIEND_MAP    m_mapGroupFirend;
    float               m_fUpdateTime;

    int                 m_nCurrentFilter; 
    std::string         m_strInputData;
  
private:
    //! 친구 팝업 기능
    void                ActionWHISPER( int nPopupIndex );
    void                ActionVIEW_INFO( int nPopupIndex );
    void                ActionPARTY_INVITE( int nPopupIndex );
    void                ActionGROUPCHAT_INVITE( int nPopupIndex );
    void                ActionFRIEND_GROUP_MOVE( int nPopupIndex );
    void                ActionMOVE_TO_FRIEND( int nPopupIndex );
    void                ActionFRIEND_BLOCK( int nPopupIndex );
    void                ActionDELETE_FRIEND( int nPopupIndex );

    
    //! 그룹 팝업 기능
    void                ActionEDIT_GORUP( int nPopupIndex );
    void                ActionDELETE_GROUP( int nPopupIndex );


};
