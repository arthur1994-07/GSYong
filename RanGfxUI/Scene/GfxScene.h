#pragma once

#include "../Core/GfxCore.h"
#include "../GfxInterface.h"

class GfxScene
{
public :
	GfxScene();
	virtual ~GfxScene();

protected :
	Ptr< Scaleform::GFx::MovieDef >			m_pMovieDef;
	Ptr< Scaleform::GFx::Movie >			m_pMovie;
	Scaleform::GFx::MovieDisplayHandle		m_hMovie;

	std::string m_strFilePath;

public :
	virtual HRESULT OnInit( LPDIRECT3DDEVICE9 pDevice ) = 0;
	virtual HRESULT OnResetDevice( LPDIRECT3DDEVICE9 pDevice );
	virtual HRESULT OnDestroyDevice();
	virtual HRESULT OnFrameMove( float fElapsedTime );
	virtual HRESULT OnFrameRender( float fElapsedTime );

public :
	// swf ·Îµå
	virtual HRESULT LoadMovie( LPDIRECT3DDEVICE9 pDevice );

public :
	GFx::Movie* GetMovie() { return m_pMovie; }
};
