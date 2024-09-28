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
#include "../../../RanLogicClient/GroupChat/GroupChatClient.h"
#include "ChatGroupWindow.h"
#include "MinChatWindow.h"
#include "MaxChatWindow.h"


// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------



CChatGroupWindow::CChatGroupWindow ( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
: CUIWindowEx ( pInterface, pEngineDevice )
, m_pGaeaClient ( pGaeaClient )
, m_pInterface ( pInterface )
, m_pEngineDevice ( pEngineDevice )
, m_RoomCount ( 0 )
, m_dwMaxRoomIndex( 0 )
{
	m_pMinChatWindow.clear();

}

CChatGroupWindow::~CChatGroupWindow ()
{
}



void CChatGroupWindow::CreateMinChatWindow ( int nRoomIndex, const char* szControlKeyword, UIGUID WndID )
{
	//미니채팅창의 부모창을 만든다 
	//부모창의 세로 길이는 미니창들의 세로 길이를 다 더한 값이다
    CMinChatWindow* pMinChatWindow = new CMinChatWindow( m_pGaeaClient, m_pInterface, m_pEngineDevice );
    pMinChatWindow->CreateSub( this,szControlKeyword,UI_FLAG_DEFAULT, WndID);
	pMinChatWindow->CreateMinChatRoom ( "" );
    pMinChatWindow->SetVisibleSingle(FALSE);
    RegisterControl ( pMinChatWindow );
	m_pMinChatWindow.push_back(pMinChatWindow);

	if ( nRoomIndex > 0 )
	{
		UIRECT rcOldPos = m_pMinChatWindow[nRoomIndex]->GetLocalPos();
		m_pMinChatWindow[nRoomIndex]->SetLocalPos(
			D3DXVECTOR2(rcOldPos.left, rcOldPos.top + CHAT_ROOM_HEIGHT * nRoomIndex ));
	}
}

void CChatGroupWindow::CreateSubControl ()
{
	//서버에서 전달받은 개수 만큼 그룹채팅 미니창을 생성한다
	DWORD dwMaxRoomIndex = m_pGaeaClient->GetChatGroup()->GetMaxGroupChatOpen();
	if( 0 < dwMaxRoomIndex )
		m_dwMaxRoomIndex = dwMaxRoomIndex;
    CD3DFontPar* pFont = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
   
	for ( DWORD i = 0; i < m_dwMaxRoomIndex; i++ )
	{
		CreateMinChatWindow( i, "MIN_GROUP_CHAT_ROOM_1", ET_CONTROL_NEXT + i );
	}
}

void CChatGroupWindow::ActiveChatRoom( int nRoomNumber, const char* szRoomName )
{
    if ( m_RoomCount < m_dwMaxRoomIndex )
    {
        UIRECT rcOldpos = this->GetGlobalPos();

        WORD wAlignFlag = GetAlignFlag ();
        SetAlignFlag ( UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        {
            UIRECT rcNewPos(rcOldpos.left, rcOldpos.top, rcOldpos.sizeX, rcOldpos.sizeY+CHAT_ROOM_HEIGHT);
            AlignSubControl( rcOldpos, rcNewPos );
            SetGlobalPos( rcNewPos );
        }
        SetAlignFlag( wAlignFlag );

        m_pMinChatWindow[m_RoomCount]->SetVisibleSingle(TRUE);
        m_pMinChatWindow[m_RoomCount]->SetMinChatRoomName(szRoomName);
        m_RoomCount++;
    }
}

int CChatGroupWindow::GetRoomIndex( DWORD dwRoomkey)
{
	//dwRoomkey 키 값으로 dwRoomkey키 값과 동일한 방의
	// 인덱스를 찾는다
	for ( DWORD i = 0; i < m_dwMaxRoomIndex; i++ )
	{
		if( m_pMinChatWindow[i]->m_dwKey == dwRoomkey )
		{
			return i;
		}			
	}	
	
	//함수를 사용하는 쪽으로 이동 할것
	/*GASSERT("dwRoomkey키가 NULL 입니다");*/
	return -1;
		
}

void CChatGroupWindow::TranslateUIMessage ( UIGUID cID, DWORD dwMsg )
{
	if ( CHECK_MOUSE_IN_LBDOWNLIKE ( dwMsg ) )
	{
		m_pInterface->UiShowGroupFocus(MIN_GROUP_CHAT_WINDOW);
	}

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



	//미니채팅창 ID - ET_CONTROL_NEXT == 방번호 
	if ( (cID - ET_CONTROL_NEXT) < static_cast<int>(m_dwMaxRoomIndex) )
	{
		if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
		{
			int nMinRoomIndex = cID - ET_CONTROL_NEXT;
			int nMaxRoomIndex;

			for ( DWORD i = 0; i < m_dwMaxRoomIndex; i++ )
			{
				//미니 챗룸의 DBNUM르로 연결된 맥스챗 룸을 찾는다
				if ( m_pMinChatWindow[nMinRoomIndex]->m_dwKey == m_pInterface->GetMaxChatWindow(i)->GetRoomKey() &&
					m_pMinChatWindow[nMinRoomIndex]->m_dwKey != INVALID_CHAT_KEY)
				{
					nMaxRoomIndex = i;
					break;
				}
			}

			if ( m_pMinChatWindow[ nMinRoomIndex ]->IsMouseIn()  )
			{
				//위에 for문에서 찾을 맥스챗물의 인덱스를 가지고 맥스챗룸의 방상태를 확인한다
				// 내가 방장일 아닐경우
				if ( m_pInterface->GetMaxChatWindow(nMaxRoomIndex)->GetRoomKey() != m_pGaeaClient->GetCharacter()->GetCharID() )
				{
					m_pInterface->GetMaxChatWindow(nMaxRoomIndex);
				}
				m_pInterface->GetMaxChatWindow(nMaxRoomIndex)->SetMaxChatWindoWTitel( 
					m_pMinChatWindow[ nMinRoomIndex ]->GetMinChatRoomName() );

				//미니창을 한번 클릭하면 맥스창이 열리고 한번더 클릭하면 닫힌다
				if ( m_pInterface->GetMaxChatWindow(nMaxRoomIndex)->IsVisible())
					m_pInterface->UiHideGroup(MAX_GROUP_CHAT_WINDOW_START + nMaxRoomIndex);
				else
					m_pInterface->UiShowGroupFocus(MAX_GROUP_CHAT_WINDOW_START + nMaxRoomIndex);
			}

		}

	}

}

//미니챗 표시 메시지가 왔을때 창이열려있을때 닫혀있을때 각각 다른 색으로 표시
void CChatGroupWindow::MinChatWindowStateUpdate()
{
	for ( DWORD i = 0; i < m_dwMaxRoomIndex; i++ )
	{
		for ( DWORD  j = 0; j < m_dwMaxRoomIndex; j++)
		{
			if ( m_pMinChatWindow[i]->m_dwKey == m_pInterface->GetMaxChatWindow(j)->GetRoomKey() &&
				m_pMinChatWindow[i]->m_dwKey != INVALID_CHAT_KEY )
			{
				if (m_pInterface->GetMaxChatWindow(j)->IsVisible())
				{
				//맥스 창이 열려있을때 밝은 파랑색으로 바뀜
					m_pInterface->GetMaxChatWindow(j)->SetNewMsgState(FALSE);
					m_pMinChatWindow[i]->SetRoomState(CMinChatWindow::ROOM_STATE_OPEN);
				}
				// 메시지가 왔을때 노란색으로 바뀜
				else if (m_pInterface->GetMaxChatWindow(j)->ISNewMsgState()) 
					m_pMinChatWindow[i]->SetRoomState(CMinChatWindow::ROOM_STATE_MSG);
				// 맥스 창이 닫혀있을때 파란색으로 바뀜
				else
					m_pMinChatWindow[i]->SetRoomState(CMinChatWindow::ROOM_STATE_NOMAL);
			}
		}
	}
}

void CChatGroupWindow::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{

    CUIWindowEx::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	MinChatWindowStateUpdate();

	//미니챗 창 UI의 상태 갱신을 위해 모든 창을 닫는다
	for ( DWORD j = 0; j < m_dwMaxRoomIndex; j++)
	{
		m_pMinChatWindow[j]->SetVisibleSingle(FALSE);
	}
	std::vector<CString> vRoomName;
	std::vector<DWORD> vRoomdwKey;

	//GetRoomName()를 호출하여 방 참여 자의 
	//이름과 DBNUM를 받아온다
	m_pGaeaClient->GetChatGroup()->GetRoomName(vRoomName,vRoomdwKey);

	for ( unsigned int j = 0; j < vRoomName.size(); j++)
	{
		// 대화방 방장 이름을 뿌려준다
		m_pMinChatWindow[j]->SetMinChatRoomName( vRoomName[j] );
		m_pMinChatWindow[j]->m_dwKey = vRoomdwKey[j];
		m_pMinChatWindow[j]->SetVisibleSingle(TRUE);
	}

      UIRECT rcOldpos = this->GetGlobalPos();

	  //미니 그룹챗창이 동적으로 생성 소멸하기 때문에 생성 소멸시
	  // 위치를 재설정 한다
      WORD wAlignFlag = GetAlignFlag ();
      SetAlignFlag ( UI_FLAG_XSIZE | UI_FLAG_YSIZE );
      {
          UIRECT rcNewPos(rcOldpos.left
						, rcOldpos.top
						, rcOldpos.sizeX
						, static_cast<float>(CHAT_ROOM_HEIGHT*static_cast<int>(vRoomName.size())));
          AlignSubControl( rcOldpos, rcNewPos );
          SetGlobalPos( rcNewPos );
      }
      SetAlignFlag( wAlignFlag );
}
