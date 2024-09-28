#include "stdafx.h"

#include "../../../EngineLib/GUInterface/UIRenderQueue.h"
#include "../../../EngineLib/DxTools/DxSurfaceTex.h"

#include "./UIMeshRender.h"
#include "./UIMeshData_SkinChar.h"
#include "./UIMeshData_SkinVehicle.h"

#include "../../../EngineLib/DxTools/Collision.h"
#include "../../../EngineLib/DxEffect/Single/DxEffSingleMan.h"


// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------
namespace MESHRDATAFUNC
{
    class DESTRUCTOR
    {
    public:
        void operator () ( CUIMeshRender::IUIMESHDATAPAIR pair )
        {
            delete pair.second;
        }
    };

    class INITDEVICEOBJECTS
    {
    public:
        INITDEVICEOBJECTS ( LPDIRECT3DDEVICEQ pd3dDevice ) : m_pd3dDevice (pd3dDevice) {}

        void operator () ( CUIMeshRender::IUIMESHDATAPAIR pair )
        {
            (pair.second)->InitDeviceObjects(m_pd3dDevice);
        }

        LPDIRECT3DDEVICEQ m_pd3dDevice;
    };

    class RESTOREDEVICEOBJECTS
    {
    public:
        RESTOREDEVICEOBJECTS ( LPDIRECT3DDEVICEQ pd3dDevice ) : m_pd3dDevice (pd3dDevice) {}

        void operator () ( CUIMeshRender::IUIMESHDATAPAIR pair )
        {
            (pair.second)->RestoreDeviceObjects(m_pd3dDevice);
        }

        LPDIRECT3DDEVICEQ m_pd3dDevice;
    };

    class INVALIDATEDEVICEOBJECTS
    {
    public:
        void operator () ( CUIMeshRender::IUIMESHDATAPAIR pair )
        {
            (pair.second)->InvalidateDeviceObjects();
        }
    };

    class DELETEDEVICEOBJECTS
    {
    public:
        void operator () ( CUIMeshRender::IUIMESHDATAPAIR pair )
        {
            (pair.second)->DeleteDeviceObjects();
        }        
    };

    class FRAMEMOVE
    {
    public:
        FRAMEMOVE ( LPDIRECT3DDEVICEQ pd3dDevice, const float fElapsedTime ) : m_pd3dDevice (pd3dDevice), m_fElapsedTime (fElapsedTime) {}

        void operator () ( CUIMeshRender::IUIMESHDATAPAIR pair )
        {
            (pair.second)->FrameMove(m_pd3dDevice, m_fElapsedTime);
        }

        LPDIRECT3DDEVICEQ m_pd3dDevice;
        FLOAT             m_fElapsedTime;
    };

    class RENDER
    {
    public:
        RENDER ( LPDIRECT3DDEVICEQ pd3dDevice ) : m_pd3dDevice (pd3dDevice) {}

        void operator () ( CUIMeshRender::IUIMESHDATAPAIR pair )
        {
            (pair.second)->Render(m_pd3dDevice);
        }

        LPDIRECT3DDEVICEQ m_pd3dDevice;
    };
}

// ----------------------------------------------------------------------------

CUIMeshRender::CUIMeshRender(EngineDeviceMan* pEngineDevice)
    : CUIControl(pEngineDevice)
    , m_pd3dDevice      (NULL)
    //, m_pTargetSurface  (NULL)
    //, m_pTargetTexture  (NULL)
    //, m_pTargetZBuf     (NULL)
    , m_pSavedSB(NULL)
    , m_pDrawSB(NULL)
{
}

CUIMeshRender::~CUIMeshRender(void)
{
    std::for_each(m_MeshMap.begin(), m_MeshMap.end(), MESHRDATAFUNC::DESTRUCTOR());
    m_MeshMap.clear();

    CUIMeshRender::InvalidateDeviceObjects ();
    CUIMeshRender::DeleteDeviceObjects ();
}

HRESULT CUIMeshRender::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
    if ( m_pd3dDevice == pd3dDevice )
    {
        return S_OK;
    }

    m_pd3dDevice = pd3dDevice;

    HRESULT hr(S_OK);

    V_RETURN( CUIControl::InitDeviceObjects( pd3dDevice ) ); 

    std::for_each(m_MeshMap.begin(), m_MeshMap.end(), MESHRDATAFUNC::INITDEVICEOBJECTS(pd3dDevice));

    return S_OK;
}

