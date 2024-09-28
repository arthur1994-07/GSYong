#include "pch.h"

#include "../DxTools/DxFontMan.h"
#include "../DxTools/TextureManager.h"
#include "../TextTexture/TextUtil.h"
#include "../DxResponseMan.h"

#include "./CustomDevice.h"

#include "../../SigmaCore/DebugInclude.h"

CCustomDevice* g_pCustomDevice = NULL;

CCustomDevice::CCustomDevice(
	EMSERVICE_PROVIDER eProvider,
	SUBPATH* pSubPath,
	const CString& strPath,
	CWnd* pParent,
	const CString& strFontType,
	language::LANGFLAG emLangSet,
	const CString& strGDIFont )
	: m_pEngineDevice( new DxResponseMan( eProvider, pSubPath ) )
	, m_strPath( strPath )
	, m_pParent( pParent )
	, m_strFontType( strFontType )
	, m_emLangSet( emLangSet )
	, m_strGDIFont( strGDIFont )
{
}

CCustomDevice::~CCustomDevice()
{
	SAFE_DELETE( m_pEngineDevice );
}

HRESULT CCustomDevice::OnDeviceCreated(
    IDirect3DDevice9* pd3dDevice,
    const D3DSURFACE_DESC* pBackBufferSurfaceDesc )
{
    CTextUtil::m_bUsage = TRUE;

	m_pEngineDevice->OneTimeSceneInit(
        m_strPath.GetString(),
		m_pParent,
        m_strFontType,
        false,
        m_emLangSet,
        m_strGDIFont );	

    DxFontMan::GetInstance().InitDeviceObjects( pd3dDevice );

    m_pEngineDevice->InitDeviceObjects( pd3dDevice, FALSE );

	return S_OK;
}

VOID CCustomDevice::OnDeviceDestroyed()
{
    m_pEngineDevice->DeleteDeviceObjects();

	m_pEngineDevice->FinalCleanup();
}

HRESULT CCustomDevice::OnDeviceReset(
    IDirect3DDevice9* pd3dDevice,
    const D3DSURFACE_DESC* pBackBufferSurfaceDesc )
{
    m_pEngineDevice->RestoreDeviceObjects();

	return S_OK;
}

VOID CCustomDevice::OnDeviceLost()
{
    m_pEngineDevice->InvalidateDeviceObjects();
}

VOID CCustomDevice::OnFrameMove(
    IDirect3DDevice9* pd3dDevice,
    DOUBLE fTime,
    FLOAT fElapsedTime )
{
	m_pEngineDevice->FrameMove( FLOAT( fTime ), fElapsedTime, FALSE );
}

VOID CCustomDevice::OnFrameRender(
    IDirect3DDevice9* pd3dDevice,
    DOUBLE fTime,
    FLOAT fElapsedTime )
{
	CTextUtil::Get()->FrameMove();
}