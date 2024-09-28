#include "StdAfx.h"
#include "UIRenderManager.h"

namespace GUIBase
{

CUIRenderManager& CUIRenderManager::GetInstance()
{
	static CUIRenderManager instance;
	return instance;
}

//! 세팅 Render State;
void CUIRenderManager::SetRenderState()
{
	if( m_renderer )
		m_renderer->SetRenderState();
}

//! 회복 Render State;
void CUIRenderManager::RecoverRenderState()
{
	if( m_renderer )
		m_renderer->RecoverRenderState();
}

//! Device
bool CUIRenderManager::InitDevice()
{
	if( m_renderer )
		return m_renderer->InitDevice();

	return true;
}

bool CUIRenderManager::RestoreDevice()
{
	if( m_renderer )
		return m_renderer->RestoreDevice();

	return true;
}

bool CUIRenderManager::InvalidateDevice()
{
	if( m_renderer )
		return m_renderer->InvalidateDevice();

	return true;
}

bool CUIRenderManager::DeleteDevice()
{
	if( m_renderer )
		return m_renderer->DeleteDevice();

	return true;
}

//! 파일을 이용해 Texture 그리기;
bool CUIRenderManager::RenderTextureUsingFile(
	UIID uiID,
	const std::string& textureID,
	COMPONENT_POSITION& compPosition )
{
	if( m_renderer )
	{
		return m_renderer->RenderTextureUsingFile(
			uiID, textureID, compPosition );
	}
	return false;
}

//! size 크기의 Texture에 Image Data 갱신시키기;
bool CUIRenderManager::RenderTextureUsingData(
	UIID uiID,
	UI_IMAGE_DATA& imgData,
	COMPONENT_POSITION& compPosition )
{
	if( m_renderer )
	{
		return m_renderer->RenderTextureUsingData(
			uiID, imgData, compPosition );
	}

	return false;
}

//! 만들어진 UI 그리기;
void CUIRenderManager::RenderUI(  UIRECT rcGlobalPos
								, UIID uiID )
{
	if( m_renderer )
		m_renderer->RenderUI( rcGlobalPos, uiID );
}

}