#include "stdafx.h"

#include "./LottoMachineViewerUIComp.h"

#include "./ILottoSystemUI.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/DxTools/d3dfont.h"

#include "../../../RanLogic/GLogicEx.h"
#include "../../../RanLogic/GLUseFeatures.h"

#include "../Util/UIViewFrameMeshComponent.h"

#include "../../InnerInterface.h"

#include "../../../SigmaCore/DebugInclude.h"

// Logic;
#include "../../../RanLogicClient/LottoSystem/GLLottoSystemManClient.h"
#include "../../../RanLogicClient/LottoSystem/GLLottoGlobalDataClient.h"

CLottoMachineViewerUIComp::CLottoMachineViewerUIComp( CInnerInterface* pInterface
													 , EngineDeviceMan* pEngineDevice )
													 : CUIGroupHelper( pEngineDevice )
													 , m_pInterface( pInterface )
													 , m_pViewer( NULL )
													 , m_pDontUseMachineBox( NULL )
													 , m_id( LottoSystem::LOTTO_NONE_ID )
{
}

CLottoMachineViewerUIComp::~CLottoMachineViewerUIComp()
{
	
}

void CLottoMachineViewerUIComp::CreateSubControl()
{
	// Viewer;
	{
		m_pViewer = new CViewFrameMeshComponent(
			m_pInterface, m_pEngineDevice );
		if( m_pViewer )
		{
			// Light, Material;
			D3DLIGHT9 light[3];
			D3DMATERIAL9 material;

			{
				ZeroMemory( &light[0], sizeof(D3DLIGHT9) );
				ZeroMemory( &light[1], sizeof(D3DLIGHT9) );
				ZeroMemory( &light[2], sizeof(D3DLIGHT9) );
				ZeroMemory( &material, sizeof(material) );

				D3DXVECTOR3 _vLight0 = D3DXVECTOR3( 0.7071f, -0.707f, 0.f );
				D3DXVECTOR3 _vLight1 = D3DXVECTOR3( 0.7071f, 0.7071f, 0.f );
				D3DXVECTOR3 _vLight2 = D3DXVECTOR3( -0.447f, -0.894f, 0.f );
				D3DXVec3Normalize( &_vLight0, &_vLight0 );
				D3DXVec3Normalize( &_vLight1, &_vLight1 );
				D3DXVec3Normalize( &_vLight2, &_vLight2 );

				const float _fFactor( 1.0f / 255.0f );
				light[0].Type = D3DLIGHT_DIRECTIONAL;
				light[0].Direction = _vLight0;
				light[0].Diffuse	= D3DXCOLOR( 255.0f * _fFactor, 255.0f * _fFactor, 255.0f * _fFactor, 1.0f );
				light[0].Ambient	= D3DXCOLOR( 191.0f * _fFactor, 188.0f * _fFactor, 188.0f * _fFactor, 1.0f );
				light[0].Specular	= D3DXCOLOR( 229.0f * _fFactor, 228.0f * _fFactor, 223.0f * _fFactor, 1.0f );

				light[1].Type = D3DLIGHT_DIRECTIONAL;
				light[1].Direction = _vLight1;
				light[1].Diffuse	= D3DXCOLOR( 121.0f * _fFactor, 18.0f * _fFactor, 139.0f * _fFactor, 1.0f );
				light[1].Ambient	= D3DXCOLOR( 11.0f * _fFactor, 11.0f * _fFactor, 11.0f * _fFactor, 1.0f );
				light[1].Specular	= D3DXCOLOR( 139.0f * _fFactor, 91.0f * _fFactor, 26.0f * _fFactor, 1.0f );

				light[2].Type = D3DLIGHT_DIRECTIONAL;
				light[2].Direction = _vLight2;
				light[2].Diffuse	= D3DXCOLOR( 69.0f * _fFactor, 96.0f * _fFactor, 255.0f * _fFactor, 1.0f );
				light[2].Ambient	= D3DXCOLOR( 7.0f * _fFactor, 7.0f * _fFactor, 7.0f * _fFactor, 1.0f );
				light[2].Specular	= D3DXCOLOR( 151.0f * _fFactor, 171.0f * _fFactor, 255.0f * _fFactor, 1.0f );

				material.Diffuse	= D3DXCOLOR( 255.0f * _fFactor, 255.0f * _fFactor, 255.0f * _fFactor, 1.0f );
				material.Ambient	= D3DXCOLOR( 149.0f * _fFactor, 149.0f * _fFactor, 191.0f * _fFactor, 1.0f );
				material.Specular	= D3DXCOLOR( 139.0f * _fFactor, 139.0f * _fFactor, 135.0f * _fFactor, 1.0f );
				material.Emissive	= D3DXCOLOR( 0.0f * _fFactor, 0.0f * _fFactor, 0.0f * _fFactor, 1.0f );
				material.Power = 8.8f;
			}	

			// Viewer 설정;
			m_pViewer->CreateSub( this, "LOTTO_MACHINE_VIEWER_REGION", UI_FLAG_DEFAULT, ELottoMachine );
			m_pViewer->CreateSubControl( "LOTTO_MACHINE_VIEWER_REGION", "WHITE_LINE_BLACK_BACKGROUND" );
			m_pViewer->CreateViewMatrix(
				D3DXVECTOR3( 124.58f, 64.579f, -336.088f ),
				D3DXVECTOR3( 46.318f, 63.49f, -121.067f ) );
			m_pViewer->AddFrameMesh( "lotto_machine_ah.X" );
			m_pViewer->AddFrameMesh( "lotto_ball_ah.X" );
			m_pViewer->SetMaterialState( true );
			m_pViewer->SetAnimationTime( 10.0f, 0 );
			m_pViewer->SetAnimationTime( 10.0f, 1 );
			m_pViewer->AddStopAnimationTime( 9.5f, 0.5f, 1 );
			m_pViewer->AddCallbackFunc( boost::bind(
				boost::mem_fn( &CLottoMachineViewerUIComp::AnimationTrigger ), this ) );
			m_pViewer->SetLight( light[ 0 ], 0 );
			m_pViewer->SetLight( light[ 1 ], 1 );
			m_pViewer->SetLight( light[ 2 ], 2 );
			m_pViewer->SetMaterial( material );
			RegisterControl( m_pViewer );
		}
	}

	// Don't Use Machine UI Box;
	{
		m_pDontUseMachineBox = new CBasicLineBoxSmart(GetEngineDeviceMan());
		m_pDontUseMachineBox->CreateSub( this, "LOTTO_MACHINE_VIEWER_REGION" ,UI_FLAG_DEFAULT, NO_ID );
		m_pDontUseMachineBox->CreateSubControl( "WHITE_LINE_DARKGRAY_BACKGROUND" );
		m_pDontUseMachineBox->SetVisibleSingle( FALSE );
		RegisterControl( m_pDontUseMachineBox );
	}
}

