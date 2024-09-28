#include "stdafx.h"

#include "./UIViewFrameMeshComponent.h"

#include "../MiniGame/ColorProperty/ColorProperty.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/GUInterface/UIRenderQueue.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/DxTools/d3dfont.h"
#include "../../../EngineLib/DxTools/DxBackUpRendTarget.h"
#include "../../../EngineLib/DxMeshs/FrameMesh/DxFrameMesh.h"
#include "../../../EngineLib/DxMeshs/DxSimpleMeshMan.h"

#include "../../../RanLogic/GLogicEx.h"
#include "../../../RanLogic/GLUseFeatures.h"

#include "../../InnerInterface.h"

#include "../../../SigmaCore/DebugInclude.h"


//////////////////////////////////////////////////////////////////////////
//					CViewFrameMeshComponent::FRAME_MESH
CViewFrameMeshComponent::FRAME_MESH::~FRAME_MESH()
{
	SAFE_DELETE( pFrameMesh );
}

//////////////////////////////////////////////////////////////////////////
//						CViewFrameMeshComponent
CViewFrameMeshComponent::CViewFrameMeshComponent( CInnerInterface* pInterface
												 , EngineDeviceMan* pEngineDevice )
												 : CUIGroupHelper( pEngineDevice )
												 , m_pInterface( pInterface )
												 , m_pViewer( NULL )
												 , m_pd3dDevice( NULL )
												 , m_pTargetTexture( NULL )
												 , m_pTargetSurface( NULL )
												 , m_pTargetZBuffer( NULL )
												 , m_bUseMaterial( false )
												 , m_fov( 0.34f * D3DX_PI )	// Default
{
}

CViewFrameMeshComponent::~CViewFrameMeshComponent()
{

}

void CViewFrameMeshComponent::CreateSub( CUIControl* pParent
										, const char* szControlKeyword
										, WORD wAlignFlag
										, UIGUID WndID )
{
	CUIGroupHelper::CreateSub( pParent, szControlKeyword, wAlignFlag, WndID );
}

void CViewFrameMeshComponent::CreateSubControl( const char* szControlKeyword
											   , const char* szTextureKeyword )
{
	// 기본 Light 초기화;
	ZeroMemory( &m_Light[0], sizeof(D3DLIGHT9) );
	ZeroMemory( &m_Light[1], sizeof(D3DLIGHT9) );
	ZeroMemory( &m_Light[2], sizeof(D3DLIGHT9) );

	D3DXVECTOR3 _vLight0 = D3DXVECTOR3( -1.0f, -0.6f, 0.13f );
	D3DXVECTOR3 _vLight1 = D3DXVECTOR3( 1.0f, -0.1f, 0.2f );
	D3DXVECTOR3 _vLight2 = D3DXVECTOR3( 0.1f, 1.0f, 0.2f );
	D3DXVec3Normalize( &_vLight0, &_vLight0 );
	D3DXVec3Normalize( &_vLight1, &_vLight1 );
	D3DXVec3Normalize( &_vLight2, &_vLight2 );

	const float _fFactor( 1.0f / 255.0f );
	m_Light[0].Type = D3DLIGHT_DIRECTIONAL;
	m_Light[0].Direction = _vLight0;
	m_Light[0].Diffuse	= D3DXCOLOR( 255.0f * _fFactor, 255.0f * _fFactor, 255.0f * _fFactor, 1.0f );
	m_Light[0].Ambient	= D3DXCOLOR( 211.0f * _fFactor, 211.0f * _fFactor, 211.0f * _fFactor, 1.0f );
	m_Light[0].Specular	= D3DXCOLOR( 249.0f * _fFactor, 249.0f * _fFactor, 249.0f * _fFactor, 1.0f );

	m_Light[1].Type = D3DLIGHT_DIRECTIONAL;
	m_Light[1].Direction = _vLight1;
	m_Light[1].Diffuse	= D3DXCOLOR( 11.0f * _fFactor, 11.0f * _fFactor, 11.0f * _fFactor, 1.0f );
	m_Light[1].Ambient	= D3DXCOLOR( 11.0f * _fFactor, 11.0f * _fFactor, 11.0f * _fFactor, 1.0f );
	m_Light[1].Specular	= D3DXCOLOR( 129.0f * _fFactor, 129.0f * _fFactor, 129.0f * _fFactor, 1.0f );

	m_Light[2].Type = D3DLIGHT_DIRECTIONAL;
	m_Light[2].Direction = _vLight2;
	m_Light[2].Diffuse	= D3DXCOLOR( 0.0f * _fFactor, 0.0f * _fFactor, 0.0f * _fFactor, 1.0f );
	m_Light[2].Ambient	= D3DXCOLOR( 7.0f * _fFactor, 7.0f * _fFactor, 7.0f * _fFactor, 1.0f );
	m_Light[2].Specular	= D3DXCOLOR( 131.0f * _fFactor, 131.0f * _fFactor, 131.0f * _fFactor, 1.0f );

	ZeroMemory( &m_Material, sizeof(m_Material) );
	m_Material.Diffuse	= D3DXCOLOR( 255.0f * _fFactor, 255.0f * _fFactor, 255.0f * _fFactor, 1.0f );
	m_Material.Ambient	= D3DXCOLOR( 163.0f * _fFactor, 163.0f * _fFactor, 163.0f * _fFactor, 1.0f );
	m_Material.Specular	= D3DXCOLOR( 237.0f * _fFactor, 237.0f * _fFactor, 237.0f * _fFactor, 1.0f );
	m_Material.Emissive	= D3DXCOLOR( 0.0f * _fFactor, 0.0f * _fFactor, 0.0f * _fFactor, 1.0f );
	m_Material.Power = 8.8f;

	// Viewer;
	{
		m_pViewer = CreateLineBox( szControlKeyword, szTextureKeyword );
		m_pViewer->SetVisibleSingle(TRUE);
	}
}

