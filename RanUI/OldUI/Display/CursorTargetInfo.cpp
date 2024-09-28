#include "StdAfx.h"
#include "./CursorTargetInfo.h"

#include "../../../EngineLib/DxEffect/Single/DxEffGameHelp.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/InterfaceCfg.h"

#include "../../../RanLogic/NpcTalk/NpcDialogue.h"
#include "../../../RanLogic/NpcTalk/NpcDialogueCase.h"
#include "../../../RanLogic/NpcTalk/NpcTalk.h"
#include "../../../RanLogic/NpcTalk/NpcTalkCondition.h"
#include "../../../RanLogic/NpcTalk/NpcTalkMan.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CCursorTargetInfo::CCursorTargetInfo( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
    : CBasicVarTextBox( pEngineDevice )
    , m_pGaeaClient( pGaeaClient )
    , m_pInterface( pInterface )
	, m_pChar( NULL )
	, m_pActor( NULL )
{
}

void CCursorTargetInfo::RightBottomGap()
{
	D3DXVECTOR2 vGap( -10.0f, -70.0f );

	INTERFACE_CFG sCfg;
	if( CInterfaceCfg::GetInstance().ArrangeInfo( "CURSOR_TARGET_INFO", sCfg ) ) 
	{
		vGap.x = -sCfg.rcControlPos.left;
		vGap.y = -sCfg.rcControlPos.top;
	}

	SetMousePointGap( vGap );
}

void CCursorTargetInfo::UpdateInfo()
{
	m_pInterface->UiHideGroup( GetWndID() );

	DxEffGameHelp::GetInstance().BlindEff( EMOVER_EFFECT );

	m_pChar = m_pGaeaClient->GetCharacter();
	if( !m_pChar )
		return;

	STARGETID& sTargetID = m_pChar->GetCursorID();
	if( sTargetID.emCrow != CROW_PC &&
		sTargetID.emCrow != CROW_NPC &&
		sTargetID.emCrow != CROW_MOB )
	{
		return;
	}

	m_pActor = m_pGaeaClient->GetCopyActor( sTargetID );
	if( !m_pActor )
		return;

	ClearText();

	switch( m_pActor->GetCrow() )
	{
	case CROW_PC:
		UpdateInfoPc();
		break;

	case CROW_NPC:
		UpdateInfoNpc();
		break;

	case CROW_MOB:
		UpdateInfoMob();
		break;
	}

	if( GetCount() < 1 )
		return;

	ResetPosition();

	m_pInterface->UiShowGroupBottom( GetWndID() );

	const D3DXVECTOR3& vPos = m_pActor->GetPosition();
	D3DXMATRIX matTrans;
	D3DXMatrixTranslation( &matTrans, vPos.x, vPos.y, vPos.z );
	DxEffGameHelp::GetInstance().UpdateGameHelpEffect( EMOVER_EFFECT, matTrans, sTargetID );
}

void CCursorTargetInfo::ResetPosition()
{
	LONG lResolution = CUIMan::GetResolution();
	INT nPosX = HIWORD( lResolution );
	INT nPosY = LOWORD( lResolution );

	const UIRECT& rcSize = CalcSize();
	nPosX -= static_cast< INT >( rcSize.sizeX );
	nPosY -= static_cast< INT >( rcSize.sizeY );

	RePosControl( nPosX, nPosY );
}

void CCursorTargetInfo::UpdateInfoPc()
{
	GLCharClient* pTarget = dynamic_cast< GLCharClient* >( m_pActor );
	if( !pTarget )
		return;

	if( m_pChar->GetCharData().m_wSchool == pTarget->GetCharSchool() )
	{
		AddNamePublic( NS_UITEXTCOLOR::PALEGREEN );
		AddSchool( NS_UITEXTCOLOR::WHITE );
		AddClass( NS_UITEXTCOLOR::WHITE );
		AddParty( D3DCOLOR_XRGB( 0, 180, 255 ) );
	}
	else
	{
		if( m_pChar->GetClubID() == pTarget->GetClubID() )
		{
			AddNamePublic( D3DCOLOR_XRGB( 0, 180, 255 ) );
			AddClass( NS_UITEXTCOLOR::WHITE );
			AddClub( NS_UITEXTCOLOR::PALEGREEN );
		}
		else if( m_pChar->GetPartyID() == pTarget->GetPartyID() )
		{
			AddNamePublic( D3DCOLOR_XRGB( 0, 180, 255 ) );
			AddClass( NS_UITEXTCOLOR::WHITE );
		}
		else
		{
			AddNameCondition( NS_UITEXTCOLOR::WHITE, NS_UITEXTCOLOR::RED );
			AddSchool( NS_UITEXTCOLOR::RED );
			AddClass( NS_UITEXTCOLOR::WHITE );

			if( !GLCONST_CHAR ::bSCHOOL_FREE_PK_ALWAYS &&
				!m_pGaeaClient->IsSchoolFreePk() )
			{
				AddTextNoSplit(
					ID2GAMEINTEXT( "CURSOR_TARGET_NONFREEPK" ),
					NS_UITEXTCOLOR::WHITE );
			}
		}
	}

	AddShiftClick( NS_UITEXTCOLOR::YELLOW );
}

