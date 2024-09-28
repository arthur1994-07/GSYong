#include "StdAfx.h"
#include "ClubMember.h"

#include "../../../SigmaCore/String/StringFormat.h"

#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/DxTools/DxClubMan.h"
#include "../../../EngineLib/GUInterface/BasicTextBoxEx.h"
#include "../../../EngineLib/GUInterface/BasicScrollBarEx.h"
#include "../../../EngineLib/GUInterface/BasicScrollThumbFrame.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/UIEditBox.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/UIKeyCheck.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Party/GLPartyClient.h"
#include "../../../RanLogicClient/Stage/DxGlobalStage.h"
#include "../../../RanLogicClient/Land/GLLandManClient.h"
#include "../../../RanLogicClient/Club/GLClubClient.h"

#include "../../InnerInterface.h"
#include "../Interface/BasicChat.h"
#include "../Interface/ChatType.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CClubMember::CClubMember(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
	: CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pInterface(pInterface)
	, m_nSelectIndex(-1)
	, m_pListText(NULL)
	, m_pListScrollBar(NULL)
{
}

CClubMember::~CClubMember ()
{
}

void CClubMember::CreateSubControl ()
{
	CD3DFontPar* pFont = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	{	//	뒷 배경
		CBasicLineBox* pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
		pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_QUEST_LIST", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxQuestList ( "CLUB_LIST_LINEBOX" );
		RegisterControl ( pBasicLineBox );		
	}

	//	텍스트 박스
	{
		CBasicTextBoxEx* pTextBox = new CBasicTextBoxEx(m_pEngineDevice);
		pTextBox->CreateSub ( this, "CLUB_LIST_TEXTBOX", UI_FLAG_DEFAULT, CLUB_LIST_TEXTBOX );
		pTextBox->SetFont ( pFont );
		pTextBox->SetLineInterval ( 3.0f );
		pTextBox->SetSensitive ( true );
		pTextBox->SetLimitLine ( 10000 );
		RegisterControl ( pTextBox );
		m_pListText = pTextBox;

		int nTotalLine = pTextBox->GetVisibleLine ();

		//	스크롤바
		CBasicScrollBarEx* pScrollBar = new CBasicScrollBarEx(m_pEngineDevice);
		pScrollBar->CreateSub ( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE, CLUB_LIST_SCROLLBAR );
		pScrollBar->CreateBaseScrollBar ( "CLUB_LIST_SCROLLBAR" );
		pScrollBar->GetThumbFrame()->SetState ( 1, nTotalLine );
		RegisterControl ( pScrollBar );
		m_pListScrollBar = pScrollBar;
	}
}

void CClubMember::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	m_pListText->SetUseOverColor ( FALSE );
	if ( IsSelectedIndex() )
		m_pListText->SetUseTextColor ( m_nSelectIndex, FALSE );

	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	if ( IsSelectedIndex() )
	{
		m_pListText->SetUseTextColor ( m_nSelectIndex, TRUE );
		m_pListText->SetTextColor ( m_nSelectIndex, NS_UITEXTCOLOR::ORANGE );
	}

	{
		CBasicScrollThumbFrame* const pThumbFrame = m_pListScrollBar->GetThumbFrame ();

		const int nTotalLine = m_pListText->GetTotalLine ();
		const int nLinePerOneView = m_pListText->GetVisibleLine ();
		CDebugSet::ToView ( 1, 10, "보이는 라인 %d", nLinePerOneView );
		pThumbFrame->SetState ( nTotalLine, nLinePerOneView );
		if ( nLinePerOneView < nTotalLine )
		{
			const int nMovableLine = nTotalLine - nLinePerOneView;
			float fPercent = pThumbFrame->GetPercent ();
			int nPos = (int)floor(fPercent * nMovableLine);
			m_pListText->SetCurLine ( nPos );			
		}
	}
}

void CClubMember::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage( ControlID, dwMsg );

	switch ( ControlID )
	{
	case CLUB_LIST_TEXTBOX:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				const int nIndex = m_pListText->GetSelectPos ();
				if ( nIndex < 0 || m_pListText->GetCount () <= nIndex )
                    return;

				m_pListText->SetUseOverColor ( TRUE );
				m_pListText->SetOverColor ( nIndex, NS_UITEXTCOLOR::DARKORANGE );

				if ( UIMSG_LB_UP & dwMsg )
				{
					m_nSelectIndex = nIndex;
				
					const DWORD MemberDbNum = m_pListText->GetTextData ( m_nSelectIndex );

					GLClubClient* pMyClub = m_pGaeaClient->GetMyClub();
					const CLUBMEMBERS& mapMembers = pMyClub->GetMembers();
					//CLUBMEMBERS_ITER iter = mapMembers.begin();
					//CLUBMEMBERS_ITER iter_end = mapMembers.end();

                    CLUBMEMBERS_CITER iter = mapMembers.find(MemberDbNum);
                    if (iter != mapMembers.end())
                    {
                        m_sClubMember = iter->second;
                        m_strSelectName = m_sClubMember.GetName();
                        break;
                    }

                    /*
					for ( ; iter != iter_end; ++iter )
					{
						m_sClubMember = (*iter).second;
						if ( m_sClubMember.dwID == MemberDbNum )
						{
							m_strSelectName = m_sClubMember.m_szName;
							break;
						}
					}
                    */
				}

				if( ( UIMSG_LB_DUP & dwMsg ) && m_sClubMember.m_Online )
				{
					//m_pInterface->PostUIMessage( GetParent()->GetWndID(), UIMSG_ADDNAME_TO_CHATBOX_FROM_CLUBWINDOW );
					GetParent()->AddMessageEx( UIMSG_ADDNAME_TO_CHATBOX_FROM_CLUBWINDOW );
				}
			}
		}
		break;
	}
}