void CLottoMachineViewerUIComp::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroupHelper::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CLottoMachineViewerUIComp::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroupHelper::TranslateUIMessage( ControlID, dwMsg );

	switch( ControlID )
	{
	case ELottoMachine:
		if( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
		{
			if( GLUseFeatures::GetInstance().IsUsingLottoSystem() &&
				GLUseFeatures::GetInstance().IsUsingColorProperty3D() )
			{
				// 애니메이션 시작;
				m_pViewer->ResetAnimation();
				m_pViewer->StartAnimation();
			}
		}
		break;
	}
}

void CLottoMachineViewerUIComp::SetVisibleSingle( BOOL bVisible )
{
	CUIGroupHelper::SetVisibleSingle( bVisible );

	if( m_pViewer )
		m_pViewer->SetVisibleSingle( bVisible );
}

static std::string GetLottoNumTextureString( int lottoNum )
{
	std::string rString;
	if( lottoNum+1 > 0 && lottoNum+1 < 10 )
		rString += sc::string::format( "lotto_0%d", lottoNum+1 );
	else if( lottoNum+1 >= 10 && lottoNum+1 <= 45 )
		rString += sc::string::format( "lotto_%d", lottoNum+1 );
	rString += ".DDS";

	return rString;
}

void CLottoMachineViewerUIComp::UpdateTurn( const LottoSystem::CLottoSystemClient* pLottoSystem )
{
	if( pLottoSystem )
	{
		if( !GLUseFeatures::GetInstance().IsUsingColorProperty3D() )
		{
			LottoSystem::EMLOTTO_STATE state = pLottoSystem->GetLottoSystemState();
			if( state == LottoSystem::LOTTO_STATE_DRAWING )
				m_pDontUseMachineBox->SetVisibleSingle( FALSE );
			else
				m_pDontUseMachineBox->SetVisibleSingle( TRUE );
		}
	}
}

void CLottoMachineViewerUIComp::UpdateMachine( const LottoSystem::CLottoSystemClient* pLottoSystem
											  , int drawingNum )
{
	if( pLottoSystem )
		m_id = pLottoSystem->GetLottoSystemId();

	// 애니메이션 스타트;
	if( m_pViewer )
	{
		// 번호 텍스처 불러오기;
		std::string strTextureName = GetLottoNumTextureString( drawingNum-1 );
		if( strTextureName.length() )
		{
			m_textureRes = TextureManager::GetInstance().LoadTexture(
				strTextureName.c_str(),
				false,
				TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
				1,
				D3DFMT_UNKNOWN,
				TEXTURE_RESIZE_NONE,
				TEXTURE_UI,
				false,
				TRUE );
		}

		LPDIRECT3DTEXTUREQ pTexture = m_textureRes.GetTexture();

		// 번호 텍스처 세팅;
		m_pViewer->SetFrameMeshTexture( pTexture, 1 );

		// 애니메이션 시작;
		m_pViewer->ResetAnimation();
		m_pViewer->StartAnimation();
	}
}

// 애니메이션이 끝나면 실행;
void CLottoMachineViewerUIComp::AnimationTrigger()
{
	if( m_id != LottoSystem::LOTTO_NONE_ID )
	{
		LottoSystem::CLottoSystemClient* pLottoSystem =
			LottoSystem::GLLottoSystemManClient::Instance()->GetLottoSystem( m_id );
		if( pLottoSystem )
		{
			ILottoMainUI* pMainUI = m_pInterface->GetLottoMainUI();
			if( pMainUI )
				pMainUI->UpdateWinNum( pLottoSystem );
		}
	}
}

// 애니메이션 업데이트;
void CLottoMachineViewerUIComp::UpdateAnimation( float fElapsedTime )
{
	if( m_pViewer )
		m_pViewer->UpdateAnimation( fElapsedTime );
}