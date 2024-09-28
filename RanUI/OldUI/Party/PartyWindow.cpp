#include "StdAfx.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Party/GLPartyClient.h"
#include "../../../RanLogicClient/Land/GLLandManClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"

#include "../Interface/MiniMap.h"
#include "../../InnerInterface.h"
#include "PartyWindow.h"
#include "PartySlot.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CPartyWindow::CPartyWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIWindowEx(pInterface, pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pConditionTextBox(NULL)
{
	for ( int i = 0; i < MAXPARTY; i++ )
	{
		m_pPartySlot[i] = NULL;
	}
}

CPartyWindow::~CPartyWindow()
{
}

void CPartyWindow::CreateSubControl()
{	
	int nTextAlign = TEXT_ALIGN_LEFT;
	D3DCOLOR dwFontColor = NS_UITEXTCOLOR::DEFAULT;
	CD3DFontPar* pFont = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	CBasicTextBox* pTextBox = NULL;

	{	//	기본 바닥 출력
		CreateControl( "PARTY_WINDOW_MASTER_STATIC_BACK" );
		pTextBox = CreateStaticControl ( "PARTY_WINDOW_MASTER_STATIC", pFont, dwFontColor, nTextAlign );
		pTextBox->SetOneLineText ( (char*)ID2GAMEWORD( "PARTY_WINDOW_MASTER_MEMBER_STATIC", 0 ) );

		CreateControl( "PARTY_WINDOW_MEMBER_STATIC_BACK" );
		pTextBox = CreateStaticControl( "PARTY_WINDOW_MEMBER_STATIC", pFont, dwFontColor, nTextAlign );
		pTextBox->SetOneLineText( (char*)ID2GAMEWORD ( "PARTY_WINDOW_MASTER_MEMBER_STATIC", 1 ) );
	}

	{
		pTextBox = CreateStaticControl( "PARTY_SLOT_CONDITION_TEXT", pFont, dwFontColor, nTextAlign );
		m_pConditionTextBox = pTextBox;
		//m_pConditionTextBox->AddText ( "- 라운드 로빈 방식 아이템 분배", NS_UITEXTCOLOR::PARTYNAME );
		//m_pConditionTextBox->AddText ( "- 먼저 묵는놈이 임자방식 돈 분배", NS_UITEXTCOLOR::PRIVATE );
	}

	CString strMemberKeyword[MAXPARTY] = 
	{
		"PARTY_SLOT_MEMBER0",
		"PARTY_SLOT_MEMBER1",
		"PARTY_SLOT_MEMBER2",
		"PARTY_SLOT_MEMBER3",
		"PARTY_SLOT_MEMBER4",
		"PARTY_SLOT_MEMBER5",
		"PARTY_SLOT_MEMBER6",
		"PARTY_SLOT_MEMBER7",
	};

	for ( int i = 0; i < MAXPARTY; i++ )
	{
		CPartySlot* pPartySlot = new CPartySlot(m_pEngineDevice);
		pPartySlot->CreateSub( this, strMemberKeyword[i].GetString (), UI_FLAG_DEFAULT, PARTY_SLOT_MEMBER0 + i );
		pPartySlot->CreateSubControl();
		RegisterControl( pPartySlot );
		m_pPartySlot[i] = pPartySlot;
	}

//	m_pPartySlot[3]->SetSameMap ( FALSE );
}

void CPartyWindow::SetSlotMember( CPartySlot* pPartySlot, GLPartySlotClient *pMember, BOOL bSameMap, BOOL bQuitEnable, BOOL bAuthEnable )
{
	if ( !pPartySlot )
	{
		GASSERT ( 0 && "파티 슬롯이 없습니다. - 메인" );
		return ;
	}
	if ( !pMember )
	{
		GASSERT ( 0 && "멤버가 없습니다. - 메인" );
		return ;
	}

	if ( bSameMap )	//	같은 맵인가?
	{
		int nPosX(0), nPosY(0);
		GLLandManClient* pLandClient = m_pGaeaClient->GetActiveMap();
		const GLMapAxisInfo &sMapAxisInfo = pLandClient->GetMapAxisInfo();
		sMapAxisInfo.Convert2MapPos( pMember->m_vPosition.x, pMember->m_vPosition.z, nPosX, nPosY );

		CString strPos;
		strPos.Format ( "%d/%d", nPosX, nPosY );

		pPartySlot->SetPlayerPos( strPos );

		float fHPPercent = 0.0f;
		if ( pMember->m_sHP.nMax )
			fHPPercent = float(pMember->m_sHP.nNow) / float(pMember->m_sHP.nMax);
		pPartySlot->SetHP( fHPPercent );
	}
	else
	{
		const CString strMapName = m_pGaeaClient->GetMapName( pMember->m_sMapID.getBaseMapID() );
		pPartySlot->SetMapName( strMapName );
	}
	pPartySlot->SetSameMap( bSameMap );


	const int nClassType = CharClassToIndex( pMember->m_emClass );	
	const CString strName = pMember->m_szName;

	pPartySlot->SetClass( nClassType );
	pPartySlot->SetPlayerName( strName );		
	pPartySlot->SetVisibleQuitButton( bQuitEnable );	
	pPartySlot->SetVisibleAuthButton( bAuthEnable );

	pPartySlot->SetVisibleSingle( TRUE );
}