BOOL CClubMember::IsSelectedIndex()
{
	return ( 0 <= m_nSelectIndex && m_nSelectIndex < m_pListText->GetCount() );
}

const DWORD CClubMember::GetClubMemberID()
{
	if ( !IsSelectedIndex() )
		return NATIVEID_NULL().dwID;

	return m_pListText->GetTextData ( m_nSelectIndex );
}

void CClubMember::LoadClubMemberList()
{
	m_pListText->ClearText (); // 텍스트를 모두 지운다.

	GLClubClient* pMyClub = m_pGaeaClient->GetMyClub();
	if (!pMyClub->IsValid())
        return;

    std::vector<club::GLCLUBMEMBER_CLIENT> ClubMembers;
    std::vector<club::ALIGN_ORDER> MemberAlign;
    
    club::ALIGN_ORDER Align(club::ORDER_CHA_NAME, club::SQL_ASC);
    MemberAlign.push_back(Align);

    m_pGaeaClient->GetClubMember(ClubMembers, MemberAlign, true);
    m_pGaeaClient->GetClubMember(ClubMembers, MemberAlign, false);

    //CLUBMEMBERS& mapMembers = pMyClub->GetMembers();
	//CLUBMEMBERS_ITER iter = mapMembers.begin();
	//CLUBMEMBERS_ITER iter_end = mapMembers.end();
	//for ( ; iter != iter_end; ++iter)
    for (size_t i=0; i<ClubMembers.size(); ++i)
	{
		//GLCLUBMEMBER& sClubMember = (*iter).second;
        club::GLCLUBMEMBER_CLIENT& sClubMember = ClubMembers[i];
		const bool bOnline = sClubMember.m_Online;
		const int nChannel = sClubMember.m_Channel;
		const DWORD dwID = sClubMember.m_ChaDbNum;

        CString strCombine;
        CString strName(sClubMember.m_ChaName.c_str());
        CString strPos;
        std::string strBlank;
        bool bCDM = false;

		if (bOnline)
		{
			//GLLandManClient* pLandClient = m_pGaeaClient->GetActiveMap();
			//if (pLandClient)
			//{
				// 캐릭터 명의 최대 길이, 8글자 + 여백 2. 18 - 3 여기서 3은 [M]의 길이
				// 하지만 나중에 [M], 다른 글자나 이미지로 바뀌면 다시 수정해야함.
				//const int nMAX_WIDTH = 18;
				//const bool bSameMap = (pLandClient->GetMapID () == sClubMember.nidMAP);
				//if (bSameMap)
				//{	
					//int nLength = nMAX_WIDTH - strName.GetLength();
					//if (nLength < 2)
                    //    nLength = 2;
					//std::string strBlank(nLength , ' ');

					int nPosX = 0;
					int nPosY = 0;
					//const GLMapAxisInfo& sMapAxisInfo = pLandClient->GetMapAxisInfo();
					//sMapAxisInfo.Convert2MapPos(sClubMember.vecPOS.x, sClubMember.vecPOS.z, nPosX, nPosY);
                    
                    m_pGaeaClient->ConvertWorldToMap(sClubMember.m_CurrentMap, sClubMember.m_CurrentPos, nPosX, nPosY);
					strPos.Format(" %s %02d/%02d", sClubMember.GetMapName().c_str(), nPosX, nPosY);                    
				//}
				//else
				//{					
					//std::string strBlank(nMAX_WIDTH - strName.GetLength(), ' '); // MEMO
					//strPos = m_pGaeaClient->GetMapName(sClubMember.nidMAP);
                //    strPos = sClubMember.GetMapName().c_str();
				//}
                bCDM = pMyClub->IsMemberFlgCDM(dwID);
			//}
		}
        else
        {
            strPos = sClubMember.GetMapName().c_str();
        }

        strBlank = sc::string::format(" %1%Lv %2% ", sClubMember.m_ChaLevel, sClubMember.GetGradeName());

        if (m_pInterface->IsCHANNEL())
            strCombine.Format( "[%d]", nChannel ); // 채널이 추가된다.

        if (pMyClub->IsMaster(dwID))
            strCombine.AppendFormat( "[M]%s%s%s", strName, strBlank.c_str(), strPos );
        else if (pMyClub->IsSubMaster(dwID))
            strCombine.AppendFormat( "[S]%s%s%s", strName, strBlank.c_str(), strPos );
        else // 일반
            strCombine.AppendFormat( "[C]%s%s%s", strName, strBlank.c_str(), strPos );


        LoadClubMember(strCombine, bOnline, dwID, bCDM);
	}

    /*
	iter = mapMembers.begin ();
	// MEMO : 온라인상태와 오프라인 상태를 구분해서 출력하기 위해 for문을 한번 더 돈다.
	for ( ; iter != iter_end; ++iter )
	{
		GLCLUBMEMBER& sClubMember = (*iter).second;
		bool bOnline = sClubMember.bONLINE;
		if ( !bOnline ) LoadClubMember ( sClubMember.m_szName, bOnline, sClubMember.dwID, false );
	}
    */

	m_pListText->SetCurLine ( 0 );

	const float fPercent = m_pListScrollBar->GetThumbFrame()->GetPercent ();

	const int nTotal = m_pListText->GetCount ();
	const int nViewPerPage = m_pListText->GetVisibleLine ();	
	m_pListScrollBar->GetThumbFrame()->SetState ( nTotal, nViewPerPage );
	m_pListScrollBar->GetThumbFrame()->SetPercent ( fPercent );
}