void CViewFrameMeshComponent::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroupHelper::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CViewFrameMeshComponent::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroupHelper::TranslateUIMessage( ControlID, dwMsg );
}

void CViewFrameMeshComponent::SetVisibleSingle( BOOL bVisible )
{
	if( !GLUseFeatures::GetInstance().IsUsingColorProperty3D() )
		CUIGroupHelper::SetVisibleSingle( bVisible );
	else
	{
		IColorProperty* const pColorProperty(
			m_pInterface->GetColorPropertyWindow() );
		if( bVisible == TRUE )
		{
			if( IsVisible() == TRUE )
				return;

			m_pInterface->ToggleWindowColorProperty( true );

			if( pColorProperty )
			{	
				pColorProperty->bindLightProperty( &m_Light[0], 0 );
				pColorProperty->bindLightProperty( &m_Light[1], 1 );
				pColorProperty->bindLightProperty( &m_Light[2], 2 );

				pColorProperty->bindMaterialProperty( &m_Material );
			}
			
			CUIGroupHelper::SetVisibleSingle( TRUE );
		}
		else
		{
			CUIGroupHelper::SetVisibleSingle( FALSE );

			m_pInterface->ToggleWindowColorProperty( false );

			if( pColorProperty )
				pColorProperty->resetProperty();
		}
	}
}

void CViewFrameMeshComponent::CreateViewMatrix( const D3DXVECTOR3& vEye
											   , const D3DXVECTOR3& vAt )
{
	m_vAt = vAt;
	m_vEye = vEye;
}

HRESULT CViewFrameMeshComponent::InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr( S_OK );

	if( !pd3dDevice )
		return hr;

	m_pd3dDevice = pd3dDevice;

	V_RETURN( CUIGroupHelper::InitDeviceObjects( pd3dDevice ) );

	D3DXVECTOR3 vDir = m_vEye - m_vAt;
	D3DXVec3Normalize( &vDir, &vDir );

	::D3DXMatrixLookAtLH( &this->m_mView,
		&( m_vEye ),
		&m_vAt,
		&D3DXVECTOR3(0.0f, 1.0f, 0.0f) );

	float aspect = GetGlobalPos().sizeX/GetGlobalPos().sizeY;
	::D3DXMatrixPerspectiveFovLH(
		&this->m_mProjection, m_fov, aspect, 1.0f, 1000000.0f );

	for( unsigned int i=0; i<m_pMeshList.size(); ++i )
	{
		FRAME_MESH& frameMesh = m_pMeshList.at( i );
		SAFE_DELETE( frameMesh.pFrameMesh );
		frameMesh.pFrameMesh = new DxFrameMesh( FALSE );

		if( frameMesh.pFrameMesh )
		{
			frameMesh.pFrameMesh->SetFile( frameMesh.filePath.c_str(), frameMesh.fileName.c_str() );
			frameMesh.pFrameMesh->SetFVF ( VERTEX_WORLD::FVF );

			hr = frameMesh.pFrameMesh->Create ( pd3dDevice );
			if ( hr == S_OK )
			{
				frameMesh.pFrameMesh->UpdateFrames();
			}
			else
			{
				GASSERT ( 0 && "CViewFrameMeshComponent::InitDeviceObjects 사용중 특정파일이 없습니다." );
				SAFE_DELETE ( frameMesh.pFrameMesh );
			}

			//V_RETURN( frameMesh.pFrameMesh->InitDeviceObjects( pd3dDevice ) );
		}
	}

	return hr;
}

