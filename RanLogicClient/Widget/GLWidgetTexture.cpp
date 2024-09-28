#include "pch.h"
#include "GLWidgetTexture.h"

#include "../../enginelib/Common/StlFunctions.h"

#include "../GLGaeaClient.h"
#include "../GfxInterfaceBase.h"
#include "../Char/GLCharacter.h"

#include "./GLWidgetTextureChar.h"
#include "./GLWidgetTextureCharVehicle.h"
#include "./GLWidgetTextureDice.h"
#include "./GLWidgetTextureItemPreview.h"

GLWidgetTexture::GLWidgetTexture( LPDIRECT3DDEVICEQ pDeivce )
:	GfxRenderTexture()
,	m_pDevice( pDeivce )
,	m_pSurface( NULL )
,	m_pTexture( NULL )
,	m_dwClearColor( 0 )
{
}

GLWidgetTexture::~GLWidgetTexture(void)
{
	Clear();
}

void GLWidgetTexture::Clear()
{
	SAFE_RELEASE( m_pTexture );
	SAFE_RELEASE( m_pSurface );
}

void GLWidgetTexture::OnResetDevice()
{
	GfxRenderTexture::OnResetDevice();

	OnCreate();
	ReplaceGfxTexture();
}

void GLWidgetTexture::OnLostDevice()
{
	GfxRenderTexture::OnLostDevice();

	Clear(); 
}

bool GLWidgetTexture::OnCreate()
{
	HRESULT hr;

	InitTextureSize();
	InitClearColor();

	hr = m_pDevice->CreateTexture( m_nRenderTextureHeight, m_nRenderTextureHeight, 0, 
			D3DUSAGE_RENDERTARGET | D3DUSAGE_AUTOGENMIPMAP, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT,
			&m_pTexture, NULL );
	if( FAILED( hr ) )
		return false;

	hr = m_pDevice->CreateDepthStencilSurface( m_nRenderTextureHeight, m_nRenderTextureHeight,
			D3DFMT_D24S8, D3DMULTISAMPLE_NONE, 0, TRUE, &m_pSurface, 0 );
	if( FAILED( hr ) )
		return false;

	hr = CreateGfxTexture( m_pTexture );
	if( FAILED( hr ) )
		return false;

	return true;
}

void GLWidgetTexture::OnFrameMove( float fElapsedTime )
{
}

void GLWidgetTexture::OnFrameRender()
{
	if( 0 >= m_nRefCount )
		return;

	if( NULL == m_pTexture || NULL == m_pSurface )
		return;

	LPDIRECT3DSURFACEQ pOldSurface = NULL;
	LPDIRECT3DSURFACEQ pOldDepthSurface = NULL;
	LPDIRECT3DSURFACEQ pSurface = NULL;

	m_pDevice->GetRenderTarget( 0, &pOldSurface );
	m_pDevice->GetDepthStencilSurface( &pOldDepthSurface );

	m_pTexture->GetSurfaceLevel( 0, &pSurface );
	if( !FAILED( m_pDevice->SetRenderTarget( 0, pSurface ) ) )
		m_pDevice->SetDepthStencilSurface( m_pSurface );

	int nTextureWidth = 0;
	int nTextureHeight = 0;
	GetGfxTextureSize( nTextureWidth, nTextureHeight );

	D3DVIEWPORTQ vp = { 0, 0, nTextureWidth, nTextureHeight, 0.0f, 1.0f };
	m_pDevice->SetViewport( &vp );

	if( FAILED( m_pDevice->Clear( 0, NULL,
		D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 
		m_dwClearColor, 1.0f, 0 ) ) )
		return;

	//if( FAILED( m_pDevice->BeginScene() ) )
	//	return;

	OnRender();

	//m_pDevice->EndScene();

	m_pDevice->SetRenderTarget( 0, pOldSurface );
	m_pDevice->SetDepthStencilSurface( pOldDepthSurface );

	SAFE_RELEASE( pOldSurface );
	SAFE_RELEASE( pOldDepthSurface );
	SAFE_RELEASE( pSurface );
}

