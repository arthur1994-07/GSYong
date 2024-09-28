#include "StdAfx.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Party/GLPartyClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"

#include "../../InnerInterface.h"
#include "MiniPartyWindow.h"
#include "MiniPartySlot.h"
#include "PartyBuffInfo.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CMiniPartyWindow::CMiniPartyWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
	: CUIWindowEx(pInterface, pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
	, m_pPartyBuffInfo(NULL)
{
	memset( m_pPartySlot, 0, sizeof(m_pPartySlot) );
	memset( m_pSlotOver, 0, sizeof(m_pSlotOver) );
}

CMiniPartyWindow::~CMiniPartyWindow ()
{
}

void CMiniPartyWindow::CreateSubControl ()
{	
	int nTextAlign = TEXT_ALIGN_LEFT;
	D3DCOLOR dwFontColor = NS_UITEXTCOLOR::DEFAULT;

	CBasicTextBox* pTextBox = NULL;

	CString strMemberKeyword[MAXPARTY] = 
	{
		"MINIPARTY_SLOT_MEMBER0",
		"MINIPARTY_SLOT_MEMBER1",
		"MINIPARTY_SLOT_MEMBER2",
		"MINIPARTY_SLOT_MEMBER3",
		"MINIPARTY_SLOT_MEMBER4",
		"MINIPARTY_SLOT_MEMBER5",
		"MINIPARTY_SLOT_MEMBER6",
		"MINIPARTY_SLOT_MEMBER7",
	};

	for ( int i = 0; i < MAXPARTY; i++ )
	{
		CMiniPartySlot* pPartySlot = new CMiniPartySlot(m_pEngineDevice);
		pPartySlot->CreateSub ( this, strMemberKeyword[i].GetString (), UI_FLAG_DEFAULT, MINIPARTY_SLOT_MEMBER0 + i );
		pPartySlot->CreateSubControl ();
		RegisterControl ( pPartySlot );
		m_pPartySlot[i] = pPartySlot;
	}

	CString strMemberOverKeyword[MAXPARTY] =
	{
		"MINIPARTY_SLOT_OVER0",
		"MINIPARTY_SLOT_OVER1",
		"MINIPARTY_SLOT_OVER2",
		"MINIPARTY_SLOT_OVER3",
		"MINIPARTY_SLOT_OVER4",
		"MINIPARTY_SLOT_OVER5",
		"MINIPARTY_SLOT_OVER6",
		"MINIPARTY_SLOT_OVER7",
	};

	for ( int i = 0; i < MAXPARTY; i++ )
	{
		CUIControl* pSlotOver = new CUIControl(m_pEngineDevice);
		pSlotOver->CreateSub ( this, strMemberOverKeyword[i].GetString () );
		RegisterControl ( pSlotOver );
		m_pSlotOver[i] = pSlotOver;
		pSlotOver->SetVisibleSingle ( FALSE );
	}

	//	NOTE
	//		버프 남은 시간 표시
	{
		CPartyBuffInfo* pPartyBuffInfo = new CPartyBuffInfo(m_pGaeaClient, m_pInterface, m_pEngineDevice);
		pPartyBuffInfo->CreateSub ( this, "MINIPARTY_BUFF_INFO", UI_FLAG_DEFAULT, MINIPARTY_BUFF_INFO );
		pPartyBuffInfo->CreateSubControl ();
		RegisterControl ( pPartyBuffInfo );
		m_pPartyBuffInfo = pPartyBuffInfo;
	}
}

void CMiniPartyWindow::SetSlotMember ( CMiniPartySlot* pPartySlot, GLPartySlotClient *pMember, BOOL bSameMap, const int nPartySlotID )
{
	if ( !pPartySlot || !pMember )
	{
		GASSERT ( 0 && "CMiniPartyWindow::SetSlotMember()" );
		return ;
	}

	//const int nClassType = CharClassToIndex ( pMember->m_emClass );
	//pPartySlot->SetClass ( nClassType );

	pPartySlot->SetSameMap( bSameMap );
	pPartySlot->SetNumber( nPartySlotID ); // MEMO : 슬롯아이디(M,1,2...) 바뀜
	pPartySlot->SetPlayerName ( pMember->m_szName );	
	pPartySlot->SetVisibleSingle ( TRUE );

	if( bSameMap )
	{
		float fHPPercent = 0.0f;
		if( pMember->m_sHP.nMax != 0 )
		{
			fHPPercent = float(pMember->m_sHP.nNow) / float(pMember->m_sHP.nMax);
		}

		pPartySlot->SetHP( fHPPercent );
	}
}

void CMiniPartyWindow::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if( !IsVisible() )
		return ;

	const UIRECT& rcGlobalPos = GetGlobalPos ();
	m_pPartyBuffInfo->SetLocalPos ( D3DXVECTOR2 ( x + 25 - rcGlobalPos.left, y + 25 - rcGlobalPos.top ) );
	
	m_bMOUSEIN = false; // 마우스가 어떤 슬롯위에 있을때 true가 됨

	for ( int i = 0; i < MAXPARTY; i++ )
	{
		m_pSlotOver[i]->SetVisibleSingle ( FALSE );
	}

	m_pPartyBuffInfo->SetVisibleSingle ( FALSE );

	CUIWindowEx::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	for ( int i = 0; i < MAXPARTY; i++ ) 
	{
		m_pPartySlot[i]->SetVisibleSingle ( FALSE );
	}

	//	파티 리스트 갱신
	//	
	GLPartyClient* const pMyParty = m_pGaeaClient->GetMyPartyClient();	
	if ( pMyParty->isValid() == false )
		return;
	
	GLPartySlotClient* const pSelf = FindSelfClient();
	//	NOTE
	//		만약, 자신을 찾지 못하면
	//		이 아래로의 작업은 아무것도 진행하지 않습니다.
	if ( !pSelf )
		return ;
	const DWORD nMEMBER_NUM = pMyParty->GetNMember();
	for ( DWORD _i = 0; _i < nMEMBER_NUM; ++_i )
	{
		GLPartySlotClient* const pMember = pMyParty->GetMember(_i);
		if ( pMember )
		{				
			const BOOL bONESELF = pMember->isOneSelf(m_pGaeaClient);

			SetSlotMember ( m_pPartySlot[_i], pMember, IsSameMap ( pSelf, pMember ), _i);
		}
	}
}


