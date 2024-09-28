#pragma once

#include <GUIBase/IUIRenderer.h>
#include "UIWindowRender.h"

struct UIVERTEX;

namespace HTML5UI
{

class CUIRenderer : public GUIBase::IUIRenderer
{
	// Render State ��;
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
	/// Render State ����;
	virtual void SetRenderState();
	virtual void RecoverRenderState();

	/// Device Lost ���;
	virtual bool InitDevice();
	virtual bool RestoreDevice();
	virtual bool InvalidateDevice();
	virtual bool DeleteDevice();

	/// Texture�� �����ϴ� ����;
	//! ������ �̿��� Texture �׸���;
	virtual bool RenderTextureUsingFile(
		GUIBase::UIID uiID,
		const std::string& textureID,
		GUIBase::COMPONENT_POSITION& compPosition );

	//! WebView�� ImageData�� �̿��� Texture �׸���;
	virtual bool RenderTextureUsingData(
		GUIBase::UIID uiID,
		GUIBase::UI_IMAGE_DATA& imgData,
		GUIBase::COMPONENT_POSITION& compPosition );

	/// ������ �׸��� ���� ( WebView );
	//! ������� UI �׸���;
	virtual void RenderUI(
		GUIBase::UIRECT rcGlobalPos,
		GUIBase::UIID uiID );

private:
	/// XML�̿�, Texture; 
	//! �ؽ�ó �ε�;
	void LoadComponentTexture();

	//! �ؽ�ó ��ε�;
	void UnloadComponentTexture();

	//! �ؽ�ó ���ε�;
	void ReloadComponentTexture();

private:
	/// Utility �Լ�;
	//! ������ �ؽ�ó�� �̿��Ͽ� �׸���;
	bool RenderUsingTexture(
		GUIBase::UIRECT rcGlobalPos,
		LPDIRECT3DTEXTUREQ pTexture,
		WindowRender::TEXEL texel = WindowRender::TEXEL() );

	// UV ����;
	void SetTexturePos(
		UIVERTEX* vertex,
		const WindowRender::TEXEL& texel );

	// ImgData ����ü�� �ؽ�ó ����;
	HRESULT CreateTextureUsingUpdateImg(
		std::tr1::shared_ptr< WindowRender::HTML_TEXTURE > htmlTexture,
		GUIBase::UI_IMAGE_DATA& imgData );

	// Low Data�� Texture ����� ( Update Texture );
	void PaintTextureToData(
		std::tr1::shared_ptr< WindowRender::HTML_TEXTURE > htmlTexture,
		GUIBase::UI_IMAGE_DATA& imgData );

	// ��ũ�Ѹ�;
	HRESULT Scrolling( std::tr1::shared_ptr< WindowRender::HTML_TEXTURE > htmlTexture
		, GUIBase::UI_IMAGE_DATA& imgData );

	// ��ũ�Ѹ� �ؽ�ó �׸���;
	HRESULT PaintScrollingTexture(
		std::tr1::shared_ptr< WindowRender::HTML_TEXTURE > htmlTexture,
		const RECT& scrollRect,
		const POINT& scrollOffset );

public:
	// Devuce ����;
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