void GLWidgetTexture::InitTextureSize()
{
	m_nRenderTextureWidth = 1024;
	m_nRenderTextureHeight = 1024;
}

void GLWidgetTexture::InitClearColor()
{
	m_dwClearColor = D3DCOLOR_ARGB( 0, 0, 0, 0 );
}

//---------------------------------------------------------------------------------
GLWidgetTextureMan::GLWidgetTextureMan(void)
:	m_pDevice( NULL )
{
}

GLWidgetTextureMan::GLWidgetTextureMan( const GLWidgetTextureMan& value )
{
}

GLWidgetTextureMan::~GLWidgetTextureMan(void)
{
}

GLWidgetTextureMan& GLWidgetTextureMan::GetInstance()
{
	static GLWidgetTextureMan Instance;
	return Instance;
}

void GLWidgetTextureMan::OnCreateDevice( LPDIRECT3DDEVICEQ pDevice )
{
	m_pDevice = pDevice;
}

void GLWidgetTextureMan::OnResetDevice()
{
	MAP_WIDGET_TEXTURE_ITER iter = m_mapWidgetTexture.begin();
	for( ; iter != m_mapWidgetTexture.end(); ++iter )
		iter->second->OnResetDevice();
}

void GLWidgetTextureMan::OnLostDevice()
{
	MAP_WIDGET_TEXTURE_ITER iter = m_mapWidgetTexture.begin();
	for( ; iter != m_mapWidgetTexture.end(); ++iter )
		iter->second->OnLostDevice();
}

void GLWidgetTextureMan::OnDestroyDevice()
{
	std::for_each( m_mapWidgetTexture.begin(), m_mapWidgetTexture.end(), std_afunc::DeleteMapObject() );
	m_mapWidgetTexture.clear();
}

void GLWidgetTextureMan::OnFrameMove( float fElapsedTime )
{
	MAP_WIDGET_TEXTURE_ITER iter = m_mapWidgetTexture.begin();
	for( ; iter != m_mapWidgetTexture.end(); ++iter )
	{
		GLWidgetTexture* pTexture = iter->second;
		if( 0 < pTexture->GetRefCount() )
			pTexture->OnFrameMove( fElapsedTime );
	}
}

void GLWidgetTextureMan::OnFrameRender()
{
	MAP_WIDGET_TEXTURE_ITER iter = m_mapWidgetTexture.begin();
	for( ; iter != m_mapWidgetTexture.end(); ++iter )
	{
		GLWidgetTexture* pTexture = iter->second;
		if( 0 < pTexture->GetRefCount() )
			pTexture->OnFrameRender();
	}
}

void GLWidgetTextureMan::CleanUp()
{
	MAP_WIDGET_TEXTURE_ITER iter = m_mapWidgetTexture.begin();
	for( ; iter != m_mapWidgetTexture.end(); ++iter )
	{
		GLWidgetTexture* pTexture = iter->second;
		pTexture->CleanUp();
	}
}