BOOL CMiniPartyWindow::IsSameMap ( GLPartySlotClient *pSelf, GLPartySlotClient *pMember )
{
	if ( !pSelf || !pMember )
	{
		GASSERT ( 0 && "잘못된 데이타입니다." );
		return FALSE;
	}

	return pSelf->m_sMapID == pMember->m_sMapID;
}

GLPartySlotClient* const CMiniPartyWindow::FindSelfClient(void)
{
	GLPartyClient* const pMyParty = m_pGaeaClient->GetMyPartyClient();
	//	마스터가 아니면, 자식들을 찾아본다.
	if ( pMyParty->isValid() == false )
		return NULL;

	GLPartySlotClient *pMaster = pMyParty->GetMember(MASTER_PARTY);	//	마스터
	if ( pMaster->isOneSelf(m_pGaeaClient) )
		return pMaster;
	else
	{
		//	자기찾기
		DWORD nMEMBER_NUM = pMyParty->GetNMember();
		if( nMEMBER_NUM > 1 )
		{
			nMEMBER_NUM -= 1; // Note : 마스터는 제외
			for ( DWORD i = 0; i < nMEMBER_NUM; i++ )
			{
				GLPartySlotClient* const pMember = pMyParty->GetMember(i);
				if ( pMember && pMember->isOneSelf(m_pGaeaClient) )
				{
					return pMember;				
				}
			}
		}
	}

	return NULL;
}

void CMiniPartyWindow::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	switch ( ControlID )
	{
	case ET_CONTROL_TITLE:
	case ET_CONTROL_TITLE_F:
		{
			if ( (dwMsg & UIMSG_LB_DUP) && CHECK_MOUSE_IN ( dwMsg ) )
			{
				m_pInterface->SetDefaultPosInterface( MINIPARTY_WINDOW );
			}
		}
		break;
	case MINIPARTY_SLOT_MEMBER0:
	case MINIPARTY_SLOT_MEMBER1:
	case MINIPARTY_SLOT_MEMBER2:
	case MINIPARTY_SLOT_MEMBER3:
	case MINIPARTY_SLOT_MEMBER4:
	case MINIPARTY_SLOT_MEMBER5:
	case MINIPARTY_SLOT_MEMBER6:
	case MINIPARTY_SLOT_MEMBER7:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				m_bMOUSEIN = true;
				int nIndex = ControlID - MINIPARTY_SLOT_MEMBER0;
				m_pPartyBuffInfo->SetPartyID ( nIndex );
				m_pPartyBuffInfo->SetVisibleSingle ( TRUE );
				m_pSlotOver[nIndex]->SetVisibleSingle ( TRUE );

				if ( dwMsg & UIMSG_MOUSEIN_RB_EVENT )
				{
					int nIndex = ControlID - MINIPARTY_SLOT_MEMBER1;
					//	힐
					GLPartyClient* const pMyParty = m_pGaeaClient->GetMyPartyClient();
					if ( pMyParty->isValid() == true )
					{
						GLPartySlotClient* const pMember = pMyParty->GetMember( nIndex );
						if ( pMember )
						{
							STARGETID sTARID(CROW_PC, pMember->m_dwGaeaID);
							m_pGaeaClient->GetCharacter ()->ReqSkillReaction(sTARID);
						}
					}
				}
			}
		}
		break;

	case ET_CONTROL_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				m_pInterface->UiHideGroup ( GetWndID () );
				m_pInterface->UiShowGroupBottom ( MINIPARTY_OPEN );

//				#ifdef CH_PARAM // 중국 인터페이스 변경
//					m_pInterface->UpdatePotionTrayPosition();
//				#endif
			}
		}
		break;
	}

	CUIWindowEx::TranslateUIMessage ( ControlID, dwMsg );
}