void CPartyWindow::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIWindowEx::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
		
	//	모두 보이지 않도록 리셋
	for ( int i = 0; i < MAXPARTY; i++ )
		m_pPartySlot[i]->SetVisibleSingle( FALSE );
	if ( m_pConditionTextBox )
		m_pConditionTextBox->SetVisibleSingle( FALSE );

	//	파티 리스트 갱신
	//		
	GLPartyClient* const pMyParty = m_pGaeaClient->GetMyPartyClient();
	if ( pMyParty->isValid() == false )
		return;

	GLPartySlotClient* const pMaster = pMyParty->GetMember(MASTER_PARTY);	//	마스터

	GLPartySlotClient *pSelf = FindSelfClient();
	//	NOTE
	//		만약, 자신을 찾지 못하면
	//		이 아래로의 작업은 아무것도 진행하지 않습니다.
	if ( !pSelf )
		return ;

	const BOOL bMaster = pMaster->isOneSelf(m_pGaeaClient);
	const DWORD nMEMBER_NUM = pMyParty->GetNMember();
	for ( DWORD _i = 0; _i < nMEMBER_NUM; ++_i )
	{
		GLPartySlotClient *pMember = pMyParty->GetMember(_i);
		if ( pMember )
		{				
			const BOOL bONESELF = pMember->isOneSelf(m_pGaeaClient);
			const BOOL bEnableQuitButton = (bMaster || bONESELF);

			//	마스터가 아님 '탈퇴'라는 메시지로 적용
			m_pPartySlot[_i + 1]->SetMaster( FALSE );

			SetSlotMember( m_pPartySlot[_i], pMember, IsSameMap( pSelf, pMember ), bEnableQuitButton, bMaster );
		}
	}

	if ( m_pConditionTextBox )
	{
		const SPartyOption& sPartyOption = pMyParty->GetOption();

		if ( m_sPartyOption != sPartyOption )
		{
			m_pConditionTextBox->ClearText();

			CString strCombine;
			CString strLeft, strRight;

			strLeft = ID2GAMEWORD("PARTY_MODAL_CONDITION",0);
			strRight = ID2GAMEWORD("PARTY_MODAL_ITEM_OPTION",sPartyOption.GetItemOption());
			strCombine.Format ( "%s : %s", strLeft, strRight );
			m_pConditionTextBox->AddText ( strCombine, NS_UITEXTCOLOR::PARTYNAME );

			strLeft = ID2GAMEWORD("PARTY_MODAL_CONDITION",1);
			strRight = ID2GAMEWORD("PARTY_MODAL_MONEY_OPTION",sPartyOption.GetMoneyOption());
			strCombine.Format( "%s : %s", strLeft, strRight );
			m_pConditionTextBox->AddText( strCombine, NS_UITEXTCOLOR::PRIVATE );				

			m_sPartyOption = sPartyOption;
		}

		m_pConditionTextBox->SetVisibleSingle( TRUE );
	}
}


BOOL CPartyWindow::IsSameMap( GLPartySlotClient *pSelf, GLPartySlotClient *pMember )
{
	if ( !pSelf || !pMember )
	{
		GASSERT ( 0 && "잘못된 데이타입니다." );
		return FALSE;
	}
	
	return pSelf->m_sMapID == pMember->m_sMapID;
}

GLPartySlotClient* const CPartyWindow::FindSelfClient(void)
{
	GLPartyClient* const pMyParty = m_pGaeaClient->GetMyPartyClient();
	if ( pMyParty->isValid() == false )
		return NULL;

	//	마스터가 아니면, 자식들을 찾아본다.
	GLPartySlotClient* const pMaster = pMyParty->GetMember(MASTER_PARTY);	//	마스터
	if ( pMaster->isOneSelf(m_pGaeaClient) )
		return pMaster;
	else
	{
		//	자기찾기
		for ( int _i = 0; _i < 7; _i++ )
		{
			GLPartySlotClient* const pMember = pMyParty->GetMember(_i);
			if ( pMember && pMember->isOneSelf(m_pGaeaClient))
			{
				return pMember;				
			}
		}
	}
//	GASSERT ( 0 && "자신을 찾을 수 없습니다." );
	return NULL;
}

