#include "StdAfx.h"
#include "../../../EngineLib/GUInterface/BasicTextBoxEx.h"
#include "../../../EngineLib/GUInterface/BasicScrollBarEx.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/BasicScrollThumbFrame.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/DxTools/DebugSet.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"

#include "../../../RanLogic/Transport/GLBusStation.h"
#include "../../../RanLogic/Transport/GLBusList.h"
#include "../../../RanLogic/NpcTalk/NpcTalk.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"

#include "../../InnerInterface.h"
#include "ChatRoomList.h"


// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// 설정 파일로 이동 할것
const int MAX_GROUP_CHAT_ROOM_NUM = 10;

CChatRoomList::CChatRoomList ( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
: CUIWindowEx ( pInterface, pEngineDevice )
, m_pGaeaClient ( pGaeaClient )
, m_pInterface ( pInterface )
, m_pEngineDevice ( pEngineDevice )
, m_pRoomList ( NULL )
{
}

CChatRoomList::~CChatRoomList ()
{
}




void CChatRoomList::CreateSubControl ()
{
    CD3DFontPar* pFont = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

    m_pRoomList = new CBasicTextBoxEx( m_pEngineDevice );
    m_pRoomList->CreateSub( this,"GROUP_CHAT_ROOM_LIST",UI_FLAG_DEFAULT, GROUP_CHAT_ROOM_LIST);
    m_pRoomList->SetFont(pFont);
    m_pRoomList->SetTextAlign(TEXT_ALIGN_CENTER_Y | TEXT_ALIGN_CENTER_X );
    m_pRoomList->SetLineInterval ( 3.0f );
    RegisterControl ( m_pRoomList );
}

void CChatRoomList::TranslateUIMessage ( UIGUID cID, DWORD dwMsg )
{


    if ( CHECK_MOUSE_IN ( dwMsg ) )		//	컨트롤 내부에 마우스가 위치하고,
    {
        DWORD dwButtonMsg = UIMSG_NOMESSAGE;
        if ( m_pClose )	dwButtonMsg = m_pClose->GetMessageEx ();
        if ( (dwMsg & UIMSG_LB_DOWN) && !CHECK_MOUSE_IN ( dwButtonMsg ) )	//	마우스를 좌클릭으로 눌렀을 때,
        {			
            SetExclusiveControl();	//	단독 컨트롤로 등록하고,
            if ( !m_bFirstGap )				//	최초 포지션 갭을 기록한다.
            {
                UIRECT rcPos = GetGlobalPos ();
                m_vGap.x = m_PosX - rcPos.left;
                m_vGap.y = m_PosY - rcPos.top;
                m_bFirstGap = TRUE;
            }
        }
        else if ( CHECK_LB_UP_LIKE ( dwMsg ) )	//	좌클릭했던것을 놓으면
        {
            ResetExclusiveControl();	//	단독 컨트롤을 해제하고
        }
    }
    else if ( CHECK_LB_UP_LIKE ( dwMsg ) )		//	컨트롤 영역밖에서 버튼을 떼는 경우가
    {											//	발생하더라도
        ResetExclusiveControl();		//	단독 컨트롤을 해제하고,
    }


    CUIWindowEx::TranslateUIMessage ( cID, dwMsg );




}


void	CChatRoomList::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{


    //m_pRoomList->ClearText();
    //m_pRoomList->AddText("adsfasdf");

    CUIWindowEx::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

}