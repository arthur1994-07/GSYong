#pragma once

#include "IUIRenderer.h"

namespace GUIBase
{

class CUIRenderManager
{
public:
	static CUIRenderManager& GetInstance();

public:
	//! 세팅 Render State;
	void SetRenderState();

	//! 회복 Render State;
	void RecoverRenderState();

	//! Device
	bool InitDevice();
	bool RestoreDevice();
	bool InvalidateDevice();
	bool DeleteDevice();

	//! 파일을 이용해 Texture 그리기;
	bool RenderTextureUsingFile(
		UIID uiID,
		const std::string& textureID,
		COMPONENT_POSITION& compPosition );

	//! size 크기의 Texture에 Image Data 갱신시키기;
	bool RenderTextureUsingData(
		UIID uiID,
		UI_IMAGE_DATA& imgData,
		COMPONENT_POSITION& compPosition );

	//! 만들어진 UI 그리기;
	void RenderUI( UIRECT rcGlobalPos
		, UIID uiID );

	//! 해상도;
	inline const UISIZE<int>& GetResolution() const { return m_resolution; }
	inline void SetResolution( UISIZE<int> resolution ) { m_resolution = resolution; }

public:
	inline void SetRenderer( IUIRenderer* renderer )
	{
		SAFE_DELETE( m_renderer );
		m_renderer = renderer;
	}

private:
	CUIRenderManager() : m_renderer( NULL ) { }
	~CUIRenderManager() { SAFE_DELETE( m_renderer ); }

private:
	IUIRenderer* m_renderer;

	// 해상도;
	UISIZE<int> m_resolution;
};

}