HRESULT CUIMeshRender::RestoreDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice )
{
    // Note : 불려지는 시점에 대해서 알아봐야함

    HRESULT hr(S_OK);

    V_RETURN(CUIControl::RestoreDeviceObjects( pd3dDevice )); 

    // 저장용 상태
    pd3dDevice->CreateStateBlock( D3DSBT_ALL, &m_pSavedSB );

    // 렌더용 상태
    pd3dDevice->BeginStateBlock();	
    {
        pd3dDevice->SetRenderState( D3DRS_ZENABLE,		TRUE );
        pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,	TRUE );
        pd3dDevice->SetRenderState( D3DRS_ZFUNC,	    D3DCMP_LESSEQUAL );
    }  
    pd3dDevice->EndStateBlock( &m_pDrawSB );

    // 텍스쳐&서페이스 생성
    //V_RETURN( CreateTargetTexture (pd3dDevice) );

    std::for_each(m_MeshMap.begin(), m_MeshMap.end(), MESHRDATAFUNC::RESTOREDEVICEOBJECTS(pd3dDevice));

    return S_OK;
}

HRESULT CUIMeshRender::InvalidateDeviceObjects()
{
    // Note : 불려지는 시점에 대해서 알아봐야함

    HRESULT hr(S_OK);

    V_RETURN(CUIControl::InvalidateDeviceObjects());
    
    //SAFE_RELEASE(m_pTargetSurface );
    //SAFE_RELEASE(m_pTargetZBuf);
    //SAFE_RELEASE(m_pTargetTexture);
    SAFE_RELEASE(m_pSavedSB);
    SAFE_RELEASE(m_pDrawSB);

    std::for_each(m_MeshMap.begin(), m_MeshMap.end(), MESHRDATAFUNC::INVALIDATEDEVICEOBJECTS());

    return S_OK;
}

HRESULT CUIMeshRender::DeleteDeviceObjects()
{
    // Note : 불려지는 시점에 대해서 알아봐야함

    HRESULT hr(S_OK);

    V_RETURN( CUIControl::DeleteDeviceObjects() );

    std::for_each(m_MeshMap.begin(), m_MeshMap.end(), MESHRDATAFUNC::DELETEDEVICEOBJECTS());

    return S_OK;
}

HRESULT CUIMeshRender::CreateTargetTexture ( LPDIRECT3DDEVICEQ pd3dDevice )
{
    //GASSERT( pd3dDevice );

    //SAFE_RELEASE(m_pTargetSurface);
    //SAFE_RELEASE(m_pTargetZBuf);
    //SAFE_RELEASE(m_pTargetTexture);

    //HRESULT hr(S_OK);

    //LPDIRECT3DSURFACEQ pBackBuffer;
    //hr = pd3dDevice->GetBackBuffer ( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );

    //pBackBuffer->GetDesc( &m_descBackBuffer );

    //if ( hr != D3D_OK )
    //{
    //    return E_FAIL;
    //}

    //pBackBuffer->Release();

    //m_descBackBuffer.Width  = 512;//(UINT)m_rcGlobalPos.sizeX*2;
    //m_descBackBuffer.Height = 512;//(UINT)m_rcGlobalPos.sizeY*2;

    //// Note : 최종 타겟 생성.
    //D3DFORMAT emMainFormat(m_descBackBuffer.Format);
    //D3DFORMAT emFormat(D3DFMT_A8R8G8B8);

    //emFormat = D3DFMT_A8R8G8B8;
    //if( FAILED ( pd3dDevice->CreateTexture( m_descBackBuffer.Width, m_descBackBuffer.Height, 1, D3DUSAGE_RENDERTARGET,		// 1. D3DFMT_A8R8G8B8
    //    emFormat, D3DPOOL_DEFAULT, &m_pTargetTexture, NULL ) ) )	
    //{
    //    emFormat = emMainFormat;
    //    hr = pd3dDevice->CreateTexture( m_descBackBuffer.Width, m_descBackBuffer.Height, 1, D3DUSAGE_RENDERTARGET,			// 2. 현재 Format
    //        emFormat, D3DPOOL_DEFAULT, &m_pTargetTexture, NULL );
    //    if( FAILED(hr) )
    //    {
    //        return E_FAIL;
    //    }
    //}

    //V_RETURN ( m_pTargetTexture->GetSurfaceLevel( 0, &m_pTargetSurface ) );

    //V_RETURN ( pd3dDevice->CreateDepthStencilSurface( m_descBackBuffer.Width, m_descBackBuffer.Height, D3DFMT_D16, D3DMULTISAMPLE_NONE,
    //                                                        0, TRUE, &m_pTargetZBuf, NULL ) );

    return S_OK;
}

