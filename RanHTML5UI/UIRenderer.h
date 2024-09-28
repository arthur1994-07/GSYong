#pragma once

#include <GUIBase/IUIRenderer.h>
#include "UIWindowRender.h"

struct UIVERTEX;

namespace HTML5UI
{

class CUIRenderer : public GUIBase::IUIRenderer
{
	// Render State 값;
	struct SRENDER_STATE
	{	
		DWORD dwLight;
		DWORD dwNormalize;
		DWORD dwCullMode;
		DWORD dwMagFilter;
		DWORD dwMinFilter;
		D3DLIGHT9 Light;
	};

public:
	CUIRenderer();
	virtual ~CUIRenderer();

public:
	/// Render State 관리;
	virtual void SetRenderState();
	virtual void RecoverRenderState();

	/// Device Lost 대비;
	virtual bool InitDevice();
	virtual bool RestoreDevice();
	virtual bool InvalidateDevice();
	virtual bool DeleteDevice();

	/// Texture를 생성하는 과정;
	//! 파일을 이용해 Texture 그리기;
	virtual bool RenderTextureUsingFile(
		GUIBase::UIID uiID,
		const std::string& textureID,
		GUIBase::COMPONENT_POSITION& compPosition );

	//! WebView의 ImageData를 이용해 Texture 그리기;
	virtual bool RenderTextureUsingData(
		GUIBase::UIID uiID,
		GUIBase::UI_IMAGE_DATA& imgData,
		GUIBase::COMPONENT_POSITION& compPosition );

	/// 실제로 그리는 과정 ( WebView );
	//! 만들어진 UI 그리기;
	virtual void RenderUI(
		GUIBase::UIRECT rcGlobalPos,
		GUIBase::UIID uiID );

private:
	/// XML이용, Texture; 
	//! 텍스처 로딩;
	void LoadComponentTexture();

	//! 텍스처 언로딩;
	void UnloadComponentTexture();

	//! 텍스처 리로딩;
	void ReloadComponentTexture();

private:
	/// Utility 함수;
	//! 생성된 텍스처를 이용하여 그리기;
	bool RenderUsingTexture(
		GUIBase::UIRECT rcGlobalPos,
		LPDIRECT3DTEXTUREQ pTexture,
		WindowRender::TEXEL texel = WindowRender::TEXEL() );

	// UV 세팅;
	void SetTexturePos(
		UIVERTEX* vertex,
		const WindowRender::TEXEL& texel );

	// ImgData 구조체로 텍스처 생성;
	HRESULT CreateTextureUsingUpdateImg(
		std::tr1::shared_ptr< WindowRender::HTML_TEXTURE > htmlTexture,
		GUIBase::UI_IMAGE_DATA& imgData );

	// Low Data로 Texture 만들기 ( Update Texture );
	void PaintTextureToData(
		std::tr1::shared_ptr< WindowRender::HTML_TEXTURE > htmlTexture,
		GUIBase::UI_IMAGE_DATA& imgData );

	// 스크롤링;
	HRESULT Scrolling( std::tr1::shared_ptr< WindowRender::HTML_TEXTURE > htmlTexture
		, GUIBase::UI_IMAGE_DATA& imgData );

	// 스크롤링 텍스처 그리기;
	HRESULT PaintScrollingTexture(
		std::tr1::shared_ptr< WindowRender::HTML_TEXTURE > htmlTexture,
		const RECT& scrollRect,
		const POINT& scrollOffset );

public:
	// Devuce 세팅;
	inline void SetDevice( LPDIRECT3DDEVICEQ pDevice ) { m_pDevice = pDevice; }

private:
	//! Render State
	SRENDER_STATE m_sRenderState;

	//! Component Texture Map
	WindowRender::COMP_TEXTURE_MAP m_componentMap;

	//! UI Texture Map
	WindowRender::UI_TEXTURE_MAP m_uiMap;

	//! Device
	LPDIRECT3DDEVICEQ m_pDevice;
};

}