void CClubMember::LoadClubMember( const CString& strText, const bool bOnline, const DWORD& dwGaeaID, const bool bCDM )
{
	enum
	{
		ONLINE = 0,
		OFFLINE
	};

	static	CString strKeyword[] =
	{
		"FRIEND_ONLINE",
		"FRIEND_OFFLINE"
	};

	D3DCOLOR dwTextColor = NS_UITEXTCOLOR::WHITE;
	int nIcon;
	if ( bOnline ) 
	{
		nIcon = ONLINE;
		if ( bCDM ) dwTextColor = NS_UITEXTCOLOR::PALEGREEN;
		else dwTextColor = NS_UITEXTCOLOR::WHITE;
	}
	else 
	{
		nIcon = OFFLINE;
		dwTextColor = NS_UITEXTCOLOR::DARKGRAY;
	}

	int nIndex = m_pListText->AddText ( strText, dwTextColor );	
	m_pListText->SetTextImage ( nIndex, strKeyword[nIcon] );
	m_pListText->SetTextData ( nIndex, dwGaeaID );
}

void CClubMember::SetVisibleSingle(BOOL bVisible)
{
	CUIGroup::SetVisibleSingle(bVisible);
	if (bVisible)
		LoadClubMemberList();
}

void CClubMember::PartyRecruit()
{
	if (m_nSelectIndex < 0 || m_pListText->GetCount () <= m_nSelectIndex)
        return;

	const DWORD dwCharID = m_pListText->GetTextData ( m_nSelectIndex );

	GLClubClient* pMyClub = m_pGaeaClient->GetMyClub();
	CLUBMEMBERS& mapMembers = pMyClub->GetMembers();
	CLUBMEMBERS_ITER iter = mapMembers.find ( dwCharID );
	CLUBMEMBERS_ITER iter_end = mapMembers.end ();

	if (iter == iter_end)
        return;

	m_sClubMember = (*iter).second;
	m_strSelectName = m_sClubMember.m_ChaName.c_str();

	if (!m_sClubMember.m_Online)
        return;

	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
	if (!pCharacter)
        return;
	if (dwCharID == pCharacter->m_CharDbNum)
        return;

    DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
	if (m_sClubMember.m_Channel != pGlobalStage->GetChannel())
        return;
	
		
	//	파티장이 아니면 지원하지 않는다.
	if ( m_pGaeaClient->GetMyPartyClient()->isMaster(pCharacter->GetGaeaID()) == false )
		return;	

	VEC_LINK_DATA_BASIC vecLinkDataBasic;

	CString strChatMsg;
    std::string strTemp = sc::string::format(
        "%s%d%d%s",
        strLINK_LEFT_SIMBOL,
        PARTY_LINK, PARTY_LINK_RECRUIT,
        strLINK_RIGHT_SIMBOL);

	SLINK_DATA_BASIC sLinkDataBasic;

	sLinkDataBasic.sLinkType.wMType = PARTY_LINK;
	sLinkDataBasic.sLinkType.wSType = PARTY_LINK_RECRUIT;
	
	strChatMsg += strTemp.c_str();
	
	strTemp = sc::string::format(ID2GAMEWORD("CHAT_LINK_PARTY_RECRUIT", 5), m_strSelectName).c_str();
	strChatMsg += strTemp.c_str();

	vecLinkDataBasic.push_back(sLinkDataBasic);

	pCharacter->ReqSendChatMsg ( CHAT_CONST::CHAT_PRIVATE, m_strSelectName.GetString(), strChatMsg.GetString(), vecLinkDataBasic );

}
