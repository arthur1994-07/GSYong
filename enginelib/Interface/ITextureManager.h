#pragma once

#include "../../InternalCommonLib/Interface/TextureManagerInterface.h"
#include "../DxTools/TextureManager.h"

struct ITextureResource : public ic::TextureResourceInterface
{
private:
    TextureResource m_textureRes;

public:
    ITextureResource( TextureResource textureRes );
    virtual ~ITextureResource();

public:
    virtual void Clear();
    virtual bool IsValid() const;
    virtual const TSTRING& GetFileName() const;
    virtual bool IsLoading();
    virtual bool Update();

    virtual LPDIRECT3DTEXTUREQ GetTexture();
    virtual LPDIRECT3DCUBETEXTUREQ GetCubeTexture();
    virtual const TEXTURE_ADD_DATA& GetAddData();
    virtual const D3DXIMAGE_INFO& GetInfo();
};

class ITextureManager : public ic::TextureManagerInterface
{
public:
    virtual ic::TextureResourceInterface LoadTexture(
        const TSTRING& pathFileName,
		bool bMustDirectory,
		TEXTURE_ADD_DATA::TYPE textureType,
		DWORD mipMap,
		D3DFORMAT d3dFormat,
		TEXTURE_RESIZE resizeType,
        TEXTURE_CATEGORY emCategory,
		bool bBackgroundLoading,
		BOOL bBackUpCache );

    virtual void DeleteData( const TSTRING& pathFileName );
};