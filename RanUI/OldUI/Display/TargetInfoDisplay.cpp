#include "StdAfx.h"
#include "./TargetInfoDisplay.h"
#include "./AuthenticatorCTFDisplay.h"

#include "../../../EngineLib/GUInterface/BasicProgressBar.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxEx.h"
#include "../../../EngineLib/GUInterface/UIMan.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"

#include "../../../RanLogicClient/GLGaeaClient.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

//!bjju.temp 이 컨트롤은 이벤트 처리를 받지 말아야합니다
//
//

CTargetInfoDisplay::CTargetInfoDisplay(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pInterface( pInterface )
	, m_pGaeaClient( pGaeaClient )
	, m_sMouseOver( pGaeaClient )
	, m_sSelect( pGaeaClient )
{
	//memset ( m_pSchoolMark, 0, sizeof ( CUIControl* ) * nSCHOOLMARK );
	//memset ( m_pPartyMark, 0, sizeof ( CUIControl* ) * nPARTYMARK );
}

CTargetInfoDisplay::~CTargetInfoDisplay()
{
}

void CTargetInfoDisplay::CreateSubControl()
{
#ifdef DEF_IMPROVE_DISPLAY_NAME
	m_pFont8 = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG | D3DFONT_SHADOW );
#else
	m_pFont8 = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
#endif

	m_sMouseOver.CreateSubControl( this, m_pFont8, m_pEngineDevice );
	m_sSelect.CreateSubControl(this, m_pFont8, m_pEngineDevice );
	m_sSelect.SetNonBoundaryCheck();

	//{
	//	CString strSchoolMark[] = 
	//	{
	//		"TARGET_DISPLAY_SCHOOL1",
	//		"TARGET_DISPLAY_SCHOOL2",
	//		"TARGET_DISPLAY_SCHOOL3"
	//	};

	//	for ( int i = 0; i < nSCHOOLMARK; i++ )
	//	{
	//		m_pSchoolMark[i] = CreateControl ( strSchoolMark[i].GetString(), UI_FLAG_LEFT );
	//		m_pSchoolMark[i]->SetVisibleSingle ( FALSE );
	//	}

	//	//m_pSchoolMark[0]->SetVisible ( TRUE );
	//}

	//{
	//	CString strPartyMark[] = 
	//	{
	//		"TARGET_DISPLAY_PARTY_MEMBER",
	//		"TARGET_DISPLAY_PARTY_MASTER",
	//		"TARGET_DISPLAY_PARTY_MEMBER_TARGET",
	//		"TARGET_DISPLAY_PARTY_MASTER_TARGET",
	//		"TARGET_DISPLAY_PARTY_MASTER_GENERAL"
	//	};

	//	for ( int i = 0; i < nPARTYMARK; i++ )
	//	{
	//		m_pPartyMark[i] = CreateControl ( strPartyMark[i].GetString(), UI_FLAG_RIGHT );
	//		m_pPartyMark[i]->SetVisibleSingle ( FALSE );
	//	}

	//	//m_pPartyMark[0]->SetVisible ( TRUE );
	//}
}

CUIControl*	CTargetInfoDisplay::CreateControl( const char* szControl, WORD wAlignFlag )
{
	CUIControl* pControl = new CUIControl(m_pEngineDevice);
	pControl->CreateSub( this, szControl, wAlignFlag );
	RegisterControl( pControl );
	return pControl;
}

const bool CTargetInfoDisplay::CheckPriority ( const UI::EMTARGETINFO_TYPE emInfoType, const STARGETID& sTargetID )
{
	switch ( emInfoType )
	{
	case UI::EMTARGETINFO_TYPE_MOUSEOVER:
		{
			if ( m_sSelect.m_sTargetID == sTargetID )
			{
				return false;
			}
		}
		break;

	case UI::EMTARGETINFO_TYPE_SELECT:
		{
			return true;
		}
		break;
	}

	return true;
}