HRESULT CViewFrameMeshComponent::RestoreDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr( S_OK );

	if( !pd3dDevice )
		return hr;

	V_RETURN( CUIGroupHelper::RestoreDeviceObjects( pd3dDevice ) );

	SAFE_RELEASE(m_pTargetTexture);
	SAFE_RELEASE(m_pTargetSurface);
	SAFE_RELEASE(m_pTargetZBuffer);	

	LPDIRECT3DSURFACEQ pBackBuffer;
	if ( pd3dDevice->GetBackBuffer ( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer ) != D3D_OK )
		return E_FAIL;

	pBackBuffer->GetDesc( &this->m_d3dBackBufferDesc );
	pBackBuffer->Release();

	const UIRECT& uiRect = m_pViewer->GetGlobalPos();
	this->m_d3dBackBufferDesc.Width  = (UINT)uiRect.sizeX;
	this->m_d3dBackBufferDesc.Height = (UINT)uiRect.sizeY;

	V_RETURN( pd3dDevice->CreateDepthStencilSurface(
		m_d3dBackBufferDesc.Width, m_d3dBackBufferDesc.Height, 
		D3DFMT_D16, D3DMULTISAMPLE_NONE,
		0, TRUE, &m_pTargetZBuffer, NULL ) );

	V_RETURN( pd3dDevice->CreateTexture(
		m_d3dBackBufferDesc.Width, m_d3dBackBufferDesc.Height, 1, 
		D3DUSAGE_RENDERTARGET,
		D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_pTargetTexture, NULL ) );

	V_RETURN( m_pTargetTexture->GetSurfaceLevel( 0, &this->m_pTargetSurface ) );

	for( unsigned int i=0; i<m_pMeshList.size(); ++i )
	{
		FRAME_MESH& frameMesh = m_pMeshList.at( i );
		if( frameMesh.pFrameMesh )
			V_RETURN( frameMesh.pFrameMesh->RestoreDeviceObjects( pd3dDevice ) );
	}

	return hr;
}

HRESULT CViewFrameMeshComponent::InvalidateDeviceObjects()
{
	HRESULT hr( S_OK );

	V_RETURN( CUIGroupHelper::InvalidateDeviceObjects() );

	SAFE_RELEASE( m_pTargetSurface );
	SAFE_RELEASE( m_pTargetZBuffer );
	SAFE_RELEASE( m_pTargetTexture );

	for( unsigned int i=0; i<m_pMeshList.size(); ++i )
	{
		FRAME_MESH& frameMesh = m_pMeshList.at( i );
		if( frameMesh.pFrameMesh && m_pd3dDevice )
			V_RETURN( frameMesh.pFrameMesh->InvalidateDeviceObjects( m_pd3dDevice ) );
	}

	return hr;
}

HRESULT CViewFrameMeshComponent::DeleteDeviceObjects()
{
	HRESULT hr( S_OK );

	V_RETURN( CUIGroupHelper::DeleteDeviceObjects() );

	ClearFrameMesh();

	return hr;
}

HRESULT CViewFrameMeshComponent::Render( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr( S_OK );

	RenderState renderState;

	DxBackupTarget sBackupTarget;
	sBackupTarget.Backup( pd3dDevice );

	SetRenderState( pd3dDevice, renderState );	
	RenderItem( pd3dDevice );
	RecoverRenderState( pd3dDevice, renderState );
	sBackupTarget.Restore();

	V_RETURN( CUIGroupHelper::Render( pd3dDevice ) );

	RenderViewer( pd3dDevice );

	return hr;
}

