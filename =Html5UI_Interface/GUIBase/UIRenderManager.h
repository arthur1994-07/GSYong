#pragma once

#include "IUIRenderer.h"

namespace GUIBase
{

class CUIRenderManager
{
public:
	static CUIRenderManager& GetInstance();

public:
	//! ���� Render State;
	void SetRenderState();

	//! ȸ�� Render State;
	void RecoverRenderState();

	//! Device
	bool InitDevice();
	bool RestoreDevice();
	bool InvalidateDevice();
	bool DeleteDevice();

	//! ������ �̿��� Texture �׸���;
	bool RenderTextureUsingFile(
		UIID uiID,
		const std::string& textureID,
		COMPONENT_POSITION& compPosition );

	//! size ũ���� Texture�� Image Data ���Ž�Ű��;
	bool RenderTextureUsingData(
		UIID uiID,
		UI_IMAGE_DATA& imgData,
		COMPONENT_POSITION& compPosition );

	//! ������� UI �׸���;
	void RenderUI( UIRECT rcGlobalPos
		, UIID uiID );

	//! �ػ�;
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

	// �ػ�;
	UISIZE<int> m_resolution;
};

}