GfxRenderTexture* GLWidgetTextureMan::CreateTexture( EMWIDGET_TEXTURE emTexture )
{
	// 이미 생성되어 있는지 확인
	MAP_WIDGET_TEXTURE_ITER iter = m_mapWidgetTexture.find( emTexture );
	if( iter != m_mapWidgetTexture.end() )
	{
		// 레퍼런스 카운트 증가
		iter->second->IncreaseRefCount();
		return iter->second;
	}

	GLWidgetTexture* pWidgetTexture = NULL;

	// 텍스처 생성
	switch( emTexture )
	{
		/*
	case EMWIDGET_TEXTURE_CHARINFO :
		{
			GLWidgetTextureChar* pTexture = new GLWidgetTextureChar( m_pDevice );

			if ( NULL == pTexture )
				break;

			GLCharacter* pChar = GfxInterfaceBase::GetInstance()->GetGaeaClient()->GetCharacter();

			if ( NULL == pChar )
				break;

			//pTexture->SetAni(AN_BIKE_C, AN_SUB_ONEHSWORD);
			pTexture->SetSkinChar( pChar );
			pWidgetTexture = pTexture;
		}
		break;
	case EMWIDGET_TEXTURE_CHAR_VEHICLEINFO :
		{
			GLWidgetTextureCharVehicle* pTexture = new GLWidgetTextureCharVehicle( m_pDevice );

			if ( NULL == pTexture )
				break;

			GLCharacter* pChar = GfxInterfaceBase::GetInstance()->GetGaeaClient()->GetCharacter();

			if ( NULL == pChar )
				break;

			SITEM* pItem = NULL;
			SNATIVEID sID(161,61);
			pItem = GLogicData::GetInstance().GetItem ( sID.dwID );

			GLWidgetTextureCharVehicle::VehicleData vehicleData;
			vehicleData.VehicleID = sID;

			if( GetVehicleType(sID) == VEHICLE_TYPE_BOARD )
				pTexture->SetAni(AN_PLACID, AN_SUB_HOVERBOARD );
			else
				pTexture->SetAni(GetMainAniType(sID), AN_SUB_DRIVER_WALK);

			pTexture->SetSkinChar( pChar );

			SNATIVEID curMyVehicleId = pChar->GET_SLOT_ITEM(SLOT_VEHICLE).Id();
			if( vehicleData.VehicleID == curMyVehicleId )
			{
				for (int i = 0; i < ACCE_TYPE_SIZE; ++i)
				{
					vehicleData.Parts[i] = pChar->VehiclePutOnItem(VEHICLE_ACCESSORYTYPE(i));
					vehicleData.ColorArray[i].dwMainColor = pChar->VehicleColorMain(VEHICLE_ACCESSORYTYPE(i));
					vehicleData.ColorArray[i].dwSubColor = pChar->VehicleColorSub(VEHICLE_ACCESSORYTYPE(i));
				}
			}

			pTexture->SetSkinVehicle( vehicleData );

			pWidgetTexture = pTexture;
		}
		break;*/

	case EMWIDGET_TEXTURE_DICE:
		{
			WidgetTexture::Dice* _pTexture(new WidgetTexture::Dice(m_pDevice));
			if ( _pTexture == NULL )
				break;

			MiniGame::GLManagerClient* const _pManagerMiniGame(GfxInterfaceBase::GetInstance()->GetGaeaClient()->GetMiniGameManager());
			_pTexture->setManagerMiniGame(_pManagerMiniGame);

			pWidgetTexture = _pTexture;
		}
		break;

	case EMWIDGET_TEXTURE_ITEM_PREVIEW :
		{
			GLWidgetTextureItemPreview* pTexture = new GLWidgetTextureItemPreview( m_pDevice, GfxInterfaceBase::GetInstance()->GetGaeaClient()->GetItemPreviewClient() );
			if ( NULL == pTexture )
				break;

			GLCharacter* pChar = GfxInterfaceBase::GetInstance()->GetGaeaClient()->GetCharacter();
			if ( NULL == pChar )
				break;

			pTexture->SetSkinChar( pChar );
			pWidgetTexture = pTexture;
		}
		break;
	default :
		return NULL;
	}

	if( NULL == pWidgetTexture )
		return NULL;

	if( false == pWidgetTexture->OnCreate() )
	{
		SAFE_DELETE( pWidgetTexture );
		return NULL;
	}

	// 레퍼런스 카운트 증가
	pWidgetTexture->IncreaseRefCount();
	m_mapWidgetTexture.insert( std::make_pair( emTexture, pWidgetTexture ) );

	return pWidgetTexture;
}

GfxRenderTexture* GLWidgetTextureMan::FindTexture( EMWIDGET_TEXTURE emTexture )
{
	MAP_WIDGET_TEXTURE_ITER iter = m_mapWidgetTexture.find( emTexture );
	if( iter == m_mapWidgetTexture.end() )
		return NULL;

	return iter->second;
}