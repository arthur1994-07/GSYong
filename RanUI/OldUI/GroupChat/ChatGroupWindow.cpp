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
	//�̴�ä��â�� �θ�â�� ����� 
	//�θ�â�� ���� ���̴� �̴�â���� ���� ���̸� �� ���� ���̴�
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
	//�������� ���޹��� ���� ��ŭ �׷�ä�� �̴�â�� �����Ѵ�
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
	//dwRoomkey Ű ������ dwRoomkeyŰ ���� ������ ����
	// �ε����� ã�´�
	for ( DWORD i = 0; i < m_dwMaxRoomIndex; i++ )
	{
		if( m_pMinChatWindow[i]->m_dwKey == dwRoomkey )
		{
			return i;
		}			
	}	
	
	//�Լ��� ����ϴ� ������ �̵� �Ұ�
	/*GASSERT("dwRoomkeyŰ�� NULL �Դϴ�");*/
	return -1;
		
}

void CChatGroupWindow::TranslateUIMessage ( UIGUID cID, DWORD dwMsg )
{
	if ( CHECK_MOUSE_IN_LBDOWNLIKE ( dwMsg ) )
	{
		m_pInterface->UiShowGroupFocus(MIN_GROUP_CHAT_WINDOW);
	}

    if ( CHECK_MOUSE_IN ( dwMsg ) )		//	��Ʈ�� ���ο� ���콺�� ��ġ�ϰ�,
    {
        DWORD dwButtonMsg = UIMSG_NOMESSAGE;
        if ( m_pClose )	dwButtonMsg = m_pClose->GetMessageEx ();
        if ( (dwMsg & UIMSG_LB_DOWN) && !CHECK_MOUSE_IN ( dwButtonMsg ) )	//	���콺�� ��Ŭ������ ������ ��,
        {			
            SetExclusiveControl();	//	�ܵ� ��Ʈ�ѷ� ����ϰ�,
            if ( !m_bFirstGap )				//	���� ������ ���� ����Ѵ�.
            {
                UIRECT rcPos = GetGlobalPos ();
                m_vGap.x = m_PosX - rcPos.left;
                m_vGap.y = m_PosY - rcPos.top;
                m_bFirstGap = TRUE;
            }
        }
        else if ( CHECK_LB_UP_LIKE ( dwMsg ) )	//	��Ŭ���ߴ����� ������
        {
            ResetExclusiveControl();	//	�ܵ� ��Ʈ���� �����ϰ�
        }
    }
    else if ( CHECK_LB_UP_LIKE ( dwMsg ) )		//	��Ʈ�� �����ۿ��� ��ư�� ���� ��찡
    {											//	�߻��ϴ���
        ResetExclusiveControl();		//	�ܵ� ��Ʈ���� �����ϰ�,
    }


    CUIWindowEx::TranslateUIMessage ( cID, dwMsg );



	//�̴�ä��â ID - ET_CONTROL_NEXT == ���ȣ 
	if ( (cID - ET_CONTROL_NEXT) < static_cast<int>(m_dwMaxRoomIndex) )
	{
		if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
		{
			int nMinRoomIndex = cID - ET_CONTROL_NEXT;
			int nMaxRoomIndex;

			for ( DWORD i = 0; i < m_dwMaxRoomIndex; i++ )
			{
				//�̴� ê���� DBNUM���� ����� �ƽ�ê ���� ã�´�
				if ( m_pMinChatWindow[nMinRoomIndex]->m_dwKey == m_pInterface->GetMaxChatWindow(i)->GetRoomKey() &&
					m_pMinChatWindow[nMinRoomIndex]->m_dwKey != INVALID_CHAT_KEY)
				{
					nMaxRoomIndex = i;
					break;
				}
			}

			if ( m_pMinChatWindow[ nMinRoomIndex ]->IsMouseIn()  )
			{
				//���� for������ ã�� �ƽ�ê���� �ε����� ������ �ƽ�ê���� ����¸� Ȯ���Ѵ�
				// ���� ������ �ƴҰ��
				if ( m_pInterface->GetMaxChatWindow(nMaxRoomIndex)->GetRoomKey() != m_pGaeaClient->GetCharacter()->GetCharID() )
				{
					m_pInterface->GetMaxChatWindow(nMaxRoomIndex);
				}
				m_pInterface->GetMaxChatWindow(nMaxRoomIndex)->SetMaxChatWindoWTitel( 
					m_pMinChatWindow[ nMinRoomIndex ]->GetMinChatRoomName() );

				//�̴�â�� �ѹ� Ŭ���ϸ� �ƽ�â�� ������ �ѹ��� Ŭ���ϸ� ������
				if ( m_pInterface->GetMaxChatWindow(nMaxRoomIndex)->IsVisible())
					m_pInterface->UiHideGroup(MAX_GROUP_CHAT_WINDOW_START + nMaxRoomIndex);
				else
					m_pInterface->UiShowGroupFocus(MAX_GROUP_CHAT_WINDOW_START + nMaxRoomIndex);
			}

		}

	}

}

//�̴�ê ǥ�� �޽����� ������ â�̿��������� ���������� ���� �ٸ� ������ ǥ��
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
				//�ƽ� â�� ���������� ���� �Ķ������� �ٲ�
					m_pInterface->GetMaxChatWindow(j)->SetNewMsgState(FALSE);
					m_pMinChatWindow[i]->SetRoomState(CMinChatWindow::ROOM_STATE_OPEN);
				}
				// �޽����� ������ ��������� �ٲ�
				else if (m_pInterface->GetMaxChatWindow(j)->ISNewMsgState()) 
					m_pMinChatWindow[i]->SetRoomState(CMinChatWindow::ROOM_STATE_MSG);
				// �ƽ� â�� ���������� �Ķ������� �ٲ�
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

	//�̴�ê â UI�� ���� ������ ���� ��� â�� �ݴ´�
	for ( DWORD j = 0; j < m_dwMaxRoomIndex; j++)
	{
		m_pMinChatWindow[j]->SetVisibleSingle(FALSE);
	}
	std::vector<CString> vRoomName;
	std::vector<DWORD> vRoomdwKey;

	//GetRoomName()�� ȣ���Ͽ� �� ���� ���� 
	//�̸��� DBNUM�� �޾ƿ´�
	m_pGaeaClient->GetChatGroup()->GetRoomName(vRoomName,vRoomdwKey);

	for ( unsigned int j = 0; j < vRoomName.size(); j++)
	{
		// ��ȭ�� ���� �̸��� �ѷ��ش�
		m_pMinChatWindow[j]->SetMinChatRoomName( vRoomName[j] );
		m_pMinChatWindow[j]->m_dwKey = vRoomdwKey[j];
		m_pMinChatWindow[j]->SetVisibleSingle(TRUE);
	}

      UIRECT rcOldpos = this->GetGlobalPos();

	  //�̴� �׷�êâ�� �������� ���� �Ҹ��ϱ� ������ ���� �Ҹ��
	  // ��ġ�� �缳�� �Ѵ�
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