void CPartyWindow::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	CUIWindowEx::TranslateUIMessage( ControlID, dwMsg );

	switch ( ControlID )
	{
	case ET_CONTROL_TITLE:
	case ET_CONTROL_TITLE_F:
		{
			if ( (dwMsg & UIMSG_LB_DUP) && CHECK_MOUSE_IN( dwMsg ) )
			{
				m_pInterface->SetDefaultPosInterface( PARTY_WINDOW );
			}
		}
		break;
	case PARTY_SLOT_MEMBER0:
		{
			//	NOTE
			//		파티 해체
			if ( dwMsg & UIMSG_MOUSEIN_LBUP_EVENT )
			{
				m_pGaeaClient->GetMyPartyClient()->Dissolve();
			}

			//	NOTE
			//		힐
			if ( dwMsg & UIMSG_MOUSEIN_RBUP_EVENT )
			{
				GLPartyClient* const pMyParty = m_pGaeaClient->GetMyPartyClient();
				if ( pMyParty->isValid() == true )
				{
					GLPartySlotClient* const pMaster = pMyParty->GetMember(MASTER_PARTY);	//	마스터
					STARGETID sTARID(CROW_PC, pMaster->m_dwGaeaID);
					m_pGaeaClient->GetCharacter ()->ReqSkillReaction(sTARID);
				}
			}
		}
		break;

	case PARTY_SLOT_MEMBER1:
	case PARTY_SLOT_MEMBER2:
	case PARTY_SLOT_MEMBER3:
	case PARTY_SLOT_MEMBER4:
	case PARTY_SLOT_MEMBER5:
	case PARTY_SLOT_MEMBER6:
	case PARTY_SLOT_MEMBER7:
		{
			//	NOTE
			//		강퇴
			if ( dwMsg & UIMSG_MOUSEIN_LBUP_EVENT )
			{
				int nIndex = ControlID - PARTY_SLOT_MEMBER1;
				m_pGaeaClient->GetMyPartyClient()->Secede( nIndex );
			}

			if( dwMsg & UIMSG_MOUSEIN_LBUP_AUTH )
			{
				int nIndex = ControlID - PARTY_SLOT_MEMBER1;
				m_pGaeaClient->GetMyPartyClient()->Authority( nIndex );
			}

			//	NOTE
			//		힐
			if ( dwMsg & UIMSG_MOUSEIN_RBUP_EVENT )
			{
				int nIndex = ControlID - PARTY_SLOT_MEMBER1;

				GLPartyClient* const pMyParty = m_pGaeaClient->GetMyPartyClient();
				if ( pMyParty->isValid() == true )
				{					
					GLPartySlotClient* const pMember = pMyParty->GetMember(nIndex);
					if ( pMember )
					{
						STARGETID sTARID(CROW_PC,pMember->m_dwGaeaID);
						m_pGaeaClient->GetCharacter ()->ReqSkillReaction(sTARID);
					}
				}
			}
		}
		break;
	}
}

const DWORD CPartyWindow::GetPartyID(void) const
{
	return m_dwPartyID;
}
const DWORD CPartyWindow::GetHashKey(void) const
{
	return m_dwHashKey;
}
const DWORD CPartyWindow::GetMasterID(void) const
{
	return m_dwMasterID;
}
const DWORD CPartyWindow::GetTimeKey(void) const
{
	return m_dwTimeKey;
}

void CPartyWindow::SetPartyInfo(const DWORD dwPartyID, const DWORD dwHashKey, const DWORD dwMasterID, const DWORD dwTimeKey)
{
	m_dwPartyID = dwPartyID;
	m_dwHashKey = dwHashKey;
	m_dwMasterID = dwMasterID;
	m_dwTimeKey = dwTimeKey;
}




MyPartyWindow::MyPartyWindow( GLGaeaClient* pGaeaCleint, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
    : CPartyWindow( pGaeaCleint, pInterface, pEngineDevice )
{
}

void MyPartyWindow::CreateUIWindowAndRegisterOwnership()
{
    CPartyWindow::Create( PARTY_WINDOW, "BASIC_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
    CPartyWindow::CreateBaseWindowBlack( "PARTY_WINDOW", (char*)ID2GAMEWORD("PARTY_NAME_STATIC") );
    CPartyWindow::CreateSubControl();
    CPartyWindow::SetAlignFlag( UI_FLAG_CENTER_Y | UI_FLAG_RIGHT );
    CPartyWindow::m_pInterface->UiRegisterControl( this, true );
    CPartyWindow::m_pInterface->UiShowGroupFocus( PARTY_WINDOW );
}

const DWORD MyPartyWindow::GetPartyID(void) const
{
	return CPartyWindow::GetPartyID();
}
const DWORD MyPartyWindow::GetHashKey(void) const
{
	return CPartyWindow::GetHashKey();
}
const DWORD MyPartyWindow::GetMasterID(void) const
{
	return CPartyWindow::GetMasterID();
}
const DWORD MyPartyWindow::GetTimeKey(void) const
{
	return CPartyWindow::GetTimeKey();
}

void MyPartyWindow::SetPartyInfo(const DWORD dwPartyID, const DWORD dwHashKey, const DWORD dwMasterID, const DWORD dwTimeKey)
{
    CPartyWindow::SetPartyInfo( dwPartyID, dwHashKey, dwMasterID, dwTimeKey );
}

