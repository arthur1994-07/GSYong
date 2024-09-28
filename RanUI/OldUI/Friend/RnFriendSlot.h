#pragma once


#include "../../InnerInterface.h"

#include "../Util/TapSelectorEx.h"
#include "../Util/UIGroupHelper.h"

#include "../../../RanLogic/Character/GLCharDefine.h"
#include "../../../RanLogic/Party/GLPartyDefine.h"

class CBasicLineBoxSmart;

////////////////////////////////////////////////////////////////////
//	사용자 메시지 정의
const DWORD UIMSG_REDUCTION_CLICK = UIMSG_USER1;
const DWORD UIMSG_EXPANSION_CLICK = UIMSG_USER2;

class RnFriendSlot : public CUIGroupHelper
{
public:
    // 현재 데이터 위치를 저장한다.
    struct SDATA_POS
    {
        SDATA_POS()
        {
            wGroupId = 0;
            nDataIndex = 0;
        }
        SDATA_POS( WORD wGroupId, unsigned int nDataIndex)
        {
            this->wGroupId = wGroupId;
            this->nDataIndex = nDataIndex;
        }
        WORD wGroupId;
        unsigned nDataIndex;
    };


    typedef std::vector< const std::tr1::shared_ptr<SFRIEND> >             FRIEND_PTR_VECTOR;

    enum eSlotState
    {   
        SLOT_NONE,          // 알 수 없는(잘못 된) 상태
        SLOT_EMPTY,         // 비어있는 상태
        SLOT_GROUP,         // 그룹 표시 상태
        SLOT_FRIEND,        // 친구 표시 상태
    };
private:
    enum
    {
//         ID_BTN_REDUCTION = NO_ID + 1,
//         ID_BTN_EXPANSION, 
        ID_BTN_REDUCTION = NO_ID + 1,
        ID_BTN_EXPANSION, 
    };
    enum
    {
        ICON_OFFLINE,
        ICON_ONLINE,
        ICON_MAX,
    };

    
public:
    RnFriendSlot( GLGaeaClient* pGaeaClient, EngineDeviceMan* pEngineDevice );
    void	            CreateSubControl();
    virtual void        TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
    void                Reset();

private:
    GLGaeaClient*       m_pGaeaClient;
    CUIControl*         m_pItemBG;
    CBasicLineBoxSmart* m_pIcon_PartyState;
    CBasicLineBoxSmart* m_pClassIcon[GLCI_NUM_TRICKER][ICON_MAX];   //온라인 오프라인
    RnButton*           m_pButton_GroupClose;
    RnButton*           m_pButton_GroupOpen;
    CBasicLineBoxSmart* m_pIcon_GroupImgOpen;
    CBasicLineBoxSmart* m_pIcon_GroupImgClose;
    //CBasicLineBoxSmart* m_pIcon_GroupImg;
    CBasicTextBox*      m_pText_GroupName;
    CBasicTextBox*      m_pText_Level;
    CBasicTextBox*      m_pText_CharName;
    CBasicTextBox*      m_pText_MapName;
    CBasicTextBox*      m_pText_Coord;
    CBasicTextBox*      m_pText_Channel;

    eSlotState          m_eSlotState;

public:
    eSlotState			GetSlotState();
    SDATA_POS		GetDataPos();
    WORD				GetGroupNum();
    std::string			GetGroupName();
    const char*		GetCharName();
    EMCHARINDEX		GetGLCCToGLCI( EMCHARCLASS eGLCC );
    const GLPartyID&	GetPartyID(void) const;

    void                SetData( const SDATA_POS sDataPos, const FRIEND_PTR_VECTOR& vecFriendPtr, bool bOpen = true );     // 그룹으로 표시
    void                SetData( const SDATA_POS sDataPos, const std::tr1::shared_ptr<SFRIEND> pFriend );                    // 친구 이름으로 표시

private:
    SDATA_POS           m_sDataPos;
    std::string         m_strCharName;
    GLPartyID               m_PartyID;
};