HRESULT CUIMeshRender::Render( LPDIRECT3DDEVICEQ pd3dDevice )
{
    GASSERT( pd3dDevice );

    HRESULT hr(S_OK);

    if ( !IsVisible() )
    {
        return S_OK;
    }

    CUIControl::Render (pd3dDevice);
    CUIRenderQueue::Get()->RenderImmediately();



	//BOOL backupCollision = COLLISION::bCollisionCheck;
	//COLLISION::bCollisionCheck = FALSE;

	//DxLandMan* pBackupLand = DxEffSingleMan::GetInstance().GetLandMan();
	//DxEffSingleMan::GetInstance().SetLandMan(NULL);

	// 조명을 다 끄자 , 부분적으로만 켜지는게 있는거 같다
	DWORD lightPREV;
	pd3dDevice->GetRenderState(D3DRS_LIGHTING,&lightPREV);
	pd3dDevice->SetRenderState(D3DRS_LIGHTING,FALSE);

    V_RETURN(RenderBegin(pd3dDevice));

    m_pSavedSB->Capture();
    m_pDrawSB->Apply();

    D3DXMATRIX matViewPREV, matProjPREV;
    pd3dDevice->GetTransform( D3DTS_VIEW,		&matViewPREV );
    pd3dDevice->GetTransform( D3DTS_PROJECTION, &matProjPREV );

	DWORD dwAlphaBlendEnable;
	pd3dDevice->GetRenderState( D3DRS_ALPHABLENDENABLE,	&dwAlphaBlendEnable );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	FALSE );

	D3DXVECTOR3 vFromPt = DxViewPort::GetInstance().GetFromPt();

    V_RETURN(RenderMesh(pd3dDevice));

	// RollBack
	DxViewPort::GetInstance().SetFromPt( vFromPt );

	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	dwAlphaBlendEnable );

    pd3dDevice->SetTransform( D3DTS_VIEW,		&matViewPREV );
    pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProjPREV );

	pd3dDevice->SetRenderState(D3DRS_LIGHTING,lightPREV);

    m_pSavedSB->Apply();

    V_RETURN(RenderEnd(pd3dDevice));

	//COLLISION::bCollisionCheck = backupCollision;
	//DxEffSingleMan::GetInstance().SetLandMan(pBackupLand);


    return S_OK;
}

HRESULT CUIMeshRender::RenderBegin ( LPDIRECT3DDEVICEQ pd3dDevice )
{
    m_sBackupTarget.Backup( pd3dDevice );

    //pd3dDevice->SetRenderTarget ( 0, m_pTargetSurface );
    //pd3dDevice->SetDepthStencilSurface ( m_pTargetZBuf );
    //pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0L);

	pd3dDevice->SetRenderTarget ( 0, DxSurfaceTex::GetInstance().m_pTempSuf512 );
	pd3dDevice->SetDepthStencilSurface ( DxSurfaceTex::GetInstance().m_pShadowZBuf );
	pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0L);

    return S_OK;
}

HRESULT CUIMeshRender::RenderEnd ( LPDIRECT3DDEVICEQ pd3dDevice )
{
    m_sBackupTarget.Restore();

    // 버텍스 복사
    memcpy ( m_TargetUIVertex, m_UIVertex, sizeof(m_UIVertex) );
    m_TargetUIVertex[0].SetTexturePos(0.0f, 0.0f);
    m_TargetUIVertex[1].SetTexturePos(1.0f, 0.0f);
    m_TargetUIVertex[2].SetTexturePos(1.0f, 1.0f);
    m_TargetUIVertex[3].SetTexturePos(0.0f, 1.0f);

	DWORD dwMagFilter, dwMinFilter;
	pd3dDevice->GetSamplerState ( 0, D3DSAMP_MAGFILTER,		&dwMagFilter );
	pd3dDevice->GetSamplerState ( 0, D3DSAMP_MINFILTER,		&dwMinFilter );
    pd3dDevice->SetSamplerState ( 0, D3DSAMP_MAGFILTER,		D3DTEXF_LINEAR );
    pd3dDevice->SetSamplerState ( 0, D3DSAMP_MINFILTER,		D3DTEXF_LINEAR );
	{
		//// 타겟 텍스쳐를 그린다.
		//if( CUIRenderQueue::Get()->Render( m_pTargetTexture, m_TargetUIVertex ) )
		//	return S_OK;

		//HRESULT hr = pd3dDevice->SetTexture ( 0, m_pTargetTexture );
		//if ( FAILED ( hr ) )
		//	return hr;

		// 타겟 텍스쳐를 그린다.
		if( CUIRenderQueue::Get()->Render( DxSurfaceTex::GetInstance().m_pTempTex512, m_TargetUIVertex ) )
        {
            pd3dDevice->SetSamplerState ( 0, D3DSAMP_MAGFILTER,		dwMagFilter );
            pd3dDevice->SetSamplerState ( 0, D3DSAMP_MINFILTER,		dwMinFilter );

			return S_OK;
        }

		HRESULT hr = pd3dDevice->SetTexture ( 0, DxSurfaceTex::GetInstance().m_pTempTex512 );
		if ( FAILED ( hr ) )
        {
            pd3dDevice->SetSamplerState ( 0, D3DSAMP_MAGFILTER,		dwMagFilter );
            pd3dDevice->SetSamplerState ( 0, D3DSAMP_MINFILTER,		dwMinFilter );

			return hr;
        }

		hr = pd3dDevice->SetFVF ( UIVERTEX::D3DFVF_UIVERTEX );
		if ( FAILED ( hr ) )
        {
            pd3dDevice->SetSamplerState ( 0, D3DSAMP_MAGFILTER,		dwMagFilter );
            pd3dDevice->SetSamplerState ( 0, D3DSAMP_MINFILTER,		dwMinFilter );

			return hr;
        }

		hr = pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, (void*)m_TargetUIVertex, sizeof(UIVERTEX) );
		if ( FAILED ( hr ) )
        {
            pd3dDevice->SetSamplerState ( 0, D3DSAMP_MAGFILTER,		dwMagFilter );
            pd3dDevice->SetSamplerState ( 0, D3DSAMP_MINFILTER,		dwMinFilter );

			return hr;
        }
	}

	pd3dDevice->SetSamplerState ( 0, D3DSAMP_MAGFILTER,		dwMagFilter );
	pd3dDevice->SetSamplerState ( 0, D3DSAMP_MINFILTER,		dwMinFilter );

    return S_OK;
}