void CCursorTargetInfo::UpdateInfoNpc()
{
	GLCrowClient* pTarget = dynamic_cast< GLCrowClient* >( m_pActor );
	if( !pTarget || pTarget->IsHideTooltip())
		return;

	const SCROWDATA* pData = pTarget->m_pCrowData;
	if( !pData )
		return;

	CNpcTalkControl::NPCMAP* pTalkMap = m_pChar->FindDefaultTalk( pData->GetTalkFile() );
	if( !pTalkMap )
		return;

	AddNamePrivate( NS_UITEXTCOLOR::WHITE );

	AddInfoNpcBasic( pTalkMap );
	AddInfoNpcQuest( pTalkMap );
}

void CCursorTargetInfo::AddInfoNpcBasic( CNpcTalkControl::NPCMAP* pTalkMap )
{
	const DWORD nPartyMember = m_pGaeaClient->GetMyPartyClient()->GetNMember();

	VEC_DWORD vecTalk;

	CTime cCurSvrTime = m_pGaeaClient->GetCurrentTime();
	BOOST_FOREACH( CNpcTalkControl::NPCMAP_VALUE& it, *pTalkMap )
	{
		std::tr1::shared_ptr< SNpcTalk > spTalk = it.second;
		if( !spTalk )
			continue;

		if( spTalk->m_nACTION != SNpcTalk::EM_BASIC )
			continue;

		if( spTalk->m_dwACTION_NO >= SNpcTalk::EM_BASIC_SIZE ||
			spTalk->m_dwACTION_NO == SNpcTalk::EM_MARKET ||
			spTalk->m_dwACTION_NO == SNpcTalk::EM_CD_CERTIFY ||
			spTalk->m_dwACTION_NO == SNpcTalk::EM_ODDEVEN ||
			spTalk->m_dwACTION_NO == SNpcTalk::EM_RANDOM_PAGE ||
			spTalk->m_dwACTION_NO == SNpcTalk::EM_ATTENDANCE_BOOK ||
			spTalk->m_dwACTION_NO == SNpcTalk::EM_ITEM_MIX )
		{
			continue;
		}

		if( std::find( vecTalk.begin(), vecTalk.end(), spTalk->m_dwACTION_NO ) != vecTalk.end() )
			continue;

		if( !spTalk->DoTEST( m_pChar, nPartyMember, cCurSvrTime.GetHour() ) )
			continue;

		vecTalk.push_back( spTalk->m_dwACTION_NO );
	}

	if( vecTalk.empty() )
		return;

	std::sort( vecTalk.begin(), vecTalk.end() );

	BOOST_FOREACH( DWORD nTalk, vecTalk )
	{
		AddTextNoSplit(
			SNpcTalk::szBASICDESC[ nTalk ].c_str(),
			NS_UITEXTCOLOR::YELLOW );
	}
}

void CCursorTargetInfo::AddInfoNpcQuest( CNpcTalkControl::NPCMAP* pTalkMap )
{
	const DWORD nPartyMember = m_pGaeaClient->GetMyPartyClient()->GetNMember();
	CTime cCurSvrTime = m_pGaeaClient->GetCurrentTime();

	BOOST_FOREACH( CNpcTalkControl::NPCMAP_VALUE& it, *pTalkMap )
	{
		std::tr1::shared_ptr< SNpcTalk > spTalk = it.second;
		if( !spTalk )
			continue;

		if( spTalk->m_nACTION != SNpcTalk::EM_QUEST_START )
			continue;

		if( !spTalk->DoTEST( m_pChar, nPartyMember, cCurSvrTime.GetHour() ) )
			continue;

		AddTextNoSplit(
			ID2GAMEINTEXT( "CURSOR_TARGET_QUESTSTART" ),
			D3DCOLOR_XRGB( 0, 180, 255 ) );
		break;
	}

	BOOST_FOREACH( CNpcTalkControl::NPCMAP_VALUE& it, *pTalkMap )
	{
		std::tr1::shared_ptr< SNpcTalk > spTalk = it.second;
		if( !spTalk )
			continue;

		if( spTalk->m_nACTION != SNpcTalk::EM_QUEST_STEP )
			continue;

		if( !spTalk->DoTEST( m_pChar, nPartyMember, cCurSvrTime.GetHour() ) )
			continue;

		AddTextNoSplit(
			ID2GAMEINTEXT( "CURSOR_TARGET_QUESTSTEP" ),
			D3DCOLOR_XRGB( 0, 255, 24 ) );
		break;
	}
}