const bool CTargetInfoDisplay::UpdatePriority ( const UI::EMTARGETINFO_TYPE emInfoType, const STARGETID& sTargetID )
{
	if ( !CheckPriority( emInfoType, sTargetID ) )
	{
		return false;
	}

	
	switch ( emInfoType )
	{
	//! 만약 마우스오버와 같은 타겟을 설정한다면 마우스오버를 꺼준다.,
	case UI::EMTARGETINFO_TYPE_SELECT:
		{
			if ( m_sMouseOver.m_sTargetID == sTargetID )
			{
				SetMouseOverVisibleOff();
			}
		}
		break;
	}

	return true;
}

void CTargetInfoDisplay::SetMouseOverTargetInfo( const STARGETID& sTargetID, float fPercent, CString strName, D3DCOLOR dwColor ) /*, int nSchoolMark, int nPartyMark )*/
{
	if ( !UpdatePriority( UI::EMTARGETINFO_TYPE_MOUSEOVER, sTargetID ) )
	{
		return;
	}

	m_sMouseOver.SetNormalVisible( sTargetID );
	m_sMouseOver.m_pNormalHP[m_sMouseOver.m_emColorType]->SetPercent( fPercent );
	m_sMouseOver.m_pNormalNameBox->ClearText();

	SIZE Size;
	m_pFont8->GetTextExtent( strName.GetString(), Size );
	
	const UIRECT& rcNameBoxDummy = m_sMouseOver.m_pNormalNameBoxDummy->GetLocalPos();
	float fNameWidth = 0.0f;
	fNameWidth = (float)Size.cx;

	{
		if ( fNameWidth < rcNameBoxDummy.sizeX )
		{
			fNameWidth = rcNameBoxDummy.sizeX;
		}

		//	텍스트 박스 크게 리사이징
		const UIRECT& rcNameLocalPos = m_sMouseOver.m_pNormalNameBox->GetLocalPos();
		const UIRECT& rcNameBoxNew = UIRECT ( rcNameLocalPos.left, rcNameLocalPos.top,
			fNameWidth, rcNameLocalPos.sizeY );

		m_sMouseOver.SetNormalAlignControl( rcNameLocalPos, rcNameBoxNew );

		//	텍스트 넣기 ( 알아서 잘림 )				
		m_sMouseOver.m_pNormalNameBox->AddTextNoSplit( strName, dwColor );	
		m_sMouseOver.m_strName = strName;

		// Rect 설정
		const UIRECT& rcBaseOld = m_sMouseOver.m_pNormalBase->GetLocalPos();
		UIRECT rcBaseNew = UIRECT ( rcBaseOld.left, rcBaseOld.top,
			rcBaseOld.sizeX + (fNameWidth - rcNameLocalPos.sizeX), rcBaseOld.sizeY );

		m_sMouseOver.m_rcPos = rcBaseNew;
	}
}

void CTargetInfoDisplay::SetSelectTargetInfo( const STARGETID& sTargetID, float fPercent, CString strName, D3DCOLOR dwColor )
{
	if ( !UpdatePriority( UI::EMTARGETINFO_TYPE_SELECT, sTargetID ) )
	{
		return;
	}

	m_sSelect.SetDetailVisible( sTargetID );

	m_sSelect.m_pDetailHP[m_sSelect.m_emColorType]->SetPercent( fPercent );
	m_sSelect.m_pDetailNameBox->ClearText();

	SIZE Size;
	m_pFont8->GetTextExtent( strName.GetString(), Size );

	const UIRECT& rcNameBoxDummy = m_sSelect.m_pDetailNameBoxDummy->GetLocalPos();
	float fWidth = 0.0f;
	fWidth = (float)Size.cx + 4.0f;

	const UIRECT& rcBaseDummy = m_sSelect.m_pDetailBase->GetLocalPos();
	{
		if ( fWidth < rcNameBoxDummy.sizeX )
		{
			fWidth = rcNameBoxDummy.sizeX;
		}

		//	리사이징
		UIRECT rcOldLocalPos = m_sSelect.m_pDetailBase->GetLocalPos();
		rcOldLocalPos.sizeX = m_sSelect.m_pDetailHP[m_sSelect.m_emColorType]->GetLocalPos().sizeX;

		UIRECT rcNewLocalPos = UIRECT ( rcOldLocalPos.left, rcOldLocalPos.top,
			rcBaseDummy.sizeX + (fWidth - rcNameBoxDummy.sizeX), rcOldLocalPos.sizeY );

		m_sSelect.SetDetailAlignControl( rcOldLocalPos, rcNewLocalPos );
		m_sSelect.m_rcPos = rcNewLocalPos;

		//	텍스트 넣기 ( 알아서 잘림 )				
		m_sSelect.m_pDetailNameBox->AddTextNoSplit( strName, dwColor );	
		m_sSelect.m_strName = strName;
	}
}

void CTargetInfoDisplay::SetMouseOverAuthenticatorInfo( const STARGETID& sTargetID, float fPercentSM, float fPercentHA, float fPercentBH, ENUM_SCHOOL emAuthSchool )
{
	if ( !UpdatePriority( UI::EMTARGETINFO_TYPE_MOUSEOVER, sTargetID ) )
	{
		return;
	}

	m_sMouseOver.SetCTFVisible( sTargetID );
    m_sMouseOver.m_pCTF->SetAuthSchool( emAuthSchool );
    m_sMouseOver.m_pCTF->SetPercentSM( fPercentSM );
    m_sMouseOver.m_pCTF->SetPercentHA( fPercentHA );
    m_sMouseOver.m_pCTF->SetPercentBH( fPercentBH );
	m_sMouseOver.m_pCTF->Update();

	m_sMouseOver.m_rcPos = m_sMouseOver.m_pCTF->GetLocalPos();
}


void CTargetInfoDisplay::SetSelectAuthenticatorInfo( const STARGETID& sTargetID, float fPercentSM, float fPercentHA, float fPercentBH, ENUM_SCHOOL emAuthSchool )
{
	if ( !UpdatePriority( UI::EMTARGETINFO_TYPE_SELECT, sTargetID ) )
	{
		return;
	}

	m_sSelect.SetCTFVisible( sTargetID );
	m_sSelect.m_pCTF->SetAuthSchool( emAuthSchool );
	m_sSelect.m_pCTF->SetPercentSM( fPercentSM );
	m_sSelect.m_pCTF->SetPercentHA( fPercentHA );
	m_sSelect.m_pCTF->SetPercentBH( fPercentBH );
	m_sSelect.m_pCTF->Update();

	m_sSelect.m_rcPos = m_sSelect.m_pCTF->GetLocalPos();
}

const UIRECT& CTargetInfoDisplay::GetMouseOverGlobalPos()
{
	return m_sMouseOver.m_rcPos;
}

void CTargetInfoDisplay::SetMouseOverGlobalPos( const D3DXVECTOR2& vPos )
{
	m_sMouseOver.SetGlobalPos( vPos );
}

const UIRECT& CTargetInfoDisplay::GetSelectGlobalPos()
{
	return m_sSelect.m_rcPos;
}

void CTargetInfoDisplay::SetSelectGlobalPos( const D3DXVECTOR2& vPos )
{
	m_sSelect.SetGlobalPos( vPos );
}

void CTargetInfoDisplay::SetMouseOverVisibleOff() 
{ 
	m_sMouseOver.SetVisibleOff(); 
	m_sMouseOver.m_sTargetID = STARGETID();
}
	  
void CTargetInfoDisplay::SetSelectVisibleOff() 
{ 
	m_sSelect.SetVisibleOff(); 
	m_sSelect.m_sTargetID = STARGETID();
}
//-----------------------------------------------------------------------------------------------//

