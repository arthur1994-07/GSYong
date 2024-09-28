#pragma once

#include "../EngineLib/DxTools/TextureManager.h"
#include <map>

namespace HTML5UI
{

namespace WindowRender
{
	/// Component Texture
	struct COMPONENT_TEXTURE
	{
		TextureResource textureRes;
		LPDIRECT3DTEXTUREQ pTexture;
		GUIBase::UIRECT texPosition;

		COMPONENT_TEXTURE()
			: pTexture( NULL ) { }
	};

	/// Html Texture
	struct HTML_TEXTURE
	{
		LPDIRECT3DTEXTUREQ pTexture;			// 최종 Texture;
		LPDIRECT3DSURFACEQ pSurface;			// 최종 Surface;
		LPDIRECT3DTEXTUREQ pUpdateTexture;		// 업데이트 Texture;
		LPDIRECT3DTEXTUREQ pScrollTexture;		// 스크롤 Texture;
		LPDIRECT3DSURFACEQ pScrollSurface;		// 스크롤 Surface;

		HTML_TEXTURE()
			: pTexture( NULL )
			, pSurface( NULL )
			, pUpdateTexture( NULL )
			, pScrollTexture( NULL )
			, pScrollSurface( NULL ) { }

		~HTML_TEXTURE()
		{
			Clear();
		}

		void Clear()
		{
			SAFE_RELEASE( pSurface );
			SAFE_RELEASE( pTexture );
			SAFE_RELEASE( pScrollSurface );
			SAFE_RELEASE( pScrollTexture );
			SAFE_RELEASE( pUpdateTexture );
		}
	};

	/// UV
	struct TEXEL
	{
		float uStart;
		float uEnd;
		float vStart;
		float vEnd;

		TEXEL()
		{
			uStart = 0.f;
			uEnd = 1.f;
			vStart = 0.f;
			vEnd = 1.f;
		}
	};

	// WebKit과 관련없이 직접 그리는 Texture Map;
	typedef std::map< std::string, COMPONENT_TEXTURE >		COMP_TEXTURE_MAP;
	typedef COMP_TEXTURE_MAP::iterator						COMP_TEXTURE_MAP_ITER;
	typedef COMP_TEXTURE_MAP::value_type					COMP_TEXTURE_MAP_VALUE;

	// 각 WebPage(UI)의 Texture Map;
	typedef std::map< GUIBase::UIID, std::tr1::shared_ptr< HTML_TEXTURE > >		UI_TEXTURE_MAP;
	typedef UI_TEXTURE_MAP::iterator											UI_TEXTURE_MAP_ITER;
	typedef UI_TEXTURE_MAP::value_type											UI_TEXTURE_MAP_VALUE;
};

}