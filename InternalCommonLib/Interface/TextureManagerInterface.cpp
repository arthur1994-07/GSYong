#include "stdafx.h"
#include "./TextureManagerInterface.h"

// ----------------------------------------------------------------------------
#include "../debuginclude.h"
// ----------------------------------------------------------------------------

namespace ic
{
	static TSTRING s_strTemp;
	static TEXTURE_ADD_DATA m_sTextureAddDataTemp;
	static D3DXIMAGE_INFO m_sD3DXImageInfoTemp;

    TextureResourceInterface::TextureResourceInterface( SP_RESOURCE_INTERFACE spInterface )
        : m_spInterface( spInterface )
    {
		memset( &m_sD3DXImageInfoTemp, 0, sizeof( m_sD3DXImageInfoTemp ) );
    }

    TextureResourceInterface::~TextureResourceInterface()
    {
    }

    void TextureResourceInterface::Clear()
    {
        if( m_spInterface.get() )
			m_spInterface->Clear();
    }

    bool TextureResourceInterface::IsValid() const
    {
        if( m_spInterface.get() )
			return m_spInterface->IsValid();

		return false;
    }

    const TSTRING& TextureResourceInterface::GetFileName() const
    {
        if( m_spInterface.get() )
			return m_spInterface->GetFileName();

		return s_strTemp;
    }

    bool TextureResourceInterface::IsLoading( const TSTRING& fineName )
    {
        if( m_spInterface.get() )
			return m_spInterface->IsLoading( fineName );

		return false;
    }

    bool TextureResourceInterface::Update()
    {
        if( m_spInterface.get() )
			return m_spInterface->Update();

		return false;
    }

    LPDIRECT3DTEXTUREQ TextureResourceInterface::GetTexture()
    {
        if( m_spInterface.get() )
			return m_spInterface->GetTexture();

		return NULL;
    }

    LPDIRECT3DCUBETEXTUREQ TextureResourceInterface::GetCubeTexture()
    {
        if( m_spInterface.get() )
			return m_spInterface->GetCubeTexture();

		return NULL;
    }

    const TEXTURE_ADD_DATA& TextureResourceInterface::GetAddData()
    {
        if( m_spInterface.get() )
			return m_spInterface->GetAddData();

		return m_sTextureAddDataTemp;
    }

    const D3DXIMAGE_INFO& TextureResourceInterface::GetInfo()
    {
        if( m_spInterface.get() )
			return m_spInterface->GetInfo();

		return m_sD3DXImageInfoTemp;
    }

    TextureManagerInterface* TextureManagerInterface::g_pInstance = NULL;

    void TextureManagerInterface::SetInstance( TextureManagerInterface* pInstance )
    { 
        g_pInstance = pInstance; 
    }

    TextureManagerInterface* TextureManagerInterface::GetInstance()
    { 
        return g_pInstance;
    }

    TextureManagerInterface::TextureManagerInterface()
    {
    }

    TextureManagerInterface::~TextureManagerInterface()
    {
    }

};