void CViewFrameMeshComponent::SetRenderState( LPDIRECT3DDEVICEQ pd3dDevice, RenderState& renderState )
{
	static float fBias = -0.001f;

	pd3dDevice->GetMaterial( &renderState.Material );
	pd3dDevice->GetLight( 0, &renderState.Light[0] );
	pd3dDevice->GetLight( 1, &renderState.Light[1] );
	pd3dDevice->GetLight( 2, &renderState.Light[2] );

	pd3dDevice->GetRenderState( D3DRS_ZFUNC, &renderState.dwZFunc );
	pd3dDevice->GetRenderState( D3DRS_ZENABLE, &renderState.dwZEnable );
	pd3dDevice->GetRenderState( D3DRS_ZWRITEENABLE, &renderState.dwZWriteEnable );
	pd3dDevice->GetRenderState( D3DRS_DEPTHBIAS, &renderState.dwBiasPrev );
	pd3dDevice->GetRenderState( D3DRS_LIGHTING, &renderState.dwLight );
	pd3dDevice->GetRenderState( D3DRS_NORMALIZENORMALS, &renderState.dwNormalize );
	pd3dDevice->GetRenderState( D3DRS_SPECULARENABLE, &renderState.dwSpecular );

	pd3dDevice->GetSamplerState(0, D3DSAMP_MAGFILTER, &renderState.dwMagFilter );	
	pd3dDevice->GetSamplerState(0, D3DSAMP_MINFILTER, &renderState.dwMinFilter );	

	pd3dDevice->GetTransform( D3DTS_VIEW, &renderState.mViewPREV );
	pd3dDevice->GetTransform( D3DTS_PROJECTION, &renderState.mProjPREV );

	pd3dDevice->SetRenderTarget( 0, this->m_pTargetSurface );
	pd3dDevice->SetDepthStencilSurface( this->m_pTargetZBuffer );

	pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0x00000000L, 1.0f, 0x0L );	

	pd3dDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESSEQUAL );
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, true );
	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, true );
	pd3dDevice->SetRenderState( D3DRS_DEPTHBIAS, *((DWORD*)&fBias) );
	pd3dDevice->SetRenderState( D3DRS_LIGHTING, true );
	pd3dDevice->SetRenderState( D3DRS_NORMALIZENORMALS, true );
	pd3dDevice->SetRenderState( D3DRS_SPECULARENABLE, true );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );	
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );	
	pd3dDevice->SetLight( 0, &m_Light[0] );
	pd3dDevice->SetLight( 1, &m_Light[1] );
	pd3dDevice->SetLight( 2, &m_Light[2] );
	pd3dDevice->LightEnable( 0, true );
	pd3dDevice->LightEnable( 1, true );
	pd3dDevice->LightEnable( 2, true );

	pd3dDevice->SetTransform( D3DTS_VIEW, &this->m_mView );
	pd3dDevice->SetTransform( D3DTS_PROJECTION, &this->m_mProjection );
}

void CViewFrameMeshComponent::RenderItem( LPDIRECT3DDEVICEQ pd3dDevice )
{
	for( unsigned int i=0; i<m_pMeshList.size(); ++i )
	{
		FRAME_MESH& frameMesh = m_pMeshList.at( i );
		if( frameMesh.pFrameMesh && frameMesh.pFrameMesh->GetFrameRoot() )
		{
			if( m_bUseMaterial )
			{
				frameMesh.pFrameMesh->Render_Independence(
					pd3dDevice, frameMesh.pFrameMesh->GetFrameRoot(), &m_Material, frameMesh.pTexture );
			}
			else
			{
				frameMesh.pFrameMesh->Render_Independence(
					pd3dDevice, frameMesh.pFrameMesh->GetFrameRoot(), NULL, frameMesh.pTexture );
			}
		}
	}
}

void CViewFrameMeshComponent::RecoverRenderState( LPDIRECT3DDEVICEQ pd3dDevice, const RenderState& renderState )
{
	pd3dDevice->SetMaterial( &renderState.Material );
	pd3dDevice->SetLight( 0, &renderState.Light[0] );
	pd3dDevice->SetLight( 1, &renderState.Light[1] );
	pd3dDevice->SetLight( 2, &renderState.Light[2] );
	pd3dDevice->LightEnable( 0, true );
	pd3dDevice->LightEnable( 1, false );
	pd3dDevice->LightEnable( 2, false );

	pd3dDevice->SetRenderState( D3DRS_ZFUNC, renderState.dwZFunc );
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, renderState.dwZEnable );
	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, renderState.dwZWriteEnable );
	pd3dDevice->SetRenderState( D3DRS_DEPTHBIAS, renderState.dwBiasPrev );
	pd3dDevice->SetRenderState( D3DRS_LIGHTING, renderState.dwLight );
	pd3dDevice->SetRenderState( D3DRS_NORMALIZENORMALS, renderState.dwNormalize );
	pd3dDevice->SetRenderState( D3DRS_SPECULARENABLE, renderState.dwSpecular );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, renderState.dwMagFilter );	
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, renderState.dwMinFilter );		

	pd3dDevice->SetTransform( D3DTS_VIEW, &renderState.mViewPREV );
	pd3dDevice->SetTransform( D3DTS_PROJECTION, &renderState.mProjPREV );
}