void CCursorTargetInfo::UpdateInfoMob()
{
	GLCrowClient* pTarget = dynamic_cast< GLCrowClient* >( m_pActor );
	if( !pTarget || pTarget->IsHideTooltip())
		return;

	AddNameCondition( NS_UITEXTCOLOR::WHITE, NS_UITEXTCOLOR::RED );

	AddHp( NS_UITEXTCOLOR::YELLOW );

	if( m_pActor->IsNonDamaged() )
	{
		AddTextNoSplit(
			ID2GAMEINTEXT( "CANT_ATTACK_CROW" ),
			NS_UITEXTCOLOR::RED );
	}
}

void CCursorTargetInfo::AddNamePublic( DWORD nColor )
{
	std::string strText = sc::string::format(
		"Lv.%d %s",
		m_pActor->GetLevel(),
		m_pActor->GetName() );
	AddTextNoSplit( strText.c_str(), nColor );
}

void CCursorTargetInfo::AddNamePrivate( DWORD nColor )
{
	AddTextNoSplit( m_pActor->GetName(), nColor );
}

void CCursorTargetInfo::AddNameCondition( DWORD nColorPublic, DWORD nColorPrivate )
{
	if( m_pChar->GetLevel() + 30 < m_pActor->GetLevel() )
		AddNamePrivate( nColorPrivate );
	else
		AddNamePublic( nColorPublic );
}

void CCursorTargetInfo::AddSchool( DWORD nColor )
{
	GLCharClient* pTarget = dynamic_cast< GLCharClient* >( m_pActor );
	if( !pTarget )
		return;

	WORD nSchool = pTarget->GetCharSchool();
	if( nSchool >= GLSCHOOL_NUM )
		return;

	AddTextNoSplit( ID2GAMEWORD( "ACADEMY_NAME", nSchool ), nColor );
}

void CCursorTargetInfo::AddClass( DWORD nColor )
{
	GLCharClient* pTarget = dynamic_cast< GLCharClient* >( m_pActor );
	if( !pTarget )
		return;

	EMCHARINDEX eClassIndex = CharClassToIndex( EMCHARCLASS( pTarget->GetCharData().m_Base.emClass ) );
	if( eClassIndex >= GLCI_NUM_ETC )
		return;

	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
		AddTextNoSplit( CGameTextMan::GetInstance().GetCommentText("CHARCLASS", eClassIndex).GetString(), nColor );
	else
		AddTextNoSplit( COMMENT::CHARCLASS[ eClassIndex ].c_str(), nColor );
}

void CCursorTargetInfo::AddParty( DWORD nColor )
{
	if( m_pActor->IsPartyMaster() )
	{
		AddTextNoSplit(
			ID2GAMEINTEXT( "CURSOR_TARGET_PARTYMASTER" ),
			nColor );
	}
	else if( m_pActor->IsPartyMem() )
	{
		AddTextNoSplit(
			ID2GAMEINTEXT( "CURSOR_TARGET_PARTYMEMBER" ),
			nColor );
	}
	else
	{
		AddTextNoSplit(
			ID2GAMEINTEXT( "CURSOR_TARGET_PARTYNO" ),
			nColor );
	}
}

void CCursorTargetInfo::AddClub( DWORD nColor )
{
	GLCharClient* pTarget = dynamic_cast< GLCharClient* >( m_pActor );
	if( !pTarget )
		return;

	DWORD nClub = pTarget->GetClubID();
	if( nClub == CLUB_NULL )
		return;

	AddTextNoSplit( pTarget->GetClubName(), nColor );
}

void CCursorTargetInfo::AddHp( DWORD nColor )
{
	if( m_pActor->GetHp().dwMax <= 0 )
		return;

	FLOAT fPercent = static_cast< FLOAT >( m_pActor->GetHp().dwNow );
	fPercent /= static_cast< FLOAT >( m_pActor->GetHp().dwMax );
	fPercent *= 100.0f;

	INT nPercent = static_cast< INT >( fPercent );
	if( nPercent <= 0 && m_pActor->GetHp().dwNow > 0 )
		nPercent = 1;

	std::string strText = sc::string::format(
		"%d / %d (%d%%)",
		m_pActor->GetHp().dwNow,
		m_pActor->GetHp().dwMax,
		nPercent );
	AddTextNoSplit( strText.c_str(), nColor );
}

void CCursorTargetInfo::AddShiftClick( DWORD nColor )
{
	AddTextNoSplit(
		ID2GAMEINTEXT( "CURSOR_TARGET_SHIFTCLICK" ),
		nColor );
}

MyCursorTargetInfo::MyCursorTargetInfo( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
	: CCursorTargetInfo( pGaeaClient, pInterface, pEngineDevice )
{
}

void MyCursorTargetInfo::CreateUIWindowAndRegisterOwnership()
{
	Create( CURSOR_TARGET_INFO, "BASIC_VAR_TEXT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	CreateSubControl();
	RightBottomGap();
	SetBlockMouseTracking( true );
	SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
	m_pInterface->UiRegisterControl( this );
	m_pInterface->UiShowGroupBottom( GetWndID() );
	m_pInterface->UiHideGroup( GetWndID() );
}

void MyCursorTargetInfo::UpdateInfo()
{
	CCursorTargetInfo::UpdateInfo();
}