HRESULT CUIMeshRender::RenderMesh ( LPDIRECT3DDEVICEQ pd3dDevice )
{
    std::for_each(m_MeshMap.begin(), m_MeshMap.end(), MESHRDATAFUNC::RENDER(pd3dDevice));
    return S_OK;
}

void CUIMeshRender::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    if ( m_pd3dDevice )
    {
        std::for_each(m_MeshMap.begin(), m_MeshMap.end(), MESHRDATAFUNC::FRAMEMOVE(m_pd3dDevice, fElapsedTime));
    }

    CUIControl::Update(x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl);
}

void CUIMeshRender::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
    CUIControl::TranslateUIMessage(ControlID, dwMsg);
}

CUIMeshData_SkinChar* CUIMeshRender::LoadSkinChar ( const MESH_ID nID, const std::string& szFile )
{
 //   GASSERT( m_pd3dDevice&&"CUIMeshRender::InsertSkinChar" );
	GASSERT(m_pd3dDevice);
    CUIMeshData_SkinChar* pData = new CUIMeshData_SkinChar;
	LoadHelper(pData,nID,szFile);
    return pData;
}

IUIMeshData* CUIMeshRender::FindMeshData ( const MESH_ID nID )
{
    IUIMESHDATAMAPITER iter = m_MeshMap.find(nID);
    if ( iter == m_MeshMap.end() )
    {
        return NULL;
    }

    return iter->second;
}


void CUIMeshRender::DeleteMeshData( const MESH_ID nID )
{
	IUIMESHDATAMAPITER iter = m_MeshMap.find(nID);
	if ( iter == m_MeshMap.end() )
	{
		return;
	}

	delete iter->second;
	m_MeshMap.erase(iter);
}


CUIMeshData_SkinVehicle* CUIMeshRender::LoadSkinVehicle( const MESH_ID nID, const std::string& szFile )
{
	GASSERT(m_pd3dDevice);
	CUIMeshData_SkinVehicle* pData = new CUIMeshData_SkinVehicle;
	LoadHelper(pData,nID,szFile);
	return pData;
}

void CUIMeshRender::LoadHelper( IUIMeshData* pData,const MESH_ID nID, const std::string& szFile )
{
	if ( FAILED( pData->InitDeviceObjects( m_pd3dDevice ) ) )
	{
		SAFE_DELETE(pData);
		GASSERT(false);
	}

	if ( FAILED( pData->RestoreDeviceObjects( m_pd3dDevice ) ) )
	{
		SAFE_DELETE(pData);
		GASSERT(false);
	}

	if ( !pData->Load( m_pd3dDevice, szFile ) )
	{
		SAFE_DELETE(pData);
		GASSERT(false);
	}

	// Note : 기존 ID에 있다면 삭제
	IUIMESHDATAMAPITER iter = m_MeshMap.find(nID);
	if ( iter != m_MeshMap.end() )
	{
		SAFE_DELETE(iter->second);
		m_MeshMap.erase(iter);
	}

	std::pair<IUIMESHDATAMAPITER,bool> ret = m_MeshMap.insert( IUIMESHDATAPAIR(nID, pData) );

	if (ret.second == false)
	{
		SAFE_DELETE(pData);
		GASSERT(false);
	}
}