const HRESULT CViewFrameMeshComponent::RenderViewer( LPDIRECT3DDEVICEQ pd3dDevice )
{
	UIVERTEX* pUIVertex = m_pViewer->GetUIVertex();

	::memcpy( m_targetUIVertex, pUIVertex, sizeof(m_UIVertex) );

	const D3DXVECTOR2 vRePos[4] = {
		m_UIVertex[0].GetPos(),
		m_UIVertex[1].GetPos()
	};

	m_targetUIVertex[0].SetPos( vRePos[0] );
	m_targetUIVertex[1].SetPos( vRePos[1] );
	m_targetUIVertex[0].SetTexturePos( 0.0f, 0.0f );
	m_targetUIVertex[1].SetTexturePos( 1.0f, 0.0f );

	m_targetUIVertex[2].SetTexturePos( 1.0f, 1.0f );
	m_targetUIVertex[3].SetTexturePos( 0.0f, 1.0f );

	if( CUIRenderQueue::Get()->Render( m_pTargetTexture, m_targetUIVertex ) )
		return S_OK;

	return E_FAIL;
}

// Update Animation;
void CViewFrameMeshComponent::UpdateAnimation( float fElapsedTime )
{
	for( unsigned int i=0; i<m_pMeshList.size(); ++i )
	{
		FRAME_MESH& frameMesh = m_pMeshList.at( i );

		if( frameMesh.pFrameMesh )
		{
			// 애니메이션 딜레이 상태이면;
			for( unsigned int j=0; j<m_pStopAnimationList.size(); ++j )
			{
				STOP_ANIMATION& stopAnimation = m_pStopAnimationList.at( j );
				if( stopAnimation.idx == i )
				{
					if( stopAnimation.bCurFrameDelay &&
						stopAnimation.bDelay )
					{
						stopAnimation.fTimeDelta += fElapsedTime;
						if( stopAnimation.fTimeDelta >=
							stopAnimation.fDelayTime )
						{
							stopAnimation.bDelay = false;
							stopAnimation.fTimeDelta = 0.f;
						}
					}
				}
			}

			// 애니메이션 중이면;
			if( frameMesh.bAnimation )
			{
				bool bDelayAnimation = false;
				for( unsigned int j=0; j<m_pStopAnimationList.size(); ++j )
				{
					STOP_ANIMATION& stopAnimation = m_pStopAnimationList.at( j );
					if( stopAnimation.idx == i )
					{
						if( frameMesh.pFrameMesh->GetCurTime() >=
							stopAnimation.fTime*UNITANIKEY_PERSEC &&
							!stopAnimation.bCurFrameDelay )
						{
							frameMesh.pFrameMesh->SetCurTime( stopAnimation.fTime * UNITANIKEY_PERSEC );
							frameMesh.pFrameMesh->FrameMove( 0.f, 0.f );

							stopAnimation.bDelay = true;
							stopAnimation.bCurFrameDelay = true;
						}

						bDelayAnimation = stopAnimation.bDelay;
					}
				}

				if( !bDelayAnimation )
				{
					if( ( frameMesh.pFrameMesh->GetCurTime() >=
						frameMesh.fAnimationTime*UNITANIKEY_PERSEC ) &&
						frameMesh.bAnimation )
					{
						frameMesh.bAnimation = false;

						frameMesh.pFrameMesh->SetCurTime( frameMesh.fAnimationTime * UNITANIKEY_PERSEC );
						frameMesh.pFrameMesh->FrameMove( 0.f, 0.f );

						// 애니메이션이 끝날때 트리거 실행;
						for( unsigned int j=0; j<m_callbackFuncs.size(); ++j )
						{
							FunctionVoid fnConfirm = m_callbackFuncs.at( j );
							if( fnConfirm )
								fnConfirm();
						}

						//m_pGaeaClient->PrintConsoleText( "애니메이션 종료 트리거 실행!");
					}
					else
						frameMesh.pFrameMesh->FrameMove( 0.f, fElapsedTime );
				}
			}

			// 애니메이션 진행 상태가 아니라면;
			if( !frameMesh.bAnimation )
			{
				// 애니메이션 0초 상태로 초기화;
				frameMesh.pFrameMesh->SetAnimationTimeZero();

				// 정지 애니메이션 초기화;
				for( unsigned int j=0; j<m_pStopAnimationList.size(); ++j )
				{
					STOP_ANIMATION& stopAnimation = m_pStopAnimationList.at( j );
					if( stopAnimation.idx == i )
					{
						stopAnimation.fTimeDelta = 0.f;
						stopAnimation.bDelay = false;
						stopAnimation.bCurFrameDelay = false;
					}
				}
			}

			frameMesh.pFrameMesh->UpdateFrames();
		}
	}
}

