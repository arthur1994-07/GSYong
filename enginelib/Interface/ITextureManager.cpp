#include "pch.h"
#include "./ITextureManager.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

ITextureResource::ITextureResource( TextureResource textureRes )
    : m_textureRes( textureRes )
{
}

ITextureResource::~ITextureResource()
{
}

void ITextureResource::Clear()
{
    m_textureRes.Clear();
}

bool ITextureResource::IsValid() const
{
    return m_textureRes.IsValid();
}

const TSTRING& ITextureResource::GetFileName() const
{
    return m_textureRes.GetFileName();
}

bool ITextureResource::IsLoading()
{
    return m_textureRes.IsLoading();
}

bool ITextureResource::Update()
{
    return m_textureRes.Update();
}

LPDIRECT3DTEXTUREQ ITextureResource::GetTexture()
{
    return m_textureRes.GetTexture();
}

LPDIRECT3DCUBETEXTUREQ ITextureResource::GetCubeTexture()
{
    return m_textureRes.GetCubeTexture();
}

const TEXTURE_ADD_DATA& ITextureResource::GetAddData()
{
    return m_textureRes.GetAddData();
}

const D3DXIMAGE_INFO& ITextureResource::GetInfo()
{
    return m_textureRes.GetInfo();
}

ic::TextureResourceInterface ITextureManager::LoadTexture(
    const TSTRING& pathFileName,
	bool bMustDirectory,
	TEXTURE_ADD_DATA::TYPE textureType,
	DWORD mipMap,
	D3DFORMAT d3dFormat,
	TEXTURE_RESIZE resizeType,
    TEXTURE_CATEGORY emCategory,
	bool bBackgroundLoading,
	BOOL bBackUpCache )
{
    return ic::TextureResourceInterface(
        ic::TextureResourceInterface::SP_RESOURCE_INTERFACE(
            new ITextureResource(
                TextureManager::GetInstance().LoadTexture(
                    pathFileName,
	                bMustDirectory,
	                textureType,
	                mipMap,
	                d3dFormat,
	                resizeType,
                    emCategory,
	                bBackgroundLoading,
					bBackUpCache ) ) ) );
}

void ITextureManager::DeleteData( const TSTRING& pathFileName )
{
    TextureManager::GetInstance().DeleteData( pathFileName );
}