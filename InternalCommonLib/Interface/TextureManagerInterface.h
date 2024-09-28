#pragma once

#include <boost/tr1/memory.hpp>
#include "../../../RanThirdParty/DirectX/Include/d3dx9tex.h"
#include "../Define/TextureAddData.h"

namespace ic
{

struct TextureResourceInterface
{
public:
    typedef std::tr1::shared_ptr< TextureResourceInterface > SP_RESOURCE_INTERFACE;

protected:
    SP_RESOURCE_INTERFACE m_spInterface;

public:
    TextureResourceInterface( SP_RESOURCE_INTERFACE spInterface = SP_RESOURCE_INTERFACE() );
    virtual ~TextureResourceInterface();

public:
    virtual void Clear();
    virtual bool IsValid() const;
    virtual const TSTRING& GetFileName() const;
    virtual bool IsLoading( const TSTRING& fineName );
    virtual bool Update();

    virtual LPDIRECT3DTEXTUREQ GetTexture();
    virtual LPDIRECT3DCUBETEXTUREQ GetCubeTexture();
    virtual const TEXTURE_ADD_DATA& GetAddData();
    virtual const D3DXIMAGE_INFO& GetInfo();
};

class TextureManagerInterface
{
public:
    static TextureManagerInterface* g_pInstance;

public:
    static void SetInstance( TextureManagerInterface* pInstance );
    static TextureManagerInterface* GetInstance();

public:
    virtual TextureResourceInterface LoadTexture(
        const TSTRING& pathFileName,
		bool bMustDirectory,
		TEXTURE_ADD_DATA::TYPE textureType,
		DWORD mipMap,
		D3DFORMAT d3dFormat,
		TEXTURE_RESIZE resizeType,
        TEXTURE_CATEGORY emCategory,
		bool bBackgroundLoading,
		BOOL bBackUpCache ) = 0;

    virtual void DeleteData( const TSTRING& pathFileName ) = 0;

public:
    TextureManagerInterface();
    virtual ~TextureManagerInterface();
};

}