void CViewFrameMeshComponent::AddFrameMesh( const std::string& fileName )
{
	FRAME_MESH frameMesh;
	frameMesh.filePath = DxSimpleMeshMan::GetInstance().GetPath();
	frameMesh.fileName = fileName;

	m_pMeshList.push_back( frameMesh );
}

void CViewFrameMeshComponent::ClearFrameMesh()
{
	for( unsigned int i=0; i<m_pMeshList.size(); ++i )
	{
		FRAME_MESH& frameMesh = m_pMeshList.at( i );
		if( frameMesh.pFrameMesh )
		{
			frameMesh.pFrameMesh->DeleteDeviceObjects();
			frameMesh.pFrameMesh->FinalCleanup();
		}

		SAFE_DELETE( frameMesh.pFrameMesh );
	}

	m_pMeshList.clear();
}

void CViewFrameMeshComponent::SetFrameMeshTexture( LPDIRECT3DTEXTUREQ pTexture, int idx )
{
	for( unsigned int i=0; i<m_pMeshList.size(); ++i )
	{
		FRAME_MESH& frameMesh = m_pMeshList.at( i );
		if( frameMesh.pFrameMesh && ( ( i == idx ) || ( idx == -1 ) ) )
		{
			frameMesh.pTexture = pTexture;
		}
	}
}

void CViewFrameMeshComponent::ResetAnimation( int idx )
{
	for( unsigned int i=0; i<m_pMeshList.size(); ++i )
	{
		FRAME_MESH& frameMesh = m_pMeshList.at( i );
		if( frameMesh.pFrameMesh && ( ( i == idx ) || ( idx == -1 ) ) )
		{
			frameMesh.pFrameMesh->SetCurTime( 0.f );
			frameMesh.bAnimation = false;
		}
	}

	for( unsigned int i=0; i<m_pStopAnimationList.size(); ++i )
	{
		STOP_ANIMATION& stopAnimation = m_pStopAnimationList.at( i );
		stopAnimation.bDelay = false;
		stopAnimation.bCurFrameDelay = false;
		stopAnimation.fTimeDelta = 0.f;
	}
}

void CViewFrameMeshComponent::StartAnimation( int idx )
{
	for( unsigned int i=0; i<m_pMeshList.size(); ++i )
	{
		FRAME_MESH& frameMesh = m_pMeshList.at( i );
		if( frameMesh.pFrameMesh && ( ( i == idx ) || ( idx == -1 ) ) )
		{
			frameMesh.pFrameMesh->SetCurTime( 0.f );
			frameMesh.bAnimation = true;
		}
	}
}

void CViewFrameMeshComponent::SetAnimationTime( float fTime, int idx )
{
	for( unsigned int i=0; i<m_pMeshList.size(); ++i )
	{
		FRAME_MESH& frameMesh = m_pMeshList.at( i );
		if( ( i == idx ) || ( idx == -1 ) )
			frameMesh.fAnimationTime = fTime;
	}
}

void CViewFrameMeshComponent::AddStopAnimationTime( float fTime, float fDelayTime, int idx )
{
	STOP_ANIMATION stopAnimation;
	stopAnimation.fTime = fTime;
	stopAnimation.fDelayTime = fDelayTime;
	stopAnimation.idx = idx;

	m_pStopAnimationList.push_back( stopAnimation );
}

void CViewFrameMeshComponent::SetLight( const D3DLIGHT9& light, int idx )
{
	if( idx >= 0 && idx < 3 )
		m_Light[ idx ] = light;
}

void CViewFrameMeshComponent::SetMaterial( const D3DMATERIAL9& material )
{
	m_Material = material;
}