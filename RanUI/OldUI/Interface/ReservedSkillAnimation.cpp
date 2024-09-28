#include "StdAfx.h"
#include "BasicQuickSkillSlotEx.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"
#include "../../../RanLogicClient/Input/GLInputManager.h"

#include "ReservedSkillAnimation.h"
#include "../Skill/SkillImage.h"

#include <boost/lambda/lambda.hpp>

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CReservedSkillAnimation::CReservedSkillAnimation(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
: CUIGroup(pEngineDevice)
, m_pGaeaClient(pGaeaClient)
, m_pInterface(pInterface)
, m_pd3dDevice(NULL)
{
}

CReservedSkillAnimation::~CReservedSkillAnimation ()
{
	m_listSkillObject.clear();
}

HRESULT CReservedSkillAnimation::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;
	return CUIGroup::InitDeviceObjects( pd3dDevice );
}

HRESULT CReservedSkillAnimation::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;
	return CUIGroup::RestoreDeviceObjects( pd3dDevice );
}

void CReservedSkillAnimation::CreateSubControl ()
{

	//GASSERT( 0 <= nTabIndex && nTabIndex < MAX_TAB_INDEX );
	//m_nTabIndex = nTabIndex;

	//CBasicQuickSkillSlot::CreateSubControl ();
	//if ( m_pSkillMouseOver ) m_pSkillMouseOver->SetVisibleSingle ( FALSE );

	//m_pReserved = new CUIControl(m_pEngineDevice);
	//m_pReserved->CreateSub( this, "QUICK_SKILL_RESERVED", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	//m_pReserved->SetVisibleSingle( FALSE );
	//RegisterControl(m_pReserved);
}


void CReservedSkillAnimation::Show ( const SNATIVEID& sSkillID )
{
	using namespace boost::lambda;
	LIST_SKILLOBJ_ITER finditer = std::find_if( m_listSkillObject.begin(), m_listSkillObject.end(), boost::lambda::_1->*(&SKILLOBJECT::sID) == sSkillID );

	if ( finditer == m_listSkillObject.end() )
	{
		SKILLOBJECT* pNewSkillObj = new SKILLOBJECT(m_pGaeaClient, m_pInterface, m_pEngineDevice);
		pNewSkillObj->CreateSub( this, "RESERVED_SKILL_ANIMATION_SKILLOBJ" );
		pNewSkillObj->CreateSubControl ();
		pNewSkillObj->InitDeviceObjects( m_pd3dDevice );
		pNewSkillObj->RestoreDeviceObjects( m_pd3dDevice );
		pNewSkillObj->Reset(sSkillID);
		RegisterControl( pNewSkillObj );

		m_listSkillObject.push_back( pNewSkillObj );
	}
	else
	{
		SKILLOBJECT* pSkillObject = (*finditer);
		
		if ( pSkillObject )
		{
			pSkillObject->Reset(sSkillID);
		}

		//! 삭제후 뒤에 다시넣는다.
		m_listSkillObject.erase(finditer);
		m_listSkillObject.push_back( pSkillObject );

		m_ControlContainer.EraseControl( pSkillObject->GetWndID(), FALSE );
		m_ControlContainer.InsertControl( pSkillObject );
	}
}

void CReservedSkillAnimation::HideAll()
{
	LIST_SKILLOBJ_ITER iter = m_listSkillObject.begin();
	for ( ; iter != m_listSkillObject.end(); ++iter )
	{
		SKILLOBJECT* pSkillObj = (*iter);

		if ( pSkillObj )
		{
			pSkillObj->SetEnable(false);
		}
	}
}

//-------------------------------------------------------------------------------------------------------//

float CReservedSkillAnimation::SKILLOBJECT::fMAX_REMAIN_TIME = 2.0f;
float CReservedSkillAnimation::SKILLOBJECT::fMAX_EFFECT_TIME = 0.5f;
float CReservedSkillAnimation::SKILLOBJECT::fMAX_MOVING_TIME = 0.5f;
DWORD CReservedSkillAnimation::SKILLOBJECT::dwDEFAULT_COLOR  = 0xB0FFFFFF;

void CReservedSkillAnimation::SKILLOBJECT::CreateSubControl ()
{
	CSkillImage* pSkillImage = new CSkillImage(pGaeaClient, pEngineDevice);
	pSkillImage->CreateSub( this, "RESERVED_SKILL_ANIMATION_BEGIN" );
	pSkillImage->CreateSubControl();
	pSkillImage->SetUseRender( TRUE );
	RegisterControl( pSkillImage );
	pIMAGE = pSkillImage;

	pSkillImage = new CSkillImage(pGaeaClient, pEngineDevice);
	pSkillImage->CreateSub( this, "RESERVED_SKILL_ANIMATION_BEGIN" );
	pSkillImage->CreateSubControl();
	pSkillImage->SetUseRender( TRUE );
	RegisterControl( pSkillImage );
	pIMAGE_EFFECT = pSkillImage;

	CUIControl BeginControl(m_pEngineDevice); 
	BeginControl.Create ( 1, "RESERVED_SKILL_ANIMATION_BEGIN" );
	rcBeginRect = BeginControl.GetLocalPos();

	CUIControl EffectControl(m_pEngineDevice); 
	EffectControl.Create ( 1, "RESERVED_SKILL_ANIMATION_EFFECT" );
	rcEffectRect = EffectControl.GetLocalPos();
}

void CReservedSkillAnimation::SKILLOBJECT::Reset( const SNATIVEID& sSKillID )
{
	GASSERT(pIMAGE);
	GASSERT(pIMAGE_EFFECT);

	{
		sID		  = sSKillID;
		wSLOT	  = pInterface->GetSkillTrayTab()->FindSkillSlot(sSKillID);
		rcEndRect = pInterface->GetSkillTrayTab()->FindSkillSlotGlobalPos(sSKillID);

		if ( wSLOT == USHRT_MAX )
		{
			SetEnable(false);
			return;
		}
	}

	pIMAGE->ResetControl( GetGlobalPos(), "RESERVED_SKILL_ANIMATION_BEGIN" );
	pIMAGE_EFFECT->ResetControl( GetGlobalPos(), "RESERVED_SKILL_ANIMATION_BEGIN" );

	const LONG lResolution = pInterface->UiGetResolution();

	UIRECT rcBeginPos = rcBeginRect;
	rcBeginPos.left = GetDefaultPosX(lResolution) - (rcBeginPos.sizeX/2.0f);
	rcBeginPos.top  = GetDefaultPosY(lResolution) - (rcBeginPos.sizeY/2.0f);

	pIMAGE->SetSkill( sSKillID );
	pIMAGE->SetLocalPos(rcBeginPos);
	pIMAGE->SetGlobalPos(rcBeginPos);
	pIMAGE->SetDiffuse( CReservedSkillAnimation::SKILLOBJECT::dwDEFAULT_COLOR );
	pIMAGE->SetVisibleSingle( TRUE );
	pIMAGE_EFFECT->SetSkill( sSKillID );
	pIMAGE_EFFECT->SetLocalPos(rcBeginPos);
	pIMAGE_EFFECT->SetGlobalPos(rcBeginPos);
	pIMAGE_EFFECT->SetDiffuse( CReservedSkillAnimation::SKILLOBJECT::dwDEFAULT_COLOR );
	pIMAGE_EFFECT->SetVisibleSingle( TRUE );

	fANITIME = 0.0f;
	fANITIMEEFFECT = 0.0f;

	SetEnable(true);
}

void CReservedSkillAnimation::SKILLOBJECT::SetEnable( const bool bEnable )
{
	bENABLE = bEnable;
	SetVisibleSingle( bENABLE ? TRUE : FALSE );
}