void CTargetInfoDisplay::SINFO::CreateSubControl( CUIGroup* pParent, CD3DFontPar* pFont, EngineDeviceMan* pEngineDevice )
{
	GASSERT(pParent);
	GASSERT(pFont);
	GASSERT(pEngineDevice);
	GASSERT(m_pGaeaClient);

	//! 타겟 표시(일반)
	{
#ifndef DEF_IMPROVE_DISPLAY_NAME
		CBasicLineBoxEx* pLineBox = new CBasicLineBoxEx(pEngineDevice);
		pLineBox->CreateSub( pParent, "BASIC_LINE_BOX_EX_TARGETINFO", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pLineBox->CreateBaseBoxTargetInfo( "BASIC_TARGETINFO_LINE_BOX" );
		pLineBox->SetVisibleSingle( FALSE );
		pParent->RegisterControl( pLineBox );
		m_pNormalLineBox = pLineBox;
#endif
		{
			CBasicProgressBar* pHP = new CBasicProgressBar(pEngineDevice);
			pHP->CreateSub( pParent, "TARGETINFO_HP" );
			pHP->CreateOverImage( "TARGETINFO_HP_OVERIMAGE_ENEMY" );
			pHP->SetControlNameEx( "HP 프로그래스바" );
			pHP->SetAlignFlag( UI_FLAG_XSIZE );
			pHP->SetVisibleSingle( FALSE );
			pParent->RegisterControl( pHP );
			m_pNormalHP[EMCROWCOLORTYPE_ENEMY] = pHP;

			pHP = new CBasicProgressBar(pEngineDevice);
			pHP->CreateSub( pParent, "TARGETINFO_HP" );
			pHP->CreateOverImage( "TARGETINFO_HP_OVERIMAGE_NPC" );
			pHP->SetControlNameEx( "HP 프로그래스바" );
			pHP->SetAlignFlag( UI_FLAG_XSIZE );
			pHP->SetVisibleSingle( FALSE );
			pParent->RegisterControl( pHP );
			m_pNormalHP[EMCROWCOLORTYPE_NPC] = pHP;

			pHP = new CBasicProgressBar(pEngineDevice);
			pHP->CreateSub( pParent, "TARGETINFO_HP" );
			pHP->CreateOverImage( "TARGETINFO_HP_OVERIMAGE_ALLIANCE" );
			pHP->SetControlNameEx( "HP 프로그래스바" );
			pHP->SetAlignFlag( UI_FLAG_XSIZE );
			pHP->SetVisibleSingle( FALSE );
			pParent->RegisterControl( pHP );
			m_pNormalHP[EMCROWCOLORTYPE_ALLIANCE] = pHP;
		}

		CBasicTextBox* pNameBox = new CBasicTextBox(pEngineDevice);
		pNameBox->CreateSub( pParent, "TARGETINFO_NAMEBOX" );
		pNameBox->SetFont( pFont );
		pNameBox->SetTextAlign( TEXT_ALIGN_CENTER_X );
		pNameBox->SetControlNameEx( "HP 텍스트박스" );
		pNameBox->SetAlignFlag( UI_FLAG_XSIZE );
		pNameBox->SetVisibleSingle( FALSE );
		pParent->RegisterControl( pNameBox );
		m_pNormalNameBox = pNameBox;

		m_pNormalNameBoxDummy = new CUIControl(pEngineDevice);
		m_pNormalNameBoxDummy->CreateSub( pParent, "TARGETINFO_NAMEBOX", UI_FLAG_DEFAULT );
		pParent->RegisterControl( m_pNormalNameBoxDummy );

		m_pNormalBase = new CUIControl(pEngineDevice);
		m_pNormalBase->CreateSub( pParent, "BASIC_TARGETINFO_LINE_BOX" );
		pParent->RegisterControl( m_pNormalBase );
	}

	//! 타겟 표시(정밀)
	{
#ifndef DEF_IMPROVE_DISPLAY_NAME
		CBasicLineBoxEx* pLineBox = new CBasicLineBoxEx(pEngineDevice);
		pLineBox->CreateSub( pParent, "BASIC_LINE_BOX_EX_TARGETINFO", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pLineBox->CreateBaseBoxTargetInfo( "TARGETINFO_DETAIL_LINEBOX" );
		pLineBox->SetVisibleSingle( FALSE );
		pParent->RegisterControl( pLineBox );
		m_pDetailLineBox = pLineBox;
#endif

		{
			CBasicProgressBar* pHP = new CBasicProgressBar(pEngineDevice);
			pHP->CreateSub( pParent, "TARGETINFO_DETAIL_HP" );
			pHP->CreateOverImage( "TARGETINFO_DETAIL_HP_OVERIMAGE_ENEMY" );
			pHP->CreateGrid ( "TARGETINFO_DETAIL_HP_GRID" );
			pHP->SetControlNameEx( "HP 프로그래스바" );
			pHP->SetAlignFlag( UI_FLAG_XSIZE );
			pHP->SetVisibleSingle( FALSE );
			pParent->RegisterControl( pHP );
			m_pDetailHP[EMCROWCOLORTYPE_ENEMY] = pHP;

			pHP = new CBasicProgressBar(pEngineDevice);
			pHP->CreateSub( pParent, "TARGETINFO_DETAIL_HP" );
			pHP->CreateOverImage( "TARGETINFO_DETAIL_HP_OVERIMAGE_NPC" );
			pHP->CreateGrid ( "TARGETINFO_DETAIL_HP_GRID" );
			pHP->SetControlNameEx( "HP 프로그래스바" );
			pHP->SetAlignFlag( UI_FLAG_XSIZE );
			pHP->SetVisibleSingle( FALSE );
			pParent->RegisterControl( pHP );
			m_pDetailHP[EMCROWCOLORTYPE_NPC] = pHP;

			pHP = new CBasicProgressBar(pEngineDevice);
			pHP->CreateSub( pParent, "TARGETINFO_DETAIL_HP" );
			pHP->CreateOverImage( "TARGETINFO_DETAIL_HP_OVERIMAGE_ALLIANCE" );
			pHP->CreateGrid ( "TARGETINFO_DETAIL_HP_GRID" );
			pHP->SetControlNameEx( "HP 프로그래스바" );
			pHP->SetAlignFlag( UI_FLAG_XSIZE );
			pHP->SetVisibleSingle( FALSE );
			pParent->RegisterControl( pHP );
			m_pDetailHP[EMCROWCOLORTYPE_ALLIANCE] = pHP;
		}

		CBasicTextBox* pNameBox = new CBasicTextBox(pEngineDevice);
		pNameBox->CreateSub( pParent, "TARGETINFO_DETAIL_NAMEBOX", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pNameBox->SetFont( pFont );
		pNameBox->SetTextAlign( TEXT_ALIGN_CENTER_X );
		pNameBox->SetControlNameEx( "HP 텍스트박스" );
		pNameBox->SetAlignFlag( UI_FLAG_XSIZE | UI_FLAG_CENTER_X );
		pNameBox->SetVisibleSingle( FALSE );
		pParent->RegisterControl( pNameBox );
		m_pDetailNameBox = pNameBox;

		m_pDetailNameBoxDummy = new CUIControl(pEngineDevice);
		m_pDetailNameBoxDummy->CreateSub( pParent, "TARGETINFO_DETAIL_NAMEBOX", UI_FLAG_DEFAULT );
		pParent->RegisterControl( m_pDetailNameBoxDummy );

		m_pDetailBase = new CUIControl(pEngineDevice);
		m_pDetailBase->CreateSub( pParent, "BASIC_TARGETINFO_DETAIL" );
		pParent->RegisterControl( m_pDetailBase );
	}

	//! CTF 인증기 정보
	{
		m_pCTF = new CAuthenticatorCTFDisplay(pEngineDevice);
		m_pCTF->CreateSub( pParent, "TARGETINFO_CTF" );
		m_pCTF->CreateSubControl();
		m_pCTF->Update();
		m_pCTF->SetVisibleSingle( FALSE );
		pParent->RegisterControl(m_pCTF );
	}
}

void CTargetInfoDisplay::SINFO::SetVisibleOff()
{
	m_emType	  = EMINFOTYPE_NULL;
	m_emColorType = EMCROWCOLORTYPE_NULL;

#ifndef DEF_IMPROVE_DISPLAY_NAME
	m_pNormalLineBox      ->SetVisibleSingle( FALSE );
#endif

	m_pNormalNameBox      ->SetVisibleSingle( FALSE );
	m_pNormalNameBoxDummy ->SetVisibleSingle( FALSE );

	for ( UINT i=0; i<EMCROWCOLORTYPE_SIZE; ++i )
	{
		m_pNormalHP[i]->SetVisibleSingle( FALSE );
	}

#ifndef DEF_IMPROVE_DISPLAY_NAME
	m_pDetailLineBox	  ->SetVisibleSingle( FALSE );
#endif

	m_pDetailNameBox	  ->SetVisibleSingle( FALSE );
	m_pDetailNameBoxDummy ->SetVisibleSingle( FALSE );

	for ( UINT i=0; i<EMCROWCOLORTYPE_SIZE; ++i )
	{
		m_pDetailHP[i]->SetVisibleSingle( FALSE );
	}

	m_pCTF->SetVisibleSingle( FALSE );
}

void CTargetInfoDisplay::SINFO::SetNormalVisible( const STARGETID& sTargetID )
{
	m_sTargetID = sTargetID;

	SetVisibleOff();

	m_emType = EMINFOTYPE_NORMAL;
	m_emColorType = m_pGaeaClient->GetCrowIdentifyColorType( m_sTargetID );

#ifndef DEF_IMPROVE_DISPLAY_NAME
	m_pNormalLineBox			->SetVisibleSingle( TRUE );
#endif

	m_pNormalNameBox			->SetVisibleSingle( TRUE );
	m_pNormalNameBoxDummy		->SetVisibleSingle( TRUE );
	m_pNormalHP[m_emColorType]	->SetVisibleSingle( TRUE );
}

void CTargetInfoDisplay::SINFO::SetDetailVisible( const STARGETID& sTargetID )
{
	m_sTargetID = sTargetID;

	SetVisibleOff();

	m_emType = EMINFOTYPE_DETAIL;
	m_emColorType = m_pGaeaClient->GetCrowIdentifyColorType( m_sTargetID );

#ifndef DEF_IMPROVE_DISPLAY_NAME
	m_pDetailLineBox			->SetVisibleSingle( TRUE );
#endif

	m_pDetailNameBox			->SetVisibleSingle( TRUE );
	m_pDetailNameBoxDummy		->SetVisibleSingle( TRUE );
	m_pDetailHP[m_emColorType]	->SetVisibleSingle( TRUE );
}

void CTargetInfoDisplay::SINFO::SetCTFVisible( const STARGETID& sTargetID )
{
	m_sTargetID = sTargetID;

	SetVisibleOff();

	m_emType = EMINFOTYPE_CTF_AUTH;
	m_emColorType = EMCROWCOLORTYPE_NULL;

	m_pCTF->SetVisibleSingle( TRUE );
}

void CTargetInfoDisplay::SINFO::SetNormalAlignControl( const UIRECT& rcOldParent, const UIRECT& rcNewParent )
{
	const UIRECT& rcOldPos = m_pNormalBase->GetLocalPos();
	UIRECT rcNewPos = rcOldPos;

	rcNewPos.sizeX = rcNewParent.sizeX - (rcOldParent.sizeX - rcOldPos.sizeX);

#ifndef DEF_IMPROVE_DISPLAY_NAME
	m_pNormalLineBox->AlignSubControl( rcOldPos, rcNewPos );
#endif

	m_pNormalNameBox->AlignSubControl( rcOldPos, rcNewPos );
	m_pNormalHP[m_emColorType]->AlignSubControl( rcOldPos, rcNewPos );
}

void CTargetInfoDisplay::SINFO::SetDetailAlignControl( const UIRECT& rcOldParent, const UIRECT& rcNewParent )
{
	UIRECT rcOldPos = m_pDetailBase->GetLocalPos();
	rcOldPos.sizeX = m_pDetailHP[m_emColorType]->GetLocalPos().sizeX;

	UIRECT rcNewPos = rcOldPos;
	rcNewPos.sizeX = rcNewParent.sizeX - (rcOldParent.sizeX - rcOldPos.sizeX);
	rcNewPos.right = rcNewPos.left + rcNewPos.sizeX;

#ifndef DEF_IMPROVE_DISPLAY_NAME
	m_pDetailLineBox->AlignSubControl( rcOldPos, rcNewPos );
#endif

	m_pDetailNameBox->AlignSubControl( rcOldPos, rcNewPos );
	m_pDetailHP[m_emColorType]->AlignSubControl( rcOldPos, rcNewPos );

#ifndef DEF_IMPROVE_DISPLAY_NAME
	//! 예외처리
	{
		m_pDetailLineBox->SetAlignFlag( UI_FLAG_CENTER_X );
		m_pDetailLineBox->CUIControl::AlignSubControl( rcOldPos, rcNewPos );
		m_pDetailLineBox->SetAlignFlag( (m_pDetailLineBox->GetAlignFlag() & ~UI_FLAG_CENTER_X) );
	}
#endif

}

void CTargetInfoDisplay::SINFO::SetGlobalPos ( const D3DXVECTOR2& vPos )
{
	switch( m_emType )
	{
	case EMINFOTYPE_NORMAL:
		{
#ifndef DEF_IMPROVE_DISPLAY_NAME
			{
				const UIRECT& rcLocalPos = m_pNormalLineBox->GetLocalPos();
				m_pNormalLineBox->SetGlobalPos( UIRECT(vPos.x + rcLocalPos.left, vPos.y + rcLocalPos.top, rcLocalPos.sizeX, rcLocalPos.sizeY) );
			}
#endif
			{
				const UIRECT& rcLocalPos = m_pNormalNameBox->GetLocalPos();
				m_pNormalNameBox->SetGlobalPos( UIRECT(vPos.x + rcLocalPos.left, vPos.y + rcLocalPos.top, rcLocalPos.sizeX, rcLocalPos.sizeY) );
			}

			{
				const UIRECT& rcLocalPos = m_pNormalHP[m_emColorType]->GetLocalPos();
				m_pNormalHP[m_emColorType]->SetGlobalPos( UIRECT(vPos.x + rcLocalPos.left, vPos.y + rcLocalPos.top, rcLocalPos.sizeX, rcLocalPos.sizeY) );
			}

			m_rcPos = m_pNormalBase->GetLocalPos();
			m_rcPos.left = vPos.x;
			m_rcPos.top  = vPos.y;
		}
		break;

	case EMINFOTYPE_DETAIL:
		{
#ifndef DEF_IMPROVE_DISPLAY_NAME
			{
				const UIRECT& rcLocalPos = m_pDetailLineBox->GetLocalPos();
				m_pDetailLineBox->SetGlobalPos( UIRECT(vPos.x + rcLocalPos.left, vPos.y + rcLocalPos.top, rcLocalPos.sizeX, rcLocalPos.sizeY) );
			}
#endif

			{
				const UIRECT& rcLocalPos = m_pDetailNameBox->GetLocalPos();
				m_pDetailNameBox->SetGlobalPos( UIRECT(vPos.x + rcLocalPos.left, vPos.y + rcLocalPos.top, rcLocalPos.sizeX, rcLocalPos.sizeY) );
			}

			{
				const UIRECT& rcLocalPos = m_pDetailHP[m_emColorType]->GetLocalPos();
				m_pDetailHP[m_emColorType]->SetGlobalPos( UIRECT(vPos.x + rcLocalPos.left, vPos.y + rcLocalPos.top, rcLocalPos.sizeX, rcLocalPos.sizeY) );
			}

			m_rcPos = m_pDetailBase->GetLocalPos();
			m_rcPos.left = vPos.x;
			m_rcPos.top  = vPos.y;
		}
		break;

	case EMINFOTYPE_CTF_AUTH:
		{
			const UIRECT& rcLocalPos = m_pCTF->GetLocalPos();
			m_pCTF->SetGlobalPos( UIRECT(vPos.x + rcLocalPos.left, vPos.y + rcLocalPos.top, rcLocalPos.sizeX, rcLocalPos.sizeY) );

			m_rcPos = m_pCTF->GetGlobalPos();
		}
		break;

	default:
		{
			m_rcPos = UIRECT(0.0f,0.0f,0.0f,0.0f);
		}
		break;
	};
}

void CTargetInfoDisplay::SINFO::SetNonBoundaryCheck ()
{
#ifndef DEF_IMPROVE_DISPLAY_NAME
	m_pNormalLineBox->SetNonBoundaryCheck( true );
#endif
	m_pNormalNameBox->SetNonBoundaryCheck( true );

	for ( UINT i=0; i<EMCROWCOLORTYPE_SIZE; ++i )
	{
		m_pNormalHP[i]->SetNonBoundaryCheck( true );
	}

#ifndef DEF_IMPROVE_DISPLAY_NAME
	m_pDetailLineBox->SetNonBoundaryCheck( true );
#endif

	m_pDetailNameBox->SetNonBoundaryCheck( true );

	for ( UINT i=0; i<EMCROWCOLORTYPE_SIZE; ++i )
	{
		m_pDetailHP[i]->SetNonBoundaryCheck( true );
	}

	m_pCTF->SetNonBoundaryCheck( true );
}

//-----------------------------------------------------------------------------------------------//

MyTargetInfoDisplay::MyTargetInfoDisplay(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
    : CTargetInfoDisplay( pGaeaClient, pInterface, pEngineDevice )
{
}

void MyTargetInfoDisplay::CreateUIWindowAndRegisterOwnership()
{
    CTargetInfoDisplay::Create( TARGETINFO_DISPLAY, "TARGETINFO_DISPLAY" );
    CTargetInfoDisplay::CreateSubControl();
    CTargetInfoDisplay::m_pInterface->UiRegisterControl( this );
    CTargetInfoDisplay::m_pInterface->UiShowGroupBottom( TARGETINFO_DISPLAY );
}

const bool MyTargetInfoDisplay::CheckPriority ( const UI::EMTARGETINFO_TYPE emInfoType, const STARGETID& sTargetID )
{
	return CTargetInfoDisplay::CheckPriority( emInfoType, sTargetID );
}

const bool MyTargetInfoDisplay::IsMouseOverVisible()
{
	return CTargetInfoDisplay::IsMouseOverVisible();
}

void MyTargetInfoDisplay::SetMouseOverVisibleOff()
{
	CTargetInfoDisplay::SetMouseOverVisibleOff();
}

const UIRECT& MyTargetInfoDisplay::GetMouseOverGlobalPos()
{
    return CTargetInfoDisplay::GetMouseOverGlobalPos();
}

void MyTargetInfoDisplay::SetMouseOverGlobalPos( const D3DXVECTOR2& vPos )
{
    CTargetInfoDisplay::SetMouseOverGlobalPos( vPos );
}

void MyTargetInfoDisplay::SetMouseOverTargetInfo( const STARGETID& sTargetID, float fPercent, UI::String strName, D3DCOLOR dwColor )
{
    CTargetInfoDisplay::SetMouseOverTargetInfo( sTargetID, fPercent, UI::ToString( strName ), dwColor );
}

void MyTargetInfoDisplay::SetMouseOverAuthenticatorInfo( const STARGETID& sTargetID, float fPercentSM, float fPercentHA, float fPercentBH, ENUM_SCHOOL emAuthSchool )
{
    CTargetInfoDisplay::SetMouseOverAuthenticatorInfo( sTargetID, fPercentSM, fPercentHA, fPercentBH, emAuthSchool );
}

const UI::String MyTargetInfoDisplay::GetMouseOverTargetName() const
{
    const UI::String strTargetName = UI::ToString( CTargetInfoDisplay::GetMouseOverTargetName() );
    return strTargetName;
}

void MyTargetInfoDisplay::SetSelectVisibleOff()
{
	CTargetInfoDisplay::SetSelectVisibleOff();
}

void MyTargetInfoDisplay::SetSelectTargetInfo( const STARGETID& sTargetID, float fPercent, UI::String strName, D3DCOLOR dwColor )
{
	CTargetInfoDisplay::SetSelectTargetInfo( sTargetID, fPercent, UI::ToString( strName ), dwColor );
}

void MyTargetInfoDisplay::SetSelectAuthenticatorInfo( const STARGETID& sTargetID, float fPercentSM, float fPercentHA, float fPercentBH, ENUM_SCHOOL emAuthSchool )
{
	CTargetInfoDisplay::SetSelectAuthenticatorInfo( sTargetID, fPercentSM, fPercentHA, fPercentBH, emAuthSchool );
}

const UIRECT& MyTargetInfoDisplay::GetSelectGlobalPos()
{
	return CTargetInfoDisplay::GetSelectGlobalPos();
}

void MyTargetInfoDisplay::SetSelectGlobalPos( const D3DXVECTOR2& vPos )
{
	CTargetInfoDisplay::SetSelectGlobalPos( vPos );
}