void CReservedSkillAnimation::SKILLOBJECT::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if ( !bENABLE )
	{
		return;
	}

	CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	fANITIME	   += fElapsedTime;
	fANITIMEEFFECT += fElapsedTime;

	const LONG lResolution = pInterface->UiGetResolution();
	WORD X_RES = HIWORD( lResolution );
	WORD Y_RES = LOWORD( lResolution );

	//! 이펙트
	{
		float fEffectRatio = fANITIMEEFFECT/fMAX_EFFECT_TIME;

		if ( 1.0f <= fEffectRatio )
		{
			pIMAGE_EFFECT->SetVisibleSingle( FALSE );
			fEffectRatio = 1.0f;
		}

		UIRECT rcPos;
		rcPos.sizeX  = rcBeginRect.sizeX*(1.0f-fEffectRatio) + rcEffectRect.sizeX*(fEffectRatio);
		rcPos.sizeY  = rcBeginRect.sizeY*(1.0f-fEffectRatio) + rcEffectRect.sizeY*(fEffectRatio);
		rcPos.left   =  GetDefaultPosX(lResolution)  - (rcPos.sizeX/2.0f);
		rcPos.top    =  GetDefaultPosY(lResolution)  - (rcPos.sizeY/2.0f);
		rcPos.right  = rcPos.left + rcPos.sizeX;
		rcPos.bottom = rcPos.top  + rcPos.sizeY;

		pIMAGE_EFFECT->SetLocalPos( rcPos );
		pIMAGE_EFFECT->SetGlobalPos( rcPos );

		WORD wAlpha = static_cast<WORD>( ( CReservedSkillAnimation::SKILLOBJECT::dwDEFAULT_COLOR & 0xFF000000 ) >> 24 );
		wAlpha = static_cast<WORD>( wAlpha * (1.0f - fEffectRatio) );
		pIMAGE_EFFECT->SetDiffuseAlpha( wAlpha );
	}

	//! 메인 스킬 이미지
	if ( fANITIME <= fMAX_REMAIN_TIME )
	{
		float fRemainRatio = fANITIME/fMAX_REMAIN_TIME;

		if ( 1.0f <= fRemainRatio )
		{
			fRemainRatio = 1.0f;
		}

		// 아무것도 안함
	}
	else
	{
		float fMovingRatio = (fANITIME - fMAX_REMAIN_TIME)/fMAX_MOVING_TIME;

		if ( 1.0f <= fMovingRatio )
		{
			fMovingRatio = 1.0f;
			pIMAGE->SetVisibleSingle( FALSE );
			SetEnable( false );
			return;
		}

		float fBeginPosX = GetDefaultPosX(lResolution) - (rcBeginRect.sizeX/2.0f);
		float fBeginPosY = GetDefaultPosY(lResolution) - (rcBeginRect.sizeY/2.0f);

		UIRECT rcPos;
		rcPos.sizeX  = rcBeginRect.sizeX*(1.0f-fMovingRatio) + rcEndRect.sizeX*(fMovingRatio);
		rcPos.sizeY  = rcBeginRect.sizeY*(1.0f-fMovingRatio) + rcEndRect.sizeY*(fMovingRatio);
		rcPos.left   = fBeginPosX*(1.0f-fMovingRatio)  + rcEndRect.left*(fMovingRatio);
		rcPos.top    = fBeginPosY*(1.0f-fMovingRatio)  + rcEndRect.top*(fMovingRatio);
		rcPos.right  = rcPos.left + rcPos.sizeX;
		rcPos.bottom = rcPos.top  + rcPos.sizeY;

		pIMAGE->SetLocalPos( rcPos );
		pIMAGE->SetGlobalPos( rcPos );
	}
}

const float CReservedSkillAnimation::SKILLOBJECT::GetDefaultPosX( const LONG lResolution )
{
	WORD X_RES = HIWORD( lResolution );
	return static_cast<float>(X_RES/2);
}

const float CReservedSkillAnimation::SKILLOBJECT::GetDefaultPosY( const LONG lResolution )
{
	WORD Y_RES = LOWORD( lResolution );
	return static_cast<float>(Y_RES/4);
}
//-------------------------------------------------------------------------------------------------------//

MyReservedSkillAnimation::MyReservedSkillAnimation( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
	: CReservedSkillAnimation( pGaeaClient, pInterface, pEngineDevice )
{
}

void MyReservedSkillAnimation::CreateUIWindowAndRegisterOwnership()
{
	CReservedSkillAnimation::Create( RESERVED_SKILL_ANIMATION, "RESERVED_SKILL_ANIMATION" );
	CReservedSkillAnimation::CreateSubControl();
	CReservedSkillAnimation::m_pInterface->UiRegisterControl( this );
	CReservedSkillAnimation::m_pInterface->UiShowGroupTop( RESERVED_SKILL_ANIMATION );
}

void MyReservedSkillAnimation::Show ( const SNATIVEID& sSkillID )
{
	CReservedSkillAnimation::Show( sSkillID );
}

void MyReservedSkillAnimation::HideAll ()
{
	CReservedSkillAnimation::